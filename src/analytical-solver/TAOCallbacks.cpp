#include "TAOCallbacks.h"

#include <cassert>
#include <numeric>

#include "AnalyticalSolver.h"
#include "PrintingUtils.h"
#include "TAOPrivate.h"

namespace DungeonGeneration {
namespace AnalyticalSolver {

namespace {

/// Setup penalty factor mu that was overriden in the process of TAO configuration.
void almmOverrideMu(Tao almmSolver, int iterNum)
{
    if (iterNum == 0) {
        TAO_ALMM* almmData = reinterpret_cast<TAO_ALMM*>(almmSolver->data);
        almmData->mu = 0;
        almmData->mu_fac = 25;
    } else if (iterNum == 1) {
        // ALMM recalculates penalty as mu *= mu_fac. We set mu = 0 at the first iteration, so we need to set it to 1.
        TAO_ALMM* almmData = reinterpret_cast<TAO_ALMM*>(almmSolver->data);
        almmData->mu = 1;
    }
}

}  // namespace

PetscErrorCode evaluateCostFunctionGradient(Tao almmSolver, Vec xVec, double* f, Vec gVec, void* ctx)
{
    PetscFunctionBegin;

    AnalyticalSolver* solver = reinterpret_cast<AnalyticalSolver*>(ctx);
    const size_t varCnt = solver->varCnt_;

    // Zero out result variables
    *f = 0.0;
    PetscCall(VecSet(gVec, 0.0));

    const double* xArr;
    double* gradArr;
    PetscCall(VecGetArrayRead(xVec, &xArr));
    PetscCall(VecGetArray(gVec, &gradArr));

    for (const Callbacks::FGEval& costFunction : solver->costFunctions_) {
        costFunction(xArr, *f, gradArr);
    }

    PetscCall(VecRestoreArrayRead(xVec, &xArr));
    PetscCall(VecRestoreArray(gVec, &gradArr));

    PetscFunctionReturn(PETSC_SUCCESS);
}

PetscErrorCode evaluateEqualityConstraintsFunction(Tao almmSolver, Vec xVec, Vec cEqVec, void* ctx)
{
    PetscFunctionBegin;

    AnalyticalSolver* solver = reinterpret_cast<AnalyticalSolver*>(ctx);
    const size_t varCnt = solver->varCnt_;
    const size_t cEqCnt = solver->cEqCnt_;

    const double* xArr;
    double* cEqArr;
    PetscCall(VecGetArrayRead(xVec, &xArr));
    PetscCall(VecSet(cEqVec, 0.0));
    PetscCall(VecGetArray(cEqVec, &cEqArr));

#ifndef NDEBUG
    for (size_t i = 0; i < cEqCnt; ++i) {
        assert(abs(cEqArr[i]) < 1e-9);
    }
#endif

    // Calculate both constraints values and Jacobian
    std::vector<double> JEqArr(cEqCnt * varCnt);
    for (size_t fcnId = 0; fcnId < cEqCnt; ++fcnId) {
        double* cEqGradArr = &JEqArr[fcnId * varCnt];  // This is where relevant JEq row starts
        solver->equalityConstraints_[fcnId](xArr, cEqArr[fcnId], cEqGradArr);
    }

    // Set Jacobian values
    Mat JEq = solver->JEq_;
    const PetscInt* rowIndexes = solver->JEqRowIndexes_.data();
    const PetscInt* colIndexes = solver->JEqColIndexes_.data();
    assert(rowIndexes && "Null row indicies");
    assert(colIndexes && "Null col indicies");
    PetscCall(MatSetValues(JEq, cEqCnt, rowIndexes, varCnt, colIndexes, JEqArr.data(), INSERT_VALUES));
    PetscCall(MatAssemblyBegin(JEq, MAT_FINAL_ASSEMBLY));
    PetscCall(MatAssemblyEnd(JEq, MAT_FINAL_ASSEMBLY));

    PetscCall(VecRestoreArrayRead(xVec, &xArr));
    PetscCall(VecRestoreArray(cEqVec, &cEqArr));

    PetscFunctionReturn(PETSC_SUCCESS);
}

PetscErrorCode evaluateEqualityConstraintsJacobian(Tao almmSolver, Vec xVec, Mat JEq, Mat JEqPre, void* ctx)
{
    PetscFunctionBegin;

    // We just do nothing. JEq was already calculated in evaluateEqualityConstraintsFunction.

    PetscFunctionReturn(PETSC_SUCCESS);
}

PetscErrorCode almmConvergenceTest(Tao almmSolver, void* ctx)
{
    PetscFunctionBegin;

    AnalyticalSolver* solver = reinterpret_cast<AnalyticalSolver*>(ctx);

    // Get solver info
    double LGradNorm, cnorm, gatol, catol;
    PetscInt iterNum, maxIterCount;
    PetscCall(TaoGetSolutionStatus(almmSolver, &iterNum, nullptr, &LGradNorm, &cnorm, nullptr, nullptr));
    PetscCall(TaoGetTolerances(almmSolver, &gatol, nullptr, nullptr));
    PetscCall(TaoGetConstraintTolerances(almmSolver, &catol, nullptr));
    PetscCall(TaoGetMaximumIterations(almmSolver, &maxIterCount));

    // Override mu factor
    almmOverrideMu(almmSolver, iterNum);

    // Do custom convergence checks
    TaoConvergedReason reason = TAO_CONTINUE_ITERATING;
    if (iterNum >= maxIterCount) {
        reason = TAO_DIVERGED_MAXITS;
    } else if (LGradNorm < gatol && cnorm < catol) {
        // It seems weird to check LGradNorm here -- shouldn't BQNLS only stop when gradient norm is < gatol?
        // But for some reason in TAO implementation they force gatol = catol, so it seems to be important.
        reason = TAO_CONVERGED_GATOL;
    }
    PetscCall(TaoSetConvergedReason(almmSolver, reason));

    // Run callbacks that should be ran after each iteration
    if (iterNum > 0) {
        PetscCall(solver->runCallbacks(iterNum));
    }

    PetscFunctionReturn(PETSC_SUCCESS);
}

PetscErrorCode monitorALMM(Tao almmSolver, void* ctx)
{
    using namespace PrintingUtils;

    PetscFunctionBegin;

    AnalyticalSolver* solver = reinterpret_cast<AnalyticalSolver*>(ctx);
    const size_t varCnt = solver->varCnt_;
    const size_t cEqCnt = solver->cEqCnt_;
    Tao subsolver = solver->almmSubsolver_;

    // Get basic TAO stats
    PetscInt iterNum;
    PetscReal costFuncVal, Lgnorm, cnorm, xdiff;
    PetscCall(TaoGetSolutionStatus(almmSolver, &iterNum, &costFuncVal, &Lgnorm, &cnorm, &xdiff, nullptr));
    const bool isIterSuccessful = (xdiff == 1.0);  // Whether constraints were lowered enough
    TaoConvergedReason subsolverReason;
    PetscCall(TaoGetConvergedReason(subsolver, &subsolverReason));

    // Get ALMM multipliers (both primal and slack apparently)
    int multipliersCnt;
    Vec multipliersVec;
    PetscCall(TaoALMMGetMultipliers(almmSolver, &multipliersVec));
    PetscCall(VecGetSize(multipliersVec, &multipliersCnt));

    // Get ALMM guts (uses private TAO declarations)
    TAO_ALMM* almmData = reinterpret_cast<TAO_ALMM*>(almmSolver->data);

    const std::string padding = "                 ";
    if (iterNum == 0) {
        // Inner parameters of ALMM solver will lie, so don't print them.
        // Just print info about function values
        std::cerr << "ALMM Monitoring: iteration: " << iterNum << ", Lgnorm: " << Lgnorm
                  << ", cost function: " << costFuncVal << ", cnorm: " << cnorm << "\n";
        PetscFunctionReturn(PETSC_SUCCESS);
    }

    // Print ALMM solver's info
    std::cerr << "ALMM Monitoring: iteration: " << iterNum << ", Lgnorm: " << Lgnorm
              << ", cost function: " << costFuncVal << ", cnorm: " << cnorm << "\n"
              << padding << "auglag: ytol " << almmData->ytol << ", mu " << almmData->mu << ", subsolver gtol "
              << almmData->gtol << ", is iter successful: " << isIterSuccessful << "\n";
    if (multipliersCnt != 0 && static_cast<size_t>(multipliersCnt) <= ARRAY_PRINT_LIMIT) {
        // Print Lagrangian multipliers
        const double* multArr;
        PetscCall(VecGetArrayRead(multipliersVec, &multArr));
        std::cerr << padding << "multipliers: " << arrayToString(multArr, multipliersCnt) << "\n";
        PetscCall(VecRestoreArrayRead(multipliersVec, &multArr));
    }
    std::cerr << padding << "----------\n";

    if (iterNum != 0) {
        // Print subsolver's info
        PetscInt nfuncs;
        PetscCall(TaoGetCurrentFunctionEvaluations(subsolver, &nfuncs));
        std::cerr << padding << "subsolver's iter count: " << subsolver->niter << ", nfuncs: " << nfuncs
                  << ", converged reason " << TaoConvergedReasons[subsolverReason] << "\n";
        if (subsolver->reason < 0) {
            std::cerr << padding << "[!!!] Subsolver diverged!\n";
        }
        std::cerr << padding << "----------\n";
    }

    // Print variables and functions
    if (varCnt <= ARRAY_PRINT_LIMIT) {
        const double* x;
        PetscCall(VecGetArrayRead(solver->x_, &x));
        Vec LgradX;
        PetscCall(TaoALMMGetMultipliers(almmSolver, &LgradX));
        const double* LgradXArr;
        PetscCall(VecGetArrayRead(LgradX, &LgradXArr));

        std::cerr << padding << "solution: " << arrayToString(x, varCnt) << "\n";
        std::cerr << padding << "LgradX: " << arrayToString(LgradXArr, varCnt) << "\n";

        PetscCall(VecRestoreArrayRead(solver->x_, &x));
        PetscCall(VecRestoreArrayRead(LgradX, &LgradXArr));
    }
    if (cEqCnt != 0 && cEqCnt <= ARRAY_PRINT_LIMIT) {
        const double* cEq;
        PetscCall(VecGetArrayRead(solver->cEq_, &cEq));

        const PetscInt* rowIndexes = solver->JEqRowIndexes_.data();
        const PetscInt* colIndexes = solver->JEqColIndexes_.data();
        assert(rowIndexes && "Null row indicies");
        assert(colIndexes && "Null col indicies");
        std::vector<double> JEqVal(cEqCnt * varCnt);
        PetscCall(MatGetValues(solver->JEq_, cEqCnt, rowIndexes, varCnt, colIndexes, JEqVal.data()));

        const std::string JEqPadding = "                      ";
        std::cerr << padding << "cEq: " << arrayToString(cEq, solver->cEqCnt_) << "\n";
        std::cerr << padding << "JEq: " << matrixToString(JEqVal.data(), cEqCnt, varCnt, JEqPadding) << "\n";

        PetscCall(VecRestoreArrayRead(solver->cEq_, &cEq));
    }

    PetscFunctionReturn(PETSC_SUCCESS);
}

PetscErrorCode monitorSubsolver(Tao subsolver, void* ctx)
{
    using namespace PrintingUtils;
    PetscFunctionBegin;

    AnalyticalSolver* solver = reinterpret_cast<AnalyticalSolver*>(ctx);
    const size_t varCnt = solver->varCnt_;

    // Get solving status
    PetscInt iterNum;
    PetscReal f, gnorm, xdiff;
    PetscCall(TaoGetSolutionStatus(subsolver, &iterNum, &f, &gnorm, nullptr, &xdiff, nullptr));

    // Print simple solver's info
    const std::string padding = "                      ";
    std::cerr << "Subsolver monitoring: iteration: " << iterNum << ", f: " << f << ", gnorm: " << gnorm
              << ", xdiff: " << xdiff << "\n";

    if (varCnt <= ARRAY_PRINT_LIMIT) {
        // Print solution, gradient and hessian
        const double* x;
        const double* xGrad;
        PetscCall(VecGetArrayRead(subsolver->solution, &x));
        PetscCall(VecGetArrayRead(subsolver->gradient, &xGrad));

        Mat hessianLMVM, hessianDense;
        PetscCall(TaoGetLMVMMatrix(subsolver, &hessianLMVM));
        PetscCall(MatConvert(hessianLMVM, MATDENSE, MAT_INITIAL_MATRIX, &hessianDense));
        std::vector<double> hessianArr(varCnt * varCnt);
        std::vector<int> indexes(varCnt);
        std::iota(indexes.begin(), indexes.end(), 0);
        PetscCall(MatGetValues(hessianDense, varCnt, indexes.data(), varCnt, indexes.data(), hessianArr.data()));

        const std::string hessianPadding = "                               ";
        std::cerr << padding << "solution: " << arrayToString(x, varCnt) << "\n";
        std::cerr << padding << "gradient: " << arrayToString(xGrad, varCnt) << "\n";
        std::cerr << padding << "Hessian: " << matrixToString(hessianArr.data(), varCnt, varCnt, hessianPadding)
                  << "\n";

        PetscCall(VecRestoreArrayRead(subsolver->solution, &x));
        PetscCall(VecRestoreArrayRead(subsolver->gradient, &xGrad));
    }

    PetscFunctionReturn(PETSC_SUCCESS);
}

}  // namespace AnalyticalSolver
}  // namespace DungeonGeneration

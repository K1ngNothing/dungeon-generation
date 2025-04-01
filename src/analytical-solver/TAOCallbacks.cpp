#include "TAOCallbacks.h"

#include <cassert>
#include <numeric>

#include "AnalyticalSolver.h"
#include "PrintingUtils.h"
#include "TAOPrivate.h"

namespace DungeonGenerator {
namespace AnalyticalSolver {

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

    for (const Functions::FGEval& costFunction : solver->costFunctions_) {
        costFunction(xArr, *f, gradArr);
    }

    PetscCall(VecRestoreArrayRead(xVec, &xArr));
    PetscCall(VecRestoreArray(gVec, &gradArr));

    PetscFunctionReturn(PETSC_SUCCESS);
}

PetscErrorCode evaluateEqualityConstraintFunction(Tao almmSolver, Vec xVec, Vec cEqVec, void* ctx)
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

    // DEBUG
    for (size_t i = 0; i < cEqCnt; ++i) {
        assert(abs(cEqArr[i]) < 1e-9);
    }

    AnalyticalSolver::Matrix& JEqCache = solver->provideZeroedJEqCache();
    for (size_t fcnId = 0; fcnId < cEqCnt; ++fcnId) {
        solver->equalityConstraints_[fcnId](xArr, cEqArr[fcnId], JEqCache[fcnId].data());
    }

    PetscCall(VecRestoreArrayRead(xVec, &xArr));
    PetscCall(VecRestoreArray(cEqVec, &cEqArr));

    PetscFunctionReturn(PETSC_SUCCESS);
}

PetscErrorCode evaluateEqualityConstraintJacobian(Tao almmSolver, Vec xVec, Mat JEq, Mat JEqPre, void* ctx)
{
    PetscFunctionBegin;

    AnalyticalSolver* solver = reinterpret_cast<AnalyticalSolver*>(ctx);
    const size_t varCnt = solver->varCnt_;
    const size_t cEqCnt = solver->cEqCnt_;

    // TODO: maybe remain inside PETSc objects to avoid copying?
    const std::vector<double> JEqValue = solver->provideJEqCache();
    std::vector<PetscInt> rowIndexes(cEqCnt);
    std::vector<PetscInt> colIndexes(varCnt);
    std::iota(rowIndexes.begin(), rowIndexes.end(), 0);
    std::iota(colIndexes.begin(), colIndexes.end(), 0);
    PetscCall(MatSetValues(JEq, cEqCnt, rowIndexes.data(), varCnt, colIndexes.data(), JEqValue.data(), INSERT_VALUES));
    PetscCall(MatAssemblyBegin(JEq, MAT_FINAL_ASSEMBLY));
    PetscCall(MatAssemblyEnd(JEq, MAT_FINAL_ASSEMBLY));

    PetscFunctionReturn(PETSC_SUCCESS);
}

PetscErrorCode almmConvergenceTest(Tao almmSolver, void* ctx)
{
    PetscFunctionBegin;

    AnalyticalSolver* solver = reinterpret_cast<AnalyticalSolver*>(ctx);

    // Get solver info
    double LGradNorm, gatol;
    PetscInt iterNum, maxIterCount;
    PetscCall(TaoGetSolutionStatus(almmSolver, &iterNum, nullptr, &LGradNorm, nullptr, nullptr, nullptr));
    PetscCall(TaoGetTolerances(almmSolver, &gatol, nullptr, nullptr));
    PetscCall(TaoGetMaximumIterations(almmSolver, &maxIterCount));

    // Override some parameters of ALMM solver that can't be overwritten otherwise
    if (iterNum == 0) {
        TAO_ALMM* almmData = reinterpret_cast<TAO_ALMM*>(almmSolver->data);
        almmData->mu0 = 25;
        almmData->mu_fac = 25;
    }

    // (!) Do custom convergence checks
    TaoConvergedReason reason = TAO_CONTINUE_ITERATING;
    if (iterNum >= maxIterCount) {
        reason = TAO_DIVERGED_MAXITS;
    } else if (LGradNorm < gatol) {
        // TODO: it's seems weird to check gradient norm. Ideally subsolver should only stop if
        // gradient is close to zero, and therefore this check shouldn't check anything.
        // Nevertheless I will leave it for the time being, because it was working fine as is.

        // Check penalties
        const double* cEqArr;
        PetscCall(VecGetArrayRead(solver->cEq_, &cEqArr));

        bool allConstraintsSatisfied = true;
        constexpr double constraintTolerance = 1e-3;
        const size_t cEqCnt = solver->cEqCnt_;
        for (size_t i = 0; i < cEqCnt; ++i) {
            if (std::abs(cEqArr[i]) > constraintTolerance) {
                allConstraintsSatisfied = false;
                break;
            }
        }
        if (allConstraintsSatisfied) {
            reason = TAO_CONVERGED_GATOL;
        }

        PetscCall(VecRestoreArrayRead(solver->cEq_, &cEqArr));
    }
    PetscCall(TaoSetConvergedReason(almmSolver, reason));

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
    TAO_ALMM* almmData = reinterpret_cast<TAO_ALMM*>(almmSolver);

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
        const std::vector<double>& JEqCache = solver->provideJEqCache();

        const std::string JEqPadding = "                      ";
        std::cerr << padding << "cEq: " << arrayToString(cEq, solver->cEqCnt_) << "\n";
        std::cerr << padding << "JEq: " << matrixToString(JEqCache.data(), cEqCnt, varCnt, JEqPadding) << "\n";

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
}  // namespace DungeonGenerator

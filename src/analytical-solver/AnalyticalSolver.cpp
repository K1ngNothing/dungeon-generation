#include "AnalyticalSolver.h"

#include <cassert>
#include <iostream>

namespace DungeonGenerator {
namespace AnalyticalSolver {

AnalyticalSolver::AnalyticalSolver(
    size_t roomCnt, std::vector<Callbacks::FGEval>&& costFunctions,
    std::vector<Callbacks::FGEval>&& equalityConstraints, std::vector<Callbacks::ModifierCallback>&& modifierCallbacks,
    std::vector<Callbacks::ReaderCallback>&& readerCallbacks)
      : roomCnt_(roomCnt),
        varCnt_(roomCnt * 2),
        cEqCnt_(equalityConstraints.size()),
        JEqCache_(cEqCnt_, std::vector<double>(varCnt_)),
        costFunctions_(std::move(costFunctions)),
        equalityConstraints_(std::move(equalityConstraints)),
        modifierCallbacks_(std::move(modifierCallbacks)),
        readerCallbacks_(std::move(readerCallbacks))
{
    if (PetscInitializeNoArguments() != PETSC_SUCCESS) {
        throw std::runtime_error("AnalyticalSolver:: failed to initialize PETSc");
    }
    if (initializeTAOSolvers() != PETSC_SUCCESS) {
        throw std::runtime_error("AnalyticalSolver: failed to initialize TAO solvers");
    }
    if (initializeTAOContainers() != PETSC_SUCCESS) {
        throw std::runtime_error("AnalyticalSolver: failed to initialize containers for TAO solvers");
    }
    if (setContainersAndRoutines() != PETSC_SUCCESS) {
        throw std::runtime_error("AnalyticalSolver: failed to set containers and routines to TAO solvers");
    }
    if (setScaryOptionsInTAOSolvers() != PETSC_SUCCESS) {
        throw std::runtime_error("AnalyticalSolver: failed to set scary options for TAO solvers");
    }
}

AnalyticalSolver::~AnalyticalSolver()
{
    destroyTAOObjects();
}

void AnalyticalSolver::solve()
{
    std::cerr << "AnalyticalSolver: start solving...\n";
    if (TaoSolve(almmSolver_) != PETSC_SUCCESS) {
        throw std::runtime_error("AnalyticalSolver: error in TaoSolve for ALMM solver");
    }
    TaoConvergedReason convergedReason;
    if (TaoGetConvergedReason(almmSolver_, &convergedReason) != PETSC_SUCCESS) {
        // TODO: do better
        throw std::runtime_error("AnalyticalSolver: failed to retrieve converged reason");
    }
    std::cerr << "AnalyticalSolver: finished solving!\n"
              << "Converged reason: " << TaoConvergedReasons[convergedReason] << "\n";
}

Model::Positions AnalyticalSolver::retrieveSolution() const
{
    const double* xArr = nullptr;
    if (VecGetArrayRead(x_, &xArr) != PETSC_SUCCESS) {
        return {};
    }
    Model::Positions solution(roomCnt_);
    for (size_t i = 0; i < roomCnt_; ++i) {
        solution[i].x = xArr[i * 2];
        solution[i].y = xArr[i * 2 + 1];
    }
    static_cast<void>(VecRestoreArrayRead(x_, &xArr));
    return solution;
}

PetscErrorCode AnalyticalSolver::initializeTAOSolvers()
{
    PetscFunctionBegin;

    PetscCall(TaoCreate(MPI_COMM_SELF, &almmSolver_));
    PetscCall(TaoSetType(almmSolver_, TAOALMM));
    PetscCall(TaoALMMGetSubsolver(almmSolver_, &almmSubsolver_));

    constexpr size_t almmMaxIterCount = 25;
    // Theoretically for quadratic functions we should converge in varCnt_ iterations.
    // For practical functions it's safe to set the limit as some factor of varCnt_.
    const size_t subsolverMaxIterCount = 10 * varCnt_;
    const size_t subsolverMaxFcn = 10 * subsolverMaxIterCount;  // why?
    constexpr double almmGatol = 1e-3;                          // Absolute gradient tolerance
    constexpr double almmCatol = 1e-3;                          // Absolute constraint tolerance
    // TODO: for some reason PHR doesn't support gatol != catol. I can circumvent this by setting these constants inside
    // convergence test. For now I leave them equal because I don't see the problem with that.

    PetscCall(TaoALMMSetType(almmSolver_, TAO_ALMM_PHR));
    PetscCall(TaoSetType(almmSubsolver_, TAOBQNLS));
    PetscCall(TaoSetMaximumIterations(almmSolver_, almmMaxIterCount));
    PetscCall(TaoSetMaximumIterations(almmSubsolver_, subsolverMaxIterCount));
    PetscCall(TaoSetMaximumFunctionEvaluations(almmSubsolver_, subsolverMaxFcn));
    PetscCall(TaoSetTolerances(almmSolver_, almmGatol, 0, 0));
    PetscCall(TaoSetConstraintTolerances(almmSolver_, almmCatol, 0));

    PetscFunctionReturn(PETSC_SUCCESS);
}

PetscErrorCode AnalyticalSolver::initializeTAOContainers()
{
    PetscFunctionBegin;

    PetscCall(VecCreateSeq(PETSC_COMM_SELF, varCnt_, &x_));
    PetscCall(VecCreateSeq(PETSC_COMM_SELF, varCnt_, &xLowerBound_));
    PetscCall(VecCreateSeq(PETSC_COMM_SELF, varCnt_, &xUpperBound_));
    PetscCall(VecCreateSeq(PETSC_COMM_SELF, varCnt_, &costGradient_));
    PetscCall(VecCreateSeq(PETSC_COMM_SELF, cEqCnt_, &cEq_));
    // TODO: why dense? This matrix for the most part should be sparse d.t. BFGS rank two update.
    PetscCall(MatCreateSeqDense(PETSC_COMM_SELF, cEqCnt_, varCnt_, nullptr, &JEq_));

    // Set initial solution to zero. The exact value doesn't matter, because at the first iteration constraints will
    // be disabled, and therefore solver will find the solution where most of the corridors are exactly zero.
    PetscCall(VecSet(x_, 0));

    // Tell TAO that there's no variable bounds
    PetscCall(VecSet(xLowerBound_, PETSC_NINFINITY));
    PetscCall(VecSet(xUpperBound_, PETSC_INFINITY));

    PetscFunctionReturn(PETSC_SUCCESS);
}

PetscErrorCode AnalyticalSolver::setContainersAndRoutines()
{
    PetscFunctionBegin;

    PetscCall(TaoSetSolution(almmSolver_, x_));
    PetscCall(TaoSetVariableBounds(almmSolver_, xLowerBound_, xUpperBound_));

    PetscCall(TaoSetObjectiveAndGradient(almmSolver_, costGradient_, evaluateCostFunctionGradient, this));
    PetscCall(TaoSetEqualityConstraintsRoutine(almmSolver_, cEq_, evaluateEqualityConstraintFunction, this));
    PetscCall(TaoSetJacobianEqualityRoutine(almmSolver_, JEq_, JEq_, evaluateEqualityConstraintJacobian, this));
    PetscCall(TaoSetConvergenceTest(almmSolver_, almmConvergenceTest, this));

    PetscCall(TaoMonitorSet(almmSolver_, monitorALMM, this, nullptr));
    if (std::getenv("_DEV_MONITOR_SUBSOLVER") != nullptr) {
        PetscCall(TaoMonitorSet(almmSubsolver_, monitorSubsolver, this, nullptr));
    }

    PetscFunctionReturn(PETSC_SUCCESS);
}

PetscErrorCode AnalyticalSolver::setScaryOptionsInTAOSolvers()
{
    PetscFunctionBegin;

    // Set up ALMM solver.
    // (!) From now on it's forbidden to call `TaoSet...` for both TAO solvers
    PetscCall(TaoSetUp(almmSolver_));

    // Set initial multipliers to 0 according to original Powell's paper
    Vec almmMultipliers;
    PetscCall(TaoALMMGetMultipliers(almmSolver_, &almmMultipliers));
    PetscCall(VecSet(almmMultipliers, 0.0));
    PetscCall(TaoSetRecycleHistory(almmSolver_, PETSC_TRUE));  // Otherwise they will be overwritten in TaoSolve

    // I would like to configure m0 and mu_fac here, but they will be overwritten at the start of the TAOSolve.
    // Therefore we're forced to set them inside the convergence test.
    PetscFunctionReturn(PETSC_SUCCESS);
}

void AnalyticalSolver::destroyTAOObjects()
{
    // Can't do much if some of these methods throw an error.
    // Plus it's almost impossible to begin with
    if (almmSolver_) static_cast<void>(TaoDestroy(&almmSolver_));
    if (x_) static_cast<void>(VecDestroy(&x_));
    if (xLowerBound_) static_cast<void>(VecDestroy(&xLowerBound_));
    if (xUpperBound_) static_cast<void>(VecDestroy(&xUpperBound_));
    if (costGradient_) static_cast<void>(VecDestroy(&costGradient_));
    if (cEq_) static_cast<void>(VecDestroy(&cEq_));
    if (JEq_) static_cast<void>(MatDestroy(&JEq_));
}

PetscErrorCode AnalyticalSolver::runCallbacks(int iterNum)
{
    PetscFunctionBegin;

    double* xArr;
    PetscCall(VecGetArray(x_, &xArr));
    for (const Callbacks::ModifierCallback& callback : modifierCallbacks_) {
        callback(xArr);
    }
    for (const Callbacks::ReaderCallback& callback : readerCallbacks_) {
        callback(xArr, iterNum);
    }
    PetscCall(VecRestoreArray(x_, &xArr));

    PetscFunctionReturn(PETSC_SUCCESS);
}

AnalyticalSolver::Matrix& AnalyticalSolver::provideZeroedJEqCache()
{
    assert(
        JEqCache_.size() == cEqCnt_ && (cEqCnt_ == 0 || JEqCache_[0].size() == varCnt_) &&
        "Wrong JEqCache_ dimensions");
    for (size_t i = 0; i < cEqCnt_; ++i) {
        JEqCache_[i].assign(varCnt_, 0);
    }
    return JEqCache_;
}

std::vector<double> AnalyticalSolver::provideJEqCache() const
{
    assert(
        JEqCache_.size() == cEqCnt_ && (cEqCnt_ == 0 || JEqCache_[0].size() == varCnt_) &&
        "Wrong JEqCache_ dimensions");
    std::vector<double> result(cEqCnt_ * varCnt_);
    for (size_t i = 0; i < cEqCnt_; ++i) {
        for (size_t j = 0; j < varCnt_; ++j) {
            result[i * varCnt_ + j] = JEqCache_[i][j];
        }
    }
    return result;
}

}  // namespace AnalyticalSolver
}  // namespace DungeonGenerator

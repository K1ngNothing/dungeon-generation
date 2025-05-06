#include "AnalyticalSolver.h"

#include <cassert>
#include <chrono>
#include <iostream>
#include <numeric>

#include <utils/CLArguments.h>

namespace DungeonGeneration {
namespace AnalyticalSolver {

AnalyticalSolver::AnalyticalSolver(
    size_t objectCnt, size_t varCnt, Model::VariablesBounds&& variablesBounds,
    std::vector<Callbacks::FGEval>&& costFunctions, std::vector<Callbacks::CEqFGEval>&& equalityConstraints,
    std::vector<Callbacks::ModifierCallback>&& modifierCallbacks,
    std::vector<Callbacks::ReaderCallback>&& readerCallbacks)
      : objectCnt_(objectCnt),
        varCnt_(varCnt),
        cEqCnt_(equalityConstraints.size()),
        variablesBounds_(std::move(variablesBounds)),
        costFunctions_(std::move(costFunctions)),
        equalityConstraints_(std::move(equalityConstraints)),
        modifierCallbacks_(std::move(modifierCallbacks)),
        readerCallbacks_(std::move(readerCallbacks)),
        JEqRowIndexes_(cEqCnt_),
        JEqColIndexes_(varCnt)
{
    if (initializePETSc() != PETSC_SUCCESS) {
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

    // Setup containers used for updating JEq
    std::iota(JEqRowIndexes_.begin(), JEqRowIndexes_.end(), 0);
    std::iota(JEqColIndexes_.begin(), JEqColIndexes_.end(), 0);
    assert(JEqRowIndexes_.size() == cEqCnt_ && "Incorrect row indexes count");
    assert(JEqColIndexes_.size() == varCnt_ && "Incorrect col indexes count");
}

AnalyticalSolver::~AnalyticalSolver()
{
    destroyTAOObjects();
    static_cast<void>(PetscFinalize());
}

void AnalyticalSolver::solve()
{
    // TODO: do I really want to throw exceptions? Maybe just return bool or some kind of error code?

    std::cerr << "AnalyticalSolver: start solving...\n";
    const auto beginTimestamp = std::chrono::steady_clock::now();

    if (TaoSolve(almmSolver_) != PETSC_SUCCESS) {
        throw std::runtime_error("AnalyticalSolver: error in TaoSolve for ALMM solver");
    }
    TaoConvergedReason convergedReason;
    if (TaoGetConvergedReason(almmSolver_, &convergedReason) != PETSC_SUCCESS) {
        // TODO: do better
        throw std::runtime_error("AnalyticalSolver: failed to retrieve converged reason");
    }

    const auto endTimestamp = std::chrono::steady_clock::now();
    const double solvingDuration = std::chrono::duration<double>(endTimestamp - beginTimestamp).count();
    std::cerr << "AnalyticalSolver: finished solving in " << solvingDuration << " seconds!\n"
              << "Converged reason: " << TaoConvergedReasons[convergedReason] << "\n";
}

bool AnalyticalSolver::rerunSolver()
{
    if (prepareSolverRerun() != PETSC_SUCCESS) {
        std::cerr << "(!) AnalyticalSolver::rerunSolver(): rerun preparation failed :(\n";
        return false;
    }

    // Try to run solver
    try {
        runId_++;
        std::cerr << "\n----- Rerun " << runId_ << " -----\n";
        solve();
        return true;
    } catch (std::exception& error) {
        std::cout << "(!) AnalyticalSolver::rerunSolver: solver rerun failed: " << error.what() << "\n";
        return false;
    } catch (...) {
        assert(false && "One must throw a proper exception");
        return false;
    }
}

Model::Positions AnalyticalSolver::retrieveSolution() const
{
    const double* xArr = nullptr;
    if (VecGetArrayRead(x_, &xArr) != PETSC_SUCCESS) {
        return {};
    }
    Model::Positions solution(objectCnt_);
    for (size_t objId = 0; objId < objectCnt_; ++objId) {
        const auto [xId, yId] = Model::VarUtils::getVariablesIds(objId);
        solution[objId].x = xArr[xId];
        solution[objId].y = xArr[yId];
    }
    static_cast<void>(VecRestoreArrayRead(x_, &xArr));
    return solution;
}

PetscErrorCode AnalyticalSolver::initializePETSc()
{
    PetscFunctionBegin;

    std::optional<CLUtils::CLArguments> clArgs = CLUtils::provideCLArgumentsHandler().provideArguments();
    if (clArgs.has_value()) {
        auto [argc, argv] = clArgs.value();
        PetscCall(PetscInitialize(&argc, &argv, nullptr, nullptr));
    } else {
        PetscCall(PetscInitializeNoArguments());
    }

    PetscFunctionReturn(PETSC_SUCCESS);
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
    // TODO: should this matrix be dense?
    PetscCall(MatCreateSeqDense(PETSC_COMM_SELF, cEqCnt_, varCnt_, nullptr, &JEq_));
    // PetscCall(MatCreateSeqAIJ(PETSC_COMM_SELF, cEqCnt_, varCnt_, varCnt_, nullptr, &JEq_));

    // Set initial solution to zero. The exact value doesn't matter, because at the first iteration constraints will
    // be disabled, and therefore solver will find the solution where most of the corridors are exactly zero.
    PetscCall(VecSet(x_, 0));

    // Set variables bounds
    double* xLowerBoundArr;
    double* xUpperBoundArr;
    PetscCall(VecGetArray(xLowerBound_, &xLowerBoundArr));
    PetscCall(VecGetArray(xUpperBound_, &xUpperBoundArr));

    auto setBounds = [xLowerBoundArr, xUpperBoundArr, this](size_t varId) {
        if (variablesBounds_[varId].has_value()) {
            xLowerBoundArr[varId] = variablesBounds_[varId]->lowerBound;
            xUpperBoundArr[varId] = variablesBounds_[varId]->upperBound;
        } else {
            xLowerBoundArr[varId] = PETSC_NINFINITY;
            xUpperBoundArr[varId] = PETSC_INFINITY;
        }
    };
    for (size_t objId = 0; objId < objectCnt_; ++objId) {
        const auto [xId, yId] = Model::VarUtils::getVariablesIds(objId);
        setBounds(xId);
        setBounds(yId);
    }
    PetscCall(VecRestoreArray(xLowerBound_, &xLowerBoundArr));
    PetscCall(VecRestoreArray(xUpperBound_, &xUpperBoundArr));

    PetscFunctionReturn(PETSC_SUCCESS);
}

PetscErrorCode AnalyticalSolver::setContainersAndRoutines()
{
    PetscFunctionBegin;

    PetscCall(TaoSetSolution(almmSolver_, x_));
    PetscCall(TaoSetVariableBounds(almmSolver_, xLowerBound_, xUpperBound_));

    PetscCall(TaoSetObjectiveAndGradient(almmSolver_, costGradient_, evaluateCostFunctionGradient, this));
    PetscCall(TaoSetEqualityConstraintsRoutine(almmSolver_, cEq_, evaluateEqualityConstraintsFunction, this));
    PetscCall(TaoSetJacobianEqualityRoutine(almmSolver_, JEq_, JEq_, evaluateEqualityConstraintsJacobian, this));
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

PetscErrorCode AnalyticalSolver::prepareSolverRerun()
{
    PetscFunctionBegin;

    // Nerf Lagrangian multipliers
    int multipliersCnt;
    Vec multipliersVec;
    double* multipliersArr;
    PetscCall(TaoALMMGetMultipliers(almmSolver_, &multipliersVec));
    PetscCall(VecGetSize(multipliersVec, &multipliersCnt));
    PetscCall(VecGetArray(multipliersVec, &multipliersArr));
    for (int i = 0; i < multipliersCnt; ++i) {
        multipliersArr[i] /= 1000;
    }
    PetscCall(VecRestoreArray(multipliersVec, &multipliersArr));

    // Make sure that ALMM will reuse existing solution and multipliers
    PetscCall(TaoSetRecycleHistory(almmSolver_, PETSC_TRUE));

    PetscFunctionReturn(PETSC_SUCCESS);
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
        callback(xArr, runId_, iterNum);
    }
    PetscCall(VecRestoreArray(x_, &xArr));

    PetscFunctionReturn(PETSC_SUCCESS);
}

}  // namespace AnalyticalSolver
}  // namespace DungeonGeneration

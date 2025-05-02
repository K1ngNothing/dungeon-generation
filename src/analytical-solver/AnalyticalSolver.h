#pragma once

#include <vector>

#include <callbacks/Defs.h>
#include <model/Model.h>
#include <model/Room.h>
#include <petsctao.h>

#include "TAOCallbacks.h"

namespace DungeonGeneration {
namespace AnalyticalSolver {

class AnalyticalSolver {
public:
    AnalyticalSolver(
        size_t objectCnt, size_t varCnt, Model::VariablesBounds&& variablesBounds,
        std::vector<Callbacks::FGEval>&& costFunctions, std::vector<Callbacks::FGEval>&& equalityConstraints,
        std::vector<Callbacks::ModifierCallback>&& modifierCallbacks,
        std::vector<Callbacks::ReaderCallback>&& readerCallbacks);

    ~AnalyticalSolver();

    void solve();

    Model::Positions retrieveSolution() const;

private:
    PetscErrorCode initializeTAOSolvers();
    PetscErrorCode initializeTAOContainers();
    PetscErrorCode setContainersAndRoutines();
    PetscErrorCode setScaryOptionsInTAOSolvers();
    void destroyTAOObjects();

    /// Run callbacks (e.g. SVG dump) after each ALMM iteration.
    PetscErrorCode runCallbacks(int iterNum);

    friend PetscErrorCode evaluateCostFunctionGradient(Tao, Vec, double*, Vec, void*);
    friend PetscErrorCode evaluateEqualityConstraintsFunction(Tao, Vec, Vec, void*);
    friend PetscErrorCode evaluateEqualityConstraintsJacobian(Tao, Vec, Mat, Mat, void*);
    friend PetscErrorCode monitorALMM(Tao, void*);
    friend PetscErrorCode monitorSubsolver(Tao, void*);
    friend PetscErrorCode almmConvergenceTest(Tao, void*);

    // Task info
    size_t objectCnt_;
    size_t varCnt_;
    size_t cEqCnt_;
    Model::VariablesBounds variablesBounds_;
    std::vector<Callbacks::FGEval> costFunctions_;
    std::vector<Callbacks::FGEval> equalityConstraints_;
    std::vector<Callbacks::ModifierCallback> modifierCallbacks_;
    std::vector<Callbacks::ReaderCallback> readerCallbacks_;

    // TAO solvers
    Tao almmSolver_ = nullptr;
    Tao almmSubsolver_ = nullptr;

    // TAO containers
    Vec x_ = nullptr;
    Vec xLowerBound_ = nullptr;
    Vec xUpperBound_ = nullptr;
    Vec costGradient_ = nullptr;
    Vec cEq_ = nullptr;
    Mat JEq_ = nullptr;

    // Helper containers used to update JEq
    std::vector<PetscInt> JEqRowIndexes_;
    std::vector<PetscInt> JEqColIndexes_;
};

}  // namespace AnalyticalSolver
}  // namespace DungeonGeneration

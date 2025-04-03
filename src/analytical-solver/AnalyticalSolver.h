#pragma once

#include <vector>

#include <callbacks/Defs.h>
#include <model/Model.h>
#include <model/Room.h>
#include <petsctao.h>

#include "TAOCallbacks.h"

namespace DungeonGenerator {
namespace AnalyticalSolver {

class AnalyticalSolver {
    // Temporary, check comment for JEqCache
    using Matrix = std::vector<std::vector<double>>;

public:
    AnalyticalSolver(
        size_t roomCnt, std::vector<Callbacks::FGEval>&& costFunctions,
        std::vector<Callbacks::FGEval>&& equalityConstraints,
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

    // Temporary, check comment for JEqCache
    Matrix& provideZeroedJEqCache();
    std::vector<double> provideJEqCache() const;

    friend PetscErrorCode evaluateCostFunctionGradient(Tao, Vec, double*, Vec, void*);
    friend PetscErrorCode evaluateEqualityConstraintFunction(Tao, Vec, Vec, void*);
    friend PetscErrorCode evaluateEqualityConstraintJacobian(Tao, Vec, Mat, Mat, void*);
    friend PetscErrorCode monitorALMM(Tao, void*);
    friend PetscErrorCode monitorSubsolver(Tao, void*);
    friend PetscErrorCode almmConvergenceTest(Tao, void*);

    // Task info
    size_t roomCnt_;
    size_t varCnt_;
    size_t cEqCnt_;
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

    // Cache for JEq. Rational here is that in TAO there's no routine that evaluates both cEq and
    // JEq at the same time. To circumvent this we evaluate them both in cEq evaluation function,
    // and in JEq evaluation function we only copy already calculated cache.

    // TODO: it seems that we can avoid JEqCache_ entirely by storing calculated value in JEq_ directly.
    // We can use this cache in asserts to check that that matrix doesn't change between cEq and
    // JEq evaluations. If these asserts don't fail, Jacobian cache should be removed entirely.
    Matrix JEqCache_;
};

}  // namespace AnalyticalSolver
}  // namespace DungeonGenerator

#pragma once

#include <petsctao.h>

namespace DungeonGeneration {
namespace AnalyticalSolver {

PetscErrorCode evaluateCostFunctionGradient(Tao almmSolver, Vec xVec, double* f, Vec gVec, void* ctx);
PetscErrorCode evaluateEqualityConstraintsFunction(Tao almmSolver, Vec xVec, Vec cEq, void* ctx);
PetscErrorCode evaluateEqualityConstraintsJacobian(Tao almmSolver, Vec xVec, Mat JEq, Mat JEqPre, void* ctx);
PetscErrorCode almmConvergenceTest(Tao almmSolver, void* ctx);

PetscErrorCode monitorALMM(Tao almmSolver, void* ctx);
PetscErrorCode monitorSubsolver(Tao subsolver, void* ctx);

}  // namespace AnalyticalSolver
}  // namespace DungeonGeneration

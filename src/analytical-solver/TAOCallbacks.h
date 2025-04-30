#pragma once

#include <petsctao.h>

namespace DungeonGeneration {
namespace AnalyticalSolver {

PetscErrorCode evaluateCostFunctionGradient(Tao almmSolver, Vec xVec, double* f, Vec gVec, void* ctx);
PetscErrorCode evaluateEqualityConstraintFunction(Tao almmSolver, Vec xVec, Vec cEq, void* ctx);
PetscErrorCode evaluateEqualityConstraintJacobian(Tao almmSolver, Vec xVec, Mat JEq, Mat JEqPre, void* ctx);
PetscErrorCode almmConvergenceTest(Tao almmSolver, void* ctx);

PetscErrorCode monitorALMM(Tao almmSolver, void* ctx);
PetscErrorCode monitorSubsolver(Tao subsolver, void* ctx);

}  // namespace AnalyticalSolver
}  // namespace DungeonGeneration

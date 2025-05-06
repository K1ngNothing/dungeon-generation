#pragma once

#include <functional>

namespace DungeonGeneration {
namespace Callbacks {

using FGEval = std::function<void(const double*, double&, double*)>;
using CEqFGEval = std::function<void(const double*, double&, void*, int)>;  // void* to avoid defining PETSc structures
using ModifierCallback = std::function<void(double*)>;
using ReaderCallback = std::function<void(const double*, int, int)>;

}  // namespace Callbacks
}  // namespace DungeonGeneration

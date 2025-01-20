#pragma once

#include <functional>

namespace DungeonGenerator {
namespace Functions {

using FGEval = std::function<void(const double*, double&, double*)>;

}  // namespace Functions
}  // namespace DungeonGenerator

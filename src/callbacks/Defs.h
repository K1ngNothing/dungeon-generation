#pragma once

#include <functional>

namespace DungeonGeneration {
namespace Callbacks {

using FGEval = std::function<void(const double*, double&, double*)>;
using ModifierCallback = std::function<void(double*)>;
using ReaderCallback = std::function<void(const double*, int)>;

}  // namespace Callbacks
}  // namespace DungeonGeneration

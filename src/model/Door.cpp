#include "Door.h"

namespace DungeonGenerator {
namespace Model {

Variables Door::getVariables(const double* x) const
{
    Variables result = Utils::getVariables(roomId, x);
    result.varX += dx;
    result.varY += dy;
    return result;
}

VariablesIds Door::getVariablesIds() const
{
    return VariablesIds{.xId = roomId * 2, .yId = roomId * 2 + 1};
}

}  // namespace Model
}  // namespace DungeonGenerator

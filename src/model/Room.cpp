#include "Room.h"

namespace DungeonGenerator {
namespace Model {

Variables Room::getVariables(const double* x) const
{
    return Utils::getVariables(id, x);
}

VariablesIds Room::getVariablesIds() const
{
    return VariablesIds{.xId = id * 2, .yId = id * 2 + 1};
}

}  // namespace Model
}  // namespace DungeonGenerator

#include "CorridorLength.h"

#include <model/Variables.h>

namespace DungeonGenerator {
namespace Callbacks {

CorridorLength::CorridorLength(const Model::Door& door1, const Model::Door& door2)
      : door1_(door1),
        door2_(door2)
{}

void CorridorLength::operator()(const double* x, double& f, double* grad) const
{
    using namespace Model::VarUtils;

    /*
    Euclidean square distance with account to movable doors
    dx = x_r1 + x_d1 - x_r2 - x_d2
    dy = y_r1 + y_d1 - y_r2 - y_d2
    f = dx^2 + dy^2
    gradX1 = 2 * dx -- for both room and door
    gradY1 = 2 * dy
    */

    const auto [x1, y1] = door1_.getPositionFromVars(x);  // x_r1 + x_d1, y_r1 + y_d1
    const auto [x2, y2] = door2_.getPositionFromVars(x);  // x_r2 + x_d2, y_r2 + y_d2
    const auto [xRoom1Id, yRoom1Id] = getVariablesIds(door1_.parentRoomId);
    const auto [xRoom2Id, yRoom2Id] = getVariablesIds(door2_.parentRoomId);
    const auto [xDoor1Id, yDoor1Id] = door1_.getVariablesIds();
    const auto [xDoor2Id, yDoor2Id] = door2_.getVariablesIds();

    const double dx = x1 - x2;
    const double dy = y1 - y2;
    f += dx * dx + dy * dy;

    if (grad != nullptr) {
        const double gradX1 = 2 * dx;
        const double gradY1 = 2 * dy;
        grad[xRoom1Id] += gradX1;
        grad[xDoor1Id] += gradX1;

        grad[yRoom1Id] += gradY1;
        grad[yDoor1Id] += gradY1;

        grad[xRoom2Id] -= gradX1;
        grad[xDoor2Id] -= gradX1;

        grad[yRoom2Id] -= gradY1;
        grad[yDoor2Id] -= gradY1;
    }
}

}  // namespace Callbacks
}  // namespace DungeonGenerator

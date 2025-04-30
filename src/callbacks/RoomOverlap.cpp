#include "RoomOverlap.h"

#include <cassert>
#include <cstdlib>

namespace DungeonGeneration {
namespace Callbacks {

RoomOverlap::RoomOverlap(const Model::Room& room1, const Model::Room& room2, double roomBloating)
      : roomBloating_(roomBloating),
        room1_(room1),
        room2_(room2)
{
    assert(room1.id != room2.id && "Don't create overlap function for one room");
    for (const Model::Room& room : {room1, room2}) {
        assert(room.width > 0.0 && "Bad room width");
        assert(room.height > 0.0 && "Bad room height");
    }
}

void RoomOverlap::operator()(const double* x, double& f, double* grad) const
{
    const auto [x1, y1] = room1_.getVariablesVal(x);
    const auto [x2, y2] = room2_.getVariablesVal(x);
    const auto [x1Id, y1Id] = room1_.getVariablesIds();
    const auto [x2Id, y2Id] = room2_.getVariablesIds();

    const double dx = x1 - x2;
    const double dy = y1 - y2;
    double sumHalfWidth = (room1_.width + room2_.width) / 2;
    double sumHalfHeight = (room1_.height + room2_.height) / 2;
    sumHalfHeight *= roomBloating_;
    sumHalfWidth *= roomBloating_;
    if (std::abs(dx) >= sumHalfWidth || std::abs(dy) >= sumHalfHeight) {
        // Rooms do not intersect
        return;
    }

    /*
    fx = (dx / sumHalfWidth)^2 - 1
    fy = (dy / sumHalfHeight)^2 - 1
    f = fx^2 * fy^2
    gradX1 = fy^2 * (2 * fx) * (2 * dx / sumHalfWidth^2)
    gradY1 = fx^2 * (2 * fy) * (2 * dy / sumHalfHeight^2)
    gradX2 = -gradX1
    gradY2 = -gradY1
    */
    const double xRatio = dx / sumHalfWidth;
    const double fx = xRatio * xRatio - 1;
    const double fxSquared = fx * fx;

    const double yRatio = dy / sumHalfHeight;
    const double fy = yRatio * yRatio - 1;
    const double fySquared = fy * fy;

    const double fVal = fxSquared * fySquared;
    f += fVal;
    assert(fVal <= 1 && "Room overlap should be in range [0, 1]");

    if (grad != nullptr) {
        const double gradX1 = 4 * fySquared * fx * dx / (sumHalfWidth * sumHalfWidth);
        const double gradY1 = 4 * fxSquared * fy * dy / (sumHalfHeight * sumHalfHeight);
        grad[x1Id] += gradX1;
        grad[y1Id] += gradY1;
        grad[x2Id] -= gradX1;
        grad[y2Id] -= gradY1;
    }
}

}  // namespace Callbacks
}  // namespace DungeonGeneration

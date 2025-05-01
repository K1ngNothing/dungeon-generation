#include "RoomShaker.h"

#include <cassert>

namespace DungeonGeneration {
namespace Callbacks {

RoomShaker::RoomShaker(const Model::Model& model)
      : model_(model)
{}

void RoomShaker::operator()(double* x)
{
    assert(x != nullptr);
    const Model::Rooms& rooms = model_.rooms();
    bool roomsMoved = false;
    do {
        roomsMoved = false;
        for (size_t i = 0; i < rooms.size(); ++i) {
            for (size_t j = 0; j < rooms.size(); ++j) {
                if (i == j) continue;
                const Model::Room& room1 = rooms[i];
                const Model::Room& room2 = rooms[j];
                const auto [x1, y1] = room1.getVariablesVal(x);
                const auto [x2, y2] = room2.getVariablesVal(x);
                const double dx = abs(x1 - x2);
                const double dy = abs(y1 - y2);
                const double sumHalfW = (room1.width() + room2.width()) / 2;
                const double sumHalfH = (room1.height() + room2.height()) / 2;
                if (dx < sumHalfW * 0.001 && dy < sumHalfH * 0.001) {
                    // Rooms are stacked right on top of each other => gradient is almost zero
                    const double xBound = room1.width() * 0.01;
                    const double yBound = room1.height() * 0.01;
                    const double shiftX = Random::uniformRangeContinuous(-xBound, xBound, rng_);
                    const double shiftY = Random::uniformRangeContinuous(-yBound, yBound, rng_);
                    const auto [xId1, yId1] = room1.getVariablesIds();
                    x[xId1] += shiftX;
                    x[yId1] += shiftY;
                    roomsMoved = true;
                    // DEBUG
                    // std::cout << "Stacked rooms: " << i << ' ' << j << ", coords: " << x[xId1] << ' ' << x[yId1] << '
                    // '
                    //           << ", shifts: " << shiftX << ' ' << shiftY << "\n";
                }
            }
        }
    } while (roomsMoved);
}

}  // namespace Callbacks
}  // namespace DungeonGeneration

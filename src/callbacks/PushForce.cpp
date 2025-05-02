#include "PushForce.h"

#include <cassert>
#include <cstdlib>

namespace DungeonGeneration {
namespace Callbacks {

PushForce::PushForce(const Model::Model& model, double scale, double range)
      : model_(model),
        scale_(scale),
        range_(range),
        disconnectedRooms_(findDisconnectedRooms(model))
{}

void PushForce::operator()(const double* x, double& f, double* grad) const
{
    if constexpr (kPushOnlyDisconnected) {
        for (RoomPair rooms : disconnectedRooms_) {
            calculatePush(rooms, x, f, grad);
        }
    } else {
        size_t roomCount = model_.rooms().size();
        for (size_t i = 0; i < roomCount; ++i) {
            for (size_t j = i + 1; j < roomCount; ++j) {
                calculatePush(RoomPair{i, j}, x, f, grad);
            }
        }
    }
}

PushForce::RoomPairs PushForce::findDisconnectedRooms(const Model::Model& model) const
{
    RoomPairs disconnectedRooms;

    size_t n = model_.rooms().size();
    std::vector<std::vector<bool>> areConnected(n, std::vector<bool>(n, false));
    for (const Model::Corridor& corridor : model_.corridors()) {
        size_t room1 = corridor.door1.parentRoomId();
        size_t room2 = corridor.door2.parentRoomId();
        areConnected[room1][room2] = true;
        areConnected[room2][room1] = true;
    }
    for (size_t room1 = 0; room1 < n - 1; ++room1) {
        for (size_t room2 = room1 + 1; room2 < n; ++room2) {
            if (areConnected[room1][room2]) {
                continue;
            }
            disconnectedRooms.push_back({room1, room2});
        }
    }
    return disconnectedRooms;
}

void PushForce::calculatePush(RoomPair rooms, const double* x, double& f, double* grad) const
{
    const Model::Rooms allRooms = model_.rooms();
    assert(rooms.roomId1 < allRooms.size() && "Invalid room1Id");
    assert(rooms.roomId2 < allRooms.size() && "Invalid room2Id");
    const Model::Room& room1 = allRooms[rooms.roomId1];
    const Model::Room& room2 = allRooms[rooms.roomId2];

    const auto [x1, y1] = room1.getVariablesVal(x);
    const auto [x2, y2] = room2.getVariablesVal(x);
    const auto [x1Id, y1Id] = room1.getVariablesIds();
    const auto [x2Id, y2Id] = room2.getVariablesIds();

    const double dx = x1 - x2;
    const double dy = y1 - y2;
    const double sumHW = (room1.width() + room2.width()) / 2;
    const double sumHH = (room1.height() + room2.height()) / 2;

    /*
    xRatio = dx / (range * sumHW)
    yRatio = dy / (range * sumHH)
    f = scale / (xRatio^2 + yRatio^2 + 1)
    gradX1 = -scale * (2 * dx / (range * sumHW)^2) / (xRatio^2 + yRatio^2 + 1)^2
    gradY1 = -scale * (2 * dy / (range * sumHH)^2) / (xRatio^2 + yRatio^2 + 1)^2
    gradX2 = -gradX1
    gradY2 = -gradY1
    */
    const double xRatio = dx / (range_ * sumHW);
    const double yRatio = dy / (range_ * sumHH);
    const double denominator = xRatio * xRatio + yRatio * yRatio + 1;
    const double fVal = scale_ / denominator;

    f += fVal;
    assert(0 <= fVal && fVal <= scale_ && "Room overlap should be in range [0, scale]");

    if (grad != nullptr) {
        const double gradX1 = (-2.0 * xRatio / (range_ * sumHW)) * fVal / denominator;
        const double gradY1 = (-2.0 * yRatio / (range_ * sumHH)) * fVal / denominator;
        grad[x1Id] += gradX1;
        grad[y1Id] += gradY1;
        grad[x2Id] -= gradX1;
        grad[y2Id] -= gradY1;
    }
}

}  // namespace Callbacks
}  // namespace DungeonGeneration

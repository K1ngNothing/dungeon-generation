#pragma once

#include <model/Room.h>

namespace DungeonGenerator {
namespace Callbacks {

class RoomOverlap {
    static constexpr double kDefaultRoomBloating = 1.25;

public:
    RoomOverlap(const Model::Room& room1, const Model::Room& room2, const double roomBloating = kDefaultRoomBloating);
    void operator()(const double* x, double& f, double* grad) const;

private:
    const double roomBloating_ = kDefaultRoomBloating;
    const Model::Room& room1_;
    const Model::Room& room2_;
};

}  // namespace Callbacks
}  // namespace DungeonGenerator

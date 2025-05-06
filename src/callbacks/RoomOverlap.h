#pragma once

#include <model/Room.h>

namespace DungeonGeneration {
namespace Callbacks {

class RoomOverlap {
    static constexpr double kNoBloating = 1.0;

public:
    RoomOverlap(const Model::Room& room1, const Model::Room& room2, const double roomBloating = kNoBloating);
    void operator()(const double* x, double& f, double* grad) const;

private:
    const double roomBloating_ = kNoBloating;
    const Model::Room& room1_;
    const Model::Room& room2_;
};

}  // namespace Callbacks
}  // namespace DungeonGeneration

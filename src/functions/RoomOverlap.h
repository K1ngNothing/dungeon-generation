#pragma once

#include <model/Room.h>

namespace DungeonGenerator {
namespace Functions {

class RoomOverlap {
public:
    RoomOverlap(const Model::Room& room1, const Model::Room& room2);
    void operator()(const double* x, double& f, double* grad) const;

private:
    const Model::Room& room1_;
    const Model::Room& room2_;
};

}  // namespace Functions
}  // namespace DungeonGenerator

#pragma once

#include <model/Door.h>

namespace DungeonGenerator {
namespace Callbacks {

class CorridorLength {
public:
    CorridorLength(const Model::Door& door1, const Model::Door& door2);
    void operator()(const double* x, double& f, double* grad) const;

private:
    const Model::Door& door1_;
    const Model::Door& door2_;
};

}  // namespace Callbacks
}  // namespace DungeonGenerator

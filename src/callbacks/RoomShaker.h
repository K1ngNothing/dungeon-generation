#pragma once

#include <model/Model.h>
#include <utils/Random.h>

namespace DungeonGeneration {
namespace Callbacks {

class RoomShaker {
public:
    RoomShaker(const Model::Model& model);
    void operator()(double* x);

private:
    const Model::Model& model_;
    Random::RNG rng_ = Random::RNG(Random::kGlobalSeed);  // random number generator
};

}  // namespace Callbacks
}  // namespace DungeonGeneration

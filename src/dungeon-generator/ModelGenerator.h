#pragma once

#include <model/Model.h>
#include <utils/Random.h>

#include "Defs.h"

namespace DungeonGeneration {

// A class for generating model, i.e. rooms and connections between them.
class ModelGenerator {
public:
    ModelGenerator() = default;

    // Generation functions with predefined structure.
    Model::Model generateGrid(size_t gridSide) const;
    Model::Model generateModelCenterDoors(size_t roomCount);
    Model::Model generateTreeFixedDoors(size_t roomCount);
    Model::Model generateModelMovableDoors(size_t roomCount);

private:
    RoomDimensions generateRoom(size_t roomId);
    RoomDimensions generateRoomFromDistribution(const std::vector<RoomType>& roomTypes);

    Random::RNG rng_ = Random::RNG(Random::kGlobalSeed);  // random number generator
};

}  // namespace DungeonGeneration

#pragma once

#include <random>

#include <model/Model.h>

namespace DungeonGenerator {

// A rough draft of model generator class, which is responsible for generating rooms and connections between them.
// TODO: add more configuration options
class ModelGenerator {
public:
    // Main generating function. Not yet implemented.
    Model::Model generateDungeon(size_t roomCount) const;

    // Generation functions with predefined structure. Used in tests.
    Model::Model generateGrid(size_t gridSide) const;
    Model::Model generateTreeCenterRooms(size_t roomCount);
    Model::Model generateTreeDungeon(size_t roomCount);

private:
    static constexpr size_t kSeed = 42;

    std::mt19937 rng_ = std::mt19937(kSeed);  // random number generator
};

}  // namespace DungeonGenerator

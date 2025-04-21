#pragma once

#include <random>

#include <model/Model.h>

namespace DungeonGenerator {

// A rough draft of model generator class, which is responsible for generating rooms and connections between them.
// TODO: add more configuration options
class ModelGenerator {
    enum class TreeGenerationStrategy {
        RandomPredecessors,  // For each vertex connet it with random predecessor
        RandomChildCount,    // For each vertex randomly choose child count
    };

public:
    using Graph = std::vector<std::vector<size_t>>;

    // Main generating function. Not yet implemented.
    Model::Model generateDungeon(size_t roomCount) const;

    // Generation functions with predefined structure. Used in tests.
    Model::Model generateGrid(size_t gridSide) const;
    Model::Model generateTreeCenterRooms(size_t roomCount);
    Model::Model generateTreeDungeon(size_t roomCount);

private:
    static constexpr TreeGenerationStrategy kTreeGenerationStrategy = TreeGenerationStrategy::RandomChildCount;
    static constexpr size_t kSeed = 42;

    Graph generateTree(size_t vertexCount);
    Graph generateTreePredecessorStrategy(size_t vertexCount);
    Graph generateTreeChildCountStrategy(size_t vertexCount);

    std::mt19937 rng_ = std::mt19937(kSeed);  // random number generator
};

}  // namespace DungeonGenerator

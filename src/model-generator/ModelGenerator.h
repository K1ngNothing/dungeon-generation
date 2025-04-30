#pragma once

#include <model/Model.h>
#include <random/Random.h>

namespace DungeonGeneration {

// A rough draft of model generator class, which is responsible for generating rooms and connections between them.
// TODO: add more configuration options
class ModelGenerator {
    enum class TreeGenerationStrategy {
        RandomPredecessors,  // For each vertex connet it with random predecessor
        RandomChildCount,    // For each vertex randomly choose child count
    };

public:
    using Graph = std::vector<std::vector<size_t>>;

    // Generation functions with predefined structure.
    Model::Model generateGrid(size_t gridSide) const;
    Model::Model generateTreeMovableDoors(size_t roomCount);
    Model::Model generateTreeFixedDoors(size_t roomCount);

private:
    static constexpr TreeGenerationStrategy kTreeGenerationStrategy = TreeGenerationStrategy::RandomChildCount;

    Graph generateTree(size_t vertexCount);
    Graph generateTreePredecessorStrategy(size_t vertexCount);
    Graph generateTreeChildCountStrategy(size_t vertexCount);

    Random::RNG rng_ = Random::RNG(Random::kGlobalSeed);  // random number generator
};

}  // namespace DungeonGeneration

#pragma once

#include <model/Model.h>
#include <utils/Random.h>

namespace DungeonGeneration {

// A class for generating underlying graphs in model
class GraphGenerator {
public:
    using Graph = std::vector<std::vector<size_t>>;

    GraphGenerator() = default;

    Graph generateTree(size_t vertexCount);
    Graph generateConnectedGraph(size_t vertexCount, size_t additionalEdges);

private:
    Graph generateTreePredecessorStrategy(size_t vertexCount);
    Graph generateTreeChildCountStrategy(size_t vertexCount);

    Random::RNG rng_ = Random::RNG(Random::kGlobalSeed);  // random number generator
};

}  // namespace DungeonGeneration

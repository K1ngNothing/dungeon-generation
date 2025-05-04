#pragma once

#include <model/Model.h>
#include <utils/Random.h>

namespace DungeonGeneration {

// A class for generating model, i.e. rooms and connections between them.
class ModelGenerator {
    enum class TreeGenerationStrategy {
        RandomPredecessors,  // For each vertex connet it with random predecessor
        RandomChildCount,    // For each vertex randomly choose child count
    };

public:
    using Graph = std::vector<std::vector<size_t>>;

    // Generation functions with predefined structure.
    Model::Model generateGrid(size_t gridSide) const;
    Model::Model generateTreeCenterDoors(size_t roomCount);
    Model::Model generateTreeFixedDoors(size_t roomCount);
    Model::Model generateTreeMovableDoors(size_t roomCount);

private:
    struct RoomDimensions {
        double width;
        double height;
    };
    struct RoomType {
        RoomDimensions dimensions;
        double distributionWeight;
    };
    const std::vector<RoomType> kRegularRoomTypes = {
        {{20, 20},    1},
        {{30, 30},  0.5},
        {{20, 40},  0.3},
        {{40, 20},  0.3},
        {{40, 40}, 0.25}
    };
    const std::vector<RoomType> kHubRoomTypes{
        {{60, 60}, 1}
    };
    static constexpr size_t kHubNeighborsCount = 8;

    static constexpr TreeGenerationStrategy kTreeGenerationStrategy = TreeGenerationStrategy::RandomChildCount;
    static constexpr bool kEnableHubRoom = true;
    static constexpr bool kUniformRooms = false;  /// If enabled, only generates the first room type

    Graph generateTree(size_t vertexCount);
    Graph generateTreePredecessorStrategy(size_t vertexCount);
    Graph generateTreeChildCountStrategy(size_t vertexCount);

    RoomDimensions generateRoom(size_t roomId);
    RoomDimensions generateRoomFromDistribution(const std::vector<RoomType>& roomTypes);

    Random::RNG rng_ = Random::RNG(Random::kGlobalSeed);  // random number generator
};

}  // namespace DungeonGeneration

#pragma once

namespace DungeonGeneration {

enum class DungeonType {
    Grid,
    CenterDoors,
    TreeFixedDoors,
    MovableDoors,
};

enum class TreeGenerationStrategy {
    RandomPredecessors,  // For each vertex connet it with random predecessor
    RandomChildCount,    // For each vertex randomly choose child count
};

struct RoomDimensions {
    double width;
    double height;
};

struct RoomType {
    RoomDimensions dimensions;
    double distributionWeight;
};

}  // namespace DungeonGeneration

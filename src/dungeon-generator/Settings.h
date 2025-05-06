#pragma once

#include <cstddef>
#include <vector>

#include "Defs.h"

namespace DungeonGeneration {

// Model generation settings
constexpr DungeonType kDungeonType = DungeonType::MovableDoors;
constexpr size_t kRoomCount = 50;
static constexpr size_t kAdditionalEdges = kRoomCount * 0.1;  /// Edge count to add to the tree
const std::vector<RoomType> kRegularRoomTypes = {
    {{20, 20},    1},
    {{30, 30},  0.5},
    {{20, 40},  0.3},
    {{40, 20},  0.3},
    {{40, 40}, 0.25}
};

// Solver rerun
constexpr size_t kSolverRerunCount = 0;

// Hub
static constexpr bool kEnableHubRoom = true;
const std::vector<RoomType> kHubRoomTypes{
    {{60, 60}, 1}
};
static constexpr size_t kHubNeighborsCount = kRoomCount * 0.1;

// Callback settings
constexpr bool kEnablePushForce = true;
constexpr double kPushForceScale = 10.0;
constexpr double kPushForceRange = 5.0;

constexpr double kRoomBloating = 1.5;

// Misc. (more of a test settings)
static constexpr bool kUniformRooms = false;  /// If enabled, only generates the first room type
constexpr TreeGenerationStrategy kTreeGenerationStrategy = TreeGenerationStrategy::RandomChildCount;

}  // namespace DungeonGeneration

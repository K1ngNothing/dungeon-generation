#pragma once

#include <ModelGenerator.h>

namespace DungeonGeneration {

// Main generator class that is responsible for whole generation flow
class DungeonGenerator {
    enum class DungeonType {
        Grid,
        TreeCenterDoors,
        TreeFixedDoors,
        TreeMovableDoors,
    };

public:
    DungeonGenerator() = default;
    Model::Model generateDungeon() const;

private:
    Model::Model generateModel() const;
    Model::Model runSolver(Model::Model&& model) const;

    // Setting for dungeon generation
    static constexpr DungeonType kDungeonType = DungeonType::TreeMovableDoors;
    static constexpr bool kEnablePushForce = true;
    static constexpr size_t kGridSide = 5;
    static constexpr size_t kRoomCount = 50;
};

}  // namespace DungeonGeneration

#pragma once

#include <model/Model.h>

namespace DungeonGeneration {

// Main generator class that is responsible for whole generation flow
class DungeonGenerator {
public:
    DungeonGenerator() = default;

    Model::Model generateDungeon() const;

private:
    Model::Model generateModel() const;
    Model::Model runSolver(Model::Model&& model) const;
};

}  // namespace DungeonGeneration

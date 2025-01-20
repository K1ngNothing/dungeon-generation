#pragma once

#include <filesystem>

#include <AnalyticalSolver.h>

#include "Door.h"
#include "Room.h"

namespace DungeonGenerator {
namespace Model {

struct Corridor {
    Door& door1;
    Door& door2;
};

struct Model {
    Rooms rooms;
    std::vector<Corridor> corridors;

    void dumpToSVG(const AnalyticalSolver::Solution& roomPositions, const std::filesystem::path& outputPath) const;
};

}  // namespace Model
}  // namespace DungeonGenerator

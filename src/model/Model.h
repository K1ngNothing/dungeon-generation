#pragma once

#include <filesystem>

#include <AnalyticalSolver.h>

#include "Corridor.h"
#include "Room.h"

namespace DungeonGenerator {
namespace Model {

struct Model {
public:
    Model() = default;
    Model(Rooms&& rooms, Corridors&& corridors);

    // Movable
    Model(Model&& other) = default;
    Model& operator=(Model&& other) = default;

    // Non-copyable
    Model(const Model& other) = delete;
    Model& operator=(const Model& other) = delete;

    const Rooms& getRooms() const;
    const Corridors& getCorridors() const;

    void setPositions(const Positions& roomPositions);

    // Very rough SVG dumper. It maybe will be removed in favor of SFML.
    void dumpToSVG(const Positions& roomPositions, const std::filesystem::path& outputPath) const;

private:
    Rooms rooms_;
    Corridors corridors_;
};

}  // namespace Model
}  // namespace DungeonGenerator

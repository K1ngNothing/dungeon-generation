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

    Room getRoom(size_t id) const;
    const Rooms& getRooms() const;
    const Corridors& getCorridors() const;

    size_t getObjectCount() const;
    size_t getVariablesCount() const;
    VariablesBounds getVariablesBounds() const;

    void setPositions(const Positions& roomPositions);

    // Very rough SVG dumper. It maybe will be removed in favor of SFML.
    void dumpToSVG(const std::filesystem::path& outputPath) const;

private:
    std::array<double, 4> calculateViewBox() const;

    Rooms rooms_;
    Corridors corridors_;
};

}  // namespace Model
}  // namespace DungeonGenerator

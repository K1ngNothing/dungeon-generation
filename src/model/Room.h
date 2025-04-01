#pragma once

#include <optional>
#include <vector>

#include <svgwrite/writer.hpp>

#include "Defs.h"
#include "Door.h"
#include "Variables.h"

namespace DungeonGenerator {
namespace Model {

struct Door;

struct Room {
    size_t id;
    double width;
    double height;
    std::vector<Door> doors;
    std::optional<Position> centerPosition;

    Variables getVariables(const double* x) const;
    VariablesIds getVariablesIds() const;
    std::optional<Position> getLBPosition() const;
    void dumpToSVG(svgw::writer& svgWriter) const;
};

using Rooms = std::vector<Room>;

}  // namespace Model
}  // namespace DungeonGenerator

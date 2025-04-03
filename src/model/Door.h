#pragma once

#include <optional>

#include <svgwrite/writer.hpp>

#include "Defs.h"
#include "Variables.h"

namespace DungeonGenerator {
namespace Model {

struct Room;

struct Door {
    // Shifts on each axis relative to parent room center
    double dx;
    double dy;

    std::reference_wrapper<Room> parentRoom;

    Variables getVariablesValues(const double* x) const;
    VariablesIds getVariablesIds() const;
    std::optional<Position> getCenterPosition() const;
    void dumpToSVG(svgw::writer& svgWriter) const;
};

}  // namespace Model
}  // namespace DungeonGenerator

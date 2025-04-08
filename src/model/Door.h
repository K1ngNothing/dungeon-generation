#pragma once

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
    size_t parentRoomId;

    Variables getVariablesValues(const double* x) const;
    VariablesIds getVariablesIds() const;
    void dumpToSVG(svgw::writer& svgWriter, Model::Position roomPosition) const;
};

}  // namespace Model
}  // namespace DungeonGenerator

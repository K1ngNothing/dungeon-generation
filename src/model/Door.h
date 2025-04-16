#pragma once

#include <svgwrite/writer.hpp>

#include "Defs.h"

namespace DungeonGenerator {
namespace Model {

struct Door {
    // Shifts on each axis relative to parent room center
    double dx;
    double dy;
    size_t parentRoomId;

    Position getPosition(const double* x) const;
    void dumpToSVG(svgw::writer& svgWriter, Model::Position roomPosition) const;
};

}  // namespace Model
}  // namespace DungeonGenerator

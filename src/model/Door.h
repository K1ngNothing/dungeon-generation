#pragma once

#include <optional>

#include <svgwrite/writer.hpp>

#include "Defs.h"
#include "Variables.h"

namespace DungeonGeneration {
namespace Model {

struct Door : ObjectWithVars {
    size_t parentRoomId;
    std::optional<Position> shift;  // Shift with regard to parent room's center

    Position getPositionFromVars(const double* x) const;
    void dumpToSVG(svgw::writer& svgWriter, Model::Position roomPosition) const;
};

}  // namespace Model
}  // namespace DungeonGeneration

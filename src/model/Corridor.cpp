#include "Corridor.h"

#include <cassert>

namespace DungeonGenerator {
namespace Model {

void Corridor::dumpToSVG(svgw::writer& svgWriter) const
{
    assert(
        door1.getCenterPosition().has_value() && door2.getCenterPosition().has_value() &&
        "Corridor::dumpToSVG: no position is set");

    Position pos1 = door1.getCenterPosition().value();
    Position pos2 = door2.getCenterPosition().value();
    constexpr double corridorWidth = 0.5;
    svgWriter.line(
        pos1.x, -pos1.y, pos2.x, -pos2.y,
        {
            {"stroke-width", corridorWidth},
            {      "stroke",        "blue"}
    });
}

}  // namespace Model
}  // namespace DungeonGenerator

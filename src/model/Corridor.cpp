#include "Corridor.h"

#include <cassert>

namespace DungeonGenerator {
namespace Model {

void Corridor::dumpToSVG(svgw::writer& svgWriter, const Model::Rooms& rooms) const
{
    const size_t id1 = door1.parentRoomId;
    const size_t id2 = door2.parentRoomId;
    assert(
        rooms[id1].centerPosition.has_value() && rooms[id2].centerPosition.has_value() &&
        "Corridor::dumpToSVG: no position is set");

    Position pos1 = rooms[id1].centerPosition.value();
    Position pos2 = rooms[id2].centerPosition.value();
    pos1.x += door1.dx;
    pos1.y += door1.dy;
    pos2.x += door2.dx;
    pos2.y += door2.dy;

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

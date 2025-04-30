#include "Corridor.h"

#include <cassert>

namespace DungeonGeneration {
namespace Model {

void Corridor::dumpToSVG(svgw::writer& svgWriter, const Model::Rooms& rooms) const
{
    const size_t id1 = door1.parentRoomId;
    const size_t id2 = door2.parentRoomId;
    assert(
        rooms[id1].centerPosition.has_value() && rooms[id2].centerPosition.has_value() &&
        "Corridor::dumpToSVG: no position is set");
    assert(door1.shift.has_value() && door2.shift.has_value() && "Corridor::dumpToSVG: door shifts aren't set");

    Position pos1 = rooms[id1].centerPosition.value();
    Position pos2 = rooms[id2].centerPosition.value();
    const auto [dx1, dy1] = door1.shift.value();
    const auto [dx2, dy2] = door2.shift.value();
    pos1.x += dx1;
    pos1.y += dy1;
    pos2.x += dx2;
    pos2.y += dy2;

    constexpr double corridorWidth = 0.5;
    svgWriter.line(
        pos1.x, -pos1.y, pos2.x, -pos2.y,
        {
            {"stroke-width", corridorWidth},
            {      "stroke",        "blue"}
    });
}

}  // namespace Model
}  // namespace DungeonGeneration

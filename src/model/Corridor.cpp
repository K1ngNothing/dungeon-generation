#include "Corridor.h"

#include <cassert>

namespace DungeonGeneration {
namespace Model {

void Corridor::dumpToSVG(svgw::writer& svgWriter, const Model::Rooms& rooms) const
{
    const size_t roomId1 = door1.parentRoomId();
    const size_t roomId2 = door2.parentRoomId();

    assert(door1.isPositionSet(rooms[roomId1]) && "Corridor::dumpToSVG: door position must be set");
    assert(door2.isPositionSet(rooms[roomId2]) && "Corridor::dumpToSVG: door position must be set");
    Position centerPos1 = door1.getCenterPosition(rooms[roomId1]);
    Position centerPos = door2.getCenterPosition(rooms[roomId2]);

    // TODO: remove hardcode
    constexpr double corridorWidth = 0.5;
    svgWriter.line(
        centerPos1.x, -centerPos1.y, centerPos.x, -centerPos.y,
        {
            {"stroke-width", corridorWidth},
            {      "stroke",        "blue"}
    });
}

}  // namespace Model
}  // namespace DungeonGeneration

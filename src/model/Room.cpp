#include "Room.h"

#include <cassert>

#include "SVGUtils.h"

namespace DungeonGeneration {
namespace Model {

std::optional<Position> Room::getLBPosition() const
{
    if (!centerPosition.has_value()) {
        return std::nullopt;
    }
    Position result = centerPosition.value();
    result.x -= width / 2;
    result.y -= height / 2;
    return result;
}

void Room::dumpToSVG(svgw::writer& svgWriter) const
{
    assert(centerPosition.has_value() && "Room:dumpToSVG: no position is set");
    Position lbPos = getLBPosition().value();

    const std::string text = "room " + std::to_string(id);
    svgWriter.write(SVGUtils::generateSVGRectangle(lbPos.x, lbPos.y, width, height, "yellow", text));

    for (const Door& door : doors) {
        door.dumpToSVG(svgWriter, centerPosition.value());
    }
}

}  // namespace Model
}  // namespace DungeonGeneration

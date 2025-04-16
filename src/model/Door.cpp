#include "Door.h"

#include <cassert>

#include "Room.h"
#include "SVGUtils.h"

namespace DungeonGenerator {
namespace Model {

Position Door::getPosition(const double* x) const
{
    const auto [roomX, roomY] = VarUtils::getVariablesVal(x, parentRoomId);
    return Position{.x = roomX + dx, .y = roomY + dy};
}

void Door::dumpToSVG(svgw::writer& svgWriter, Model::Position roomPosition) const
{
    // TODO: remove hard code
    constexpr double width = 2.5;
    constexpr double height = 2.5;

    const double lbPosX = roomPosition.x + dx - width / 2;
    const double lbPosY = roomPosition.y + dy - height / 2;
    svgWriter.write(SVGUtils::generateSVGRectangle(lbPosX, lbPosY, width, height, "red"));
}

}  // namespace Model
}  // namespace DungeonGenerator

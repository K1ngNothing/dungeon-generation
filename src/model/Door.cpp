#include "Door.h"

#include <cassert>

#include "Room.h"
#include "SVGUtils.h"

namespace DungeonGenerator {
namespace Model {

Variables Door::getVariablesValues(const double* x) const
{
    Variables result = Utils::getVariables(parentRoomId, x);
    result.varX += dx;
    result.varY += dy;
    return result;
}

VariablesIds Door::getVariablesIds() const
{
    return VariablesIds{.xId = parentRoomId * 2, .yId = parentRoomId * 2 + 1};
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

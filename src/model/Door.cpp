#include "Door.h"

#include <cassert>

#include "Room.h"
#include "SVGUtils.h"

namespace DungeonGeneration {
namespace Model {

Position Door::getPositionFromVars(const double* x) const
{
    const auto [roomX, roomY] = VarUtils::getVariablesVal(x, parentRoomId);
    const auto [doorDx, doorDy] = getVariablesVal(x);
    return Position{.x = roomX + doorDx, .y = roomY + doorDy};
}

void Door::dumpToSVG(svgw::writer& svgWriter, Model::Position roomPosition) const
{
    assert(shift.has_value() && "Door::dumpToSVG: shift value must be set");

    // TODO: remove hard code
    constexpr double width = 2.5;
    constexpr double height = 2.5;

    const auto [doorDx, doorDy] = shift.value();
    const double lbPosX = roomPosition.x + doorDx - width / 2;
    const double lbPosY = roomPosition.y + doorDy - height / 2;
    svgWriter.write(SVGUtils::generateSVGRectangle(lbPosX, lbPosY, width, height, "red"));
}

}  // namespace Model
}  // namespace DungeonGeneration

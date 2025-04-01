#include "Door.h"

#include <cassert>

#include "Room.h"
#include "SVGUtils.h"

namespace DungeonGenerator {
namespace Model {

Variables Door::getVariables(const double* x) const
{
    Variables result = Utils::getVariables(parentRoom.get().id, x);
    result.varX += dx;
    result.varY += dy;
    return result;
}

VariablesIds Door::getVariablesIds() const
{
    return VariablesIds{.xId = parentRoom.get().id * 2, .yId = parentRoom.get().id * 2 + 1};
}

std::optional<Position> Door::getPosition() const
{
    if (!parentRoom.get().centerPosition.has_value()) {
        return std::nullopt;
    }
    Position result = parentRoom.get().centerPosition.value();
    result.x += dx;
    result.y += dy;
    return result;
}

void Door::dumpToSVG(svgw::writer& svgWriter) const
{
    assert(getPosition().has_value() && "Doom:dumpToSVG: no position is set");

    Position position = getPosition().value();
    constexpr double width = 2.5;
    constexpr double height = 2.5;

    double lbPosX = position.x - width / 2;
    double lbPosY = position.y - height / 2;
    svgWriter.write(SVGUtils::getSVGRectangle(lbPosX, lbPosY, width, height, "red"));
}

}  // namespace Model
}  // namespace DungeonGenerator

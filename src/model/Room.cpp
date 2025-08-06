#include "Room.h"

#include <cassert>

#include "SVGUtils.h"

namespace DungeonGeneration {
namespace Model {

Room::Room(size_t id, double width, double height, std::vector<Door>&& doors, std::optional<Position> centerPosition)
      : ObjectWithVars(id),
        width_(width),
        height_(height),
        doors_(std::move(doors)),
        centerPosition_(centerPosition)
{
    assert(width_ > 0.0 && "Bad room width");
    assert(height_ > 0.0 && "Bad room height");
    for (const Door& door : doors_) {
        assert(door.parentRoomId() == varObjId_ && "Door has an incorrect parent room id");
    }
}

size_t Room::id() const
{
    return varObjId_;
}

const std::vector<Door>& Room::doors() const
{
    return doors_;
}

std::vector<Door>& Room::doorsMutable()
{
    return doors_;
}

double Room::width() const
{
    return width_;
}
double Room::height() const
{
    return height_;
}

Position Room::getLBPosition() const
{
    assert(centerPosition_.has_value() && "Room::getLBPosition: center position must be set");
    Position result = centerPosition_.value();
    result.x -= width_ / 2;
    result.y -= height_ / 2;
    return result;
}

Position Room::getCenterPosition() const
{
    assert(centerPosition_.has_value() && "Room::getCenterPosition: center position must be set");
    return centerPosition_.value();
}

bool Room::isPositionSet() const
{
    return centerPosition_.has_value();
}

void Room::setCenterPosition(Position centerPosition)
{
    centerPosition_ = centerPosition;
}

void Room::dumpToSVG(svgw::writer& svgWriter) const
{
    assert(isPositionSet() && "Room:dumpToSVG: room's position must be set");
    const Position lbPos = getLBPosition();

    const std::string text = "room " + std::to_string(varObjId_);
    svgWriter.write(SVGUtils::generateSVGRectangle(lbPos.x, lbPos.y, width_, height_, "yellow", text));

    for (const Door& door : doors_) {
        door.dumpToSVG(svgWriter, *this);
    }
}

}  // namespace Model
}  // namespace DungeonGeneration

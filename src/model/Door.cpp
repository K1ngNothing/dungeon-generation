#include "Door.h"

#include <cassert>

#include "Room.h"
#include "SVGUtils.h"

namespace DungeonGeneration {
namespace Model {

Door Door::createMovableDoor(size_t parentRoomId, size_t doorId)
{
    Door door;
    door.parentRoomId_ = parentRoomId;
    door.varObjId_ = doorId;
    return door;
}

Door Door::createFixedDoor(size_t parentRoomId, Position shift)
{
    Door door;
    door.parentRoomId_ = parentRoomId;
    door.shift_ = shift;
    return door;
}

size_t Door::parentRoomId() const
{
    return parentRoomId_;
}

size_t Door::varObjectId() const
{
    assert(isMovable() && "Door::getVarObjectId(): only movable doors have variables");
    return varObjId_.value();
}

Position Door::getCenterPositionFromVars(const double* x) const
{
    assert(x && "Null variables array");

    const auto [roomX, roomY] = VarUtils::getVariablesVal(x, parentRoomId_);
    double doorDx = 0.0;
    double doorDy = 0.0;
    if (hasVariables()) {
        const Variables vars = getVariablesVal(x);
        doorDx = vars.varX;
        doorDy = vars.varY;
    } else {
        assert(shift_.has_value() && "Fixed door should have a defined shift");
        doorDx = shift_.value().x;
        doorDy = shift_.value().y;
    }
    return Position{.x = roomX + doorDx, .y = roomY + doorDy};
}

Position Door::getCenterPosition(const Model::Room& parentRoom) const
{
    assert(parentRoom.id() == parentRoomId_ && "Door::getPosition: incorrect parent room");
    assert(shift_.has_value() && "Door::getPosition: shift must be set");

    Position result = parentRoom.getCenterPosition();
    result.x += shift_->x;
    result.y += shift_->y;
    return result;
}

bool Door::isMovable() const
{
    return hasVariables();
}

bool Door::isPositionSet(const Room& parentRoom) const
{
    assert(parentRoom.id() == parentRoomId_ && "Door::isPositionSet: Incorrect parent rooms");
    return parentRoom.isPositionSet() && (isMovable() || shift_.has_value());
}

void Door::setShift(Position shift)
{
    assert(isMovable() && "Door::setShift: fixed doors should have a constant shift");
    shift_ = shift;
}

void Door::dumpToSVG(svgw::writer& svgWriter, const Model::Room& parentRoom) const
{
    assert(parentRoom.id() == parentRoomId_ && "Corridor::dumpToSVG: incorrect parent room is passed");
    assert(isPositionSet(parentRoom) && "Corridor::dumpToSVG: door position must be set");

    // TODO: remove hard code
    constexpr double width = 2.5;
    constexpr double height = 2.5;

    const auto [roomX, roomY] = parentRoom.getCenterPosition();
    const auto [doorDx, doorDy] = shift_.value();
    const double lbPosX = roomX + doorDx - width / 2;
    const double lbPosY = roomY + doorDy - height / 2;
    svgWriter.write(SVGUtils::generateSVGRectangle(lbPosX, lbPosY, width, height, "red"));
}

}  // namespace Model
}  // namespace DungeonGeneration

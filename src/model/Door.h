#pragma once

#include <optional>

#include <svgwrite/writer.hpp>

#include "Defs.h"
#include "Variables.h"

namespace DungeonGeneration {
namespace Model {

class Room;

class Door : public ObjectMaybeWithVars {
public:
    Door() = default;
    static Door createMovableDoor(size_t parentRoomId, size_t doorId);
    static Door createFixedDoor(size_t parentRoomId, Position shift);

    size_t parentRoomId() const;
    size_t varObjectId() const;

    Position getCenterPositionFromVars(const double* x) const;
    Position getCenterPosition(const Model::Room& parentRoom) const;

    bool isMovable() const;
    /// Checks whether door has a fixed position. Used in asserts.
    bool isPositionSet(const Room& parentRoom) const;

    void setShift(Position shift);
    void dumpToSVG(svgw::writer& svgWriter, const Model::Room& parentRoom) const;

private:
    size_t parentRoomId_;
    std::optional<Position> shift_;  // Shift with regard to parent room's center
};

}  // namespace Model
}  // namespace DungeonGeneration

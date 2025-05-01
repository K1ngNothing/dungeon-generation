#pragma once

#include <optional>
#include <vector>

#include <svgwrite/writer.hpp>

#include "Defs.h"
#include "Door.h"
#include "Variables.h"

namespace DungeonGeneration {
namespace Model {

struct Door;

class Room : public ObjectWithVars {
public:
    Room() = default;
    Room(
        size_t id, double width, double height, std::vector<Door>&& doors,
        std::optional<Position> centerPosition = std::nullopt);

    size_t id() const;
    const std::vector<Door>& doors() const;
    std::vector<Door>& doorsMutable();
    double width() const;
    double height() const;

    Position getLBPosition() const;
    Position getCenterPosition() const;

    /// Checks whether room has a fixed position. Used in asserts.
    bool isPositionSet() const;

    void setCenterPosition(Position centerPosition);
    void dumpToSVG(svgw::writer& svgWriter) const;

private:
    double width_;
    double height_;
    std::vector<Door> doors_;
    std::optional<Position> centerPosition_;
};

using Rooms = std::vector<Room>;

}  // namespace Model
}  // namespace DungeonGeneration

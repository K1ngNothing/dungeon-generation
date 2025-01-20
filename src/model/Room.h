#pragma once

#include <vector>

#include "Door.h"
#include "Variables.h"

namespace DungeonGenerator {
namespace Model {

struct Room {
    size_t id;
    double width;
    double height;
    std::vector<Door> doors;

    Variables getVariables(const double* x) const;
    VariablesIds getVariablesIds() const;
};

using Rooms = std::vector<Room>;

}  // namespace Model
}  // namespace DungeonGenerator

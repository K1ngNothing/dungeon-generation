#pragma once

#include <vector>

namespace DungeonGeneration {
namespace Model {

struct Position {
    double x;
    double y;
};
using Positions = std::vector<Position>;

}  // namespace Model
}  // namespace DungeonGeneration

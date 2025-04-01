#include "ModelGenerator.h"

#include <cassert>

namespace DungeonGenerator {

Model::Model ModelGenerator::generateDungeon(size_t roomCount) const
{
    // TODO: implement
    throw std::runtime_error("ModelGenerator::generateDungeon is not implemented yet");
}

/*
Generates dungeon with this structure with fixed grid side (example's for gidSide = 2):
┌───┐ ┌───┐
│ 0 ├─┤ 1 │
└─┬─┘ └─┬─┘
┌─┴─┐ ┌─┴─┐
│ 2 ├─┤ 3 │
└───┘ └───┘
In code doors in a room are indexed in this way:
    2
  ┌─┴─┐
1─┤   ├─3
  └─┬─┘
    0
*/
Model::Model ModelGenerator::generateGrid(size_t gridSide) const
{
    constexpr double roomWidth = 20.0;
    constexpr double roomHeight = 20.0;
    // Distances between rooms on X and Y axises
    constexpr double dx = roomWidth / 4;
    constexpr double dy = roomHeight / 4;
    auto getRoomId = [gridSide](size_t row, size_t col) -> size_t {
        assert(row < gridSide && col < gridSide && "Invalid grid coordinates");
        return row * gridSide + col;
    };

    // 1. Create rooms and doors
    Model::Rooms rooms(gridSide * gridSide);
    for (size_t row = 0; row < gridSide; ++row) {
        for (size_t col = 0; col < gridSide; ++col) {
            size_t roomId = getRoomId(row, col);
            rooms[roomId].id = roomId;
            rooms[roomId].width = roomWidth;
            rooms[roomId].height = roomHeight;
            rooms[roomId].doors = {
                Model::Door{           .dx = 0.0, .dy = -roomHeight / 2, .parentRoom = rooms[roomId]},
                Model::Door{.dx = -roomWidth / 2,             .dy = 0.0, .parentRoom = rooms[roomId]},
                Model::Door{           .dx = 0.0,  .dy = roomHeight / 2, .parentRoom = rooms[roomId]},
                Model::Door{ .dx = roomWidth / 2,             .dy = 0.0, .parentRoom = rooms[roomId]},
            };
            rooms[roomId].centerPosition =
                Model::Position{.x = col * (roomWidth + dx), .y = (gridSide - row - 1) * (roomHeight + dy)};
        }
    }

    // 2. Add corridors
    std::vector<Model::Corridor> corridors;
    for (size_t row = 0; row < gridSide; ++row) {
        for (size_t col = 0; col < gridSide; ++col) {
            size_t curRoom = getRoomId(row, col);
            if (row < gridSide - 1) {
                size_t roomDown = getRoomId(row + 1, col);
                corridors.push_back({rooms[curRoom].doors[0], rooms[roomDown].doors[2]});
            }
            if (col < gridSide - 1) {
                size_t roomRight = getRoomId(row, col + 1);
                corridors.push_back({rooms[curRoom].doors[3], rooms[roomRight].doors[1]});
            }
        }
    }
    return Model::Model{std::move(rooms), std::move(corridors)};
}

Model::Model ModelGenerator::generateTree(size_t roomCount) const
{
    // TODO: implement
    throw std::runtime_error("ModelGenerator::generateTree is not implemented yet");
}

}  // namespace DungeonGenerator

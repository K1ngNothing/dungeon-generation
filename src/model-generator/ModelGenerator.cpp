#include "ModelGenerator.h"

#include <cassert>

namespace DungeonGenerator {

namespace {

Model::Room createRoomFourDoors(size_t id, double width, double height, Model::Position center = {0, 0})
{
    const double doorDx = width * 0.4;
    const double doorDy = height * 0.4;

    Model::Room result;
    result.id = id;
    result.width = width;
    result.height = height;
    result.doors = {
        Model::Door{    .dx = 0.0, .dy = -doorDy, .parentRoomId = id},
        Model::Door{.dx = -doorDx,     .dy = 0.0, .parentRoomId = id},
        Model::Door{    .dx = 0.0,  .dy = doorDy, .parentRoomId = id},
        Model::Door{ .dx = doorDx,     .dy = 0.0, .parentRoomId = id},
    };
    result.centerPosition = center;
    return result;
}

}  // namespace

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
    constexpr double roomWidth = 35.0;
    constexpr double roomHeight = 35.0;
    // Distances between rooms on X and Y axises
    constexpr double roomDistX = roomWidth / 4;
    constexpr double roomDistY = roomHeight / 4;
    auto getRoomId = [gridSide](size_t row, size_t col) -> size_t {
        assert(row < gridSide && col < gridSide && "Invalid grid coordinates");
        return row * gridSide + col;
    };

    // 1. Create rooms and doors
    Model::Rooms rooms;
    rooms.reserve(gridSide * gridSide);
    for (size_t row = 0; row < gridSide; ++row) {
        for (size_t col = 0; col < gridSide; ++col) {
            const size_t roomId = getRoomId(row, col);
            const Model::Position center{
                .x = col * (roomWidth + roomDistX), .y = (gridSide - row - 1) * (roomHeight + roomDistY)};
            rooms.emplace_back(createRoomFourDoors(roomId, roomWidth, roomHeight, center));
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

/// Generates a dungeon based on random tree. For simplicity's sake we set each door
/// to be in the center of the room (which automatically avoids a need for rotations).
Model::Model ModelGenerator::generateTreeCenterRooms(size_t roomCount)
{
    assert(roomCount > 0);
    std::vector<std::pair<int, int>> roomsDimensions{
        {20, 20},
        {20, 40},
        {40, 20},
        {30, 30}
    };

    // 1. Create rooms and doors
    Model::Rooms rooms(roomCount);
    for (size_t roomId = 0; roomId < roomCount; ++roomId) {
        const auto [roomWidth, roomHeight] = roomsDimensions[rng_() % roomsDimensions.size()];
        rooms[roomId].id = roomId;
        rooms[roomId].width = roomWidth;
        rooms[roomId].height = roomHeight;
        rooms[roomId].doors = {
            Model::Door{.dx = 0.0, .dy = 0.0, .parentRoomId = roomId}
        };
    }

    // 2. Add corridors
    std::vector<Model::Corridor> corridors;
    corridors.reserve(roomCount - 1);
    for (size_t roomId = 1; roomId < roomCount; ++roomId) {
        size_t otherRoom = rng_() % roomId;
        corridors.push_back({rooms[roomId].doors[0], rooms[otherRoom].doors[0]});
    }
    return Model::Model{std::move(rooms), std::move(corridors)};
}

Model::Model ModelGenerator::generateTreeDungeon(size_t roomCount)
{
    assert(roomCount > 0);
    std::vector<std::pair<int, int>> roomsDimensions{
        {20, 20},
 // {20, 40},
  // {40, 20},
  // {30, 30}
    };
    Model::Rooms rooms;
    rooms.reserve(roomCount);
    for (size_t roomId = 0; roomId < roomCount; ++roomId) {
        const auto [roomWidth, roomHeight] = roomsDimensions[rng_() % roomsDimensions.size()];
        rooms.emplace_back(createRoomFourDoors(roomId, roomWidth, roomHeight));
    }

    // Algorithm: choose random room with id < roomId and connect them East-West or North-South
    std::vector<Model::Corridor> corridors;
    std::vector<std::array<bool, 4>> availableDoors(roomCount, {true, true, true, true});
    for (size_t roomId = 1; roomId < roomCount; ++roomId) {
        bool connectionAdded = false;
        while (!connectionAdded) {
            size_t otherRoom = rng_() % roomId;

            // Find possible connections between chosen rooms
            std::vector<size_t> possibleConnections;
            for (size_t side = 0; side < 4; ++side) {
                size_t otherSide = (side + 2) % 4;
                if (availableDoors[roomId][side] && availableDoors[otherRoom][otherSide]) {
                    possibleConnections.push_back(side);
                }
            }

            // Pick random possible connection
            if (possibleConnections.empty()) {
                continue;
            }
            size_t side = possibleConnections[rng_() % possibleConnections.size()];
            size_t otherSide = (side + 2) % 4;
            corridors.push_back({rooms[roomId].doors[side], rooms[otherRoom].doors[otherSide]});
            availableDoors[roomId][side] = false;
            availableDoors[otherRoom][otherSide] = false;
            connectionAdded = true;
        }
    }
    return Model::Model{std::move(rooms), std::move(corridors)};
}

}  // namespace DungeonGenerator

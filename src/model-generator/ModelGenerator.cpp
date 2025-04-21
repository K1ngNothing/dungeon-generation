#include "ModelGenerator.h"

#include <cassert>

namespace DungeonGenerator {

namespace {

Model::Room createRoomFourDoors(size_t id, double width, double height, Model::Position center = {0, 0})
{
    const double doorDx = width * 0.4;
    const double doorDy = height * 0.4;

    Model::Room result;
    // TODO: fix
    // result.id = id;
    // result.width = width;
    // result.height = height;
    // result.doors = {
    //     Model::Door{    .dx = 0.0, .dy = -doorDy, .parentRoomId = id},
    //     Model::Door{.dx = -doorDx,     .dy = 0.0, .parentRoomId = id},
    //     Model::Door{    .dx = 0.0,  .dy = doorDy, .parentRoomId = id},
    //     Model::Door{ .dx = doorDx,     .dy = 0.0, .parentRoomId = id},
    // };
    // result.centerPosition = center;
    return result;
}

bool isTree(const ModelGenerator::Graph& graph)
{
    const size_t n = graph.size();
    if (n == 0) {
        return true;
    }

    // Check edge count
    size_t edgeCount = 0;
    for (size_t v = 0; v < n; ++v) {
        edgeCount += graph[v].size();
    }
    edgeCount /= 2;  // Each edge is counted twice
    if (edgeCount != n - 1) {
        return false;
    }

    // Check connectivity
    std::vector<bool> visited(n);
    std::function<void(size_t)> dfs = [&graph, &visited, &dfs](size_t v) {
        visited[v] = true;
        for (const size_t u : graph[v]) {
            if (!visited[u]) {
                dfs(u);
            }
        }
    };
    dfs(0);
    for (size_t v = 0; v < n; ++v) {
        if (!visited[v]) {
            return false;
        }
    }
    return true;
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
    }

    // 2. Decide what rooms will be connected
    std::vector<std::vector<size_t>> graph = generateTree(roomCount);

    //  3. Add corridors: for each corridor we create a pair of movable rooms
    // (!) We must be very careful with door references in Corridors
    for (size_t roomId = 0; roomId < roomCount; ++roomId) {
        const size_t doorCount = graph[roomId].size();
        rooms[roomId].doors.reserve(doorCount);
    }

    std::vector<Model::Corridor> corridors;
    size_t freeDoorId = roomCount;
    corridors.reserve(roomCount - 1);
    for (size_t room1 = 0; room1 < roomCount; ++room1) {
        for (size_t room2 : graph[room1]) {
            assert(room1 != room2 && "Loop in the tree");
            if (room1 >= room2) {
                continue;
            }
            rooms[room1].doors.push_back(Model::Door{{.id = freeDoorId++}, .parentRoomId = room1});
            rooms[room2].doors.push_back(Model::Door{{.id = freeDoorId++}, .parentRoomId = room2});
            corridors.push_back({rooms[room1].doors.back(), rooms[room2].doors.back()});
        }
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

ModelGenerator::Graph ModelGenerator::generateTree(size_t vertexCount)
{
    switch (kTreeGenerationStrategy) {
        case TreeGenerationStrategy::RandomPredecessors:
            return generateTreePredecessorStrategy(vertexCount);
        case TreeGenerationStrategy::RandomChildCount:
            return generateTreeChildCountStrategy(vertexCount);
        default:
            assert(false && "Unknown tree generation strategy");
            return Graph();
    }
}

ModelGenerator::Graph ModelGenerator::generateTreePredecessorStrategy(size_t vertexCount)
{
    std::vector<std::vector<size_t>> graph(vertexCount);
    for (size_t v = 1; v < vertexCount; ++v) {
        const size_t u = rng_() % v;
        graph[v].push_back(u);
        graph[u].push_back(v);
    }
    assert(isTree(graph) && "Resulting graph isn't a tree :(");
    return graph;
}

ModelGenerator::Graph ModelGenerator::generateTreeChildCountStrategy(size_t vertexCount)
{
    constexpr size_t maxNeighborsCount = 4;

    std::vector<std::vector<size_t>> graph(vertexCount);
    if (vertexCount == 1) {
        // Just to be safe
        return graph;
    }

    size_t disconnectedVertex = 1;
    for (size_t v = 0; v < vertexCount; ++v) {
        assert(disconnectedVertex > v && "Current vertex isn't connected to the graph");
        assert(disconnectedVertex <= vertexCount && "disconnectedVertex is too big");

        size_t maxChildrenCount = (v == 0 ? maxNeighborsCount : maxNeighborsCount - 1);
        maxChildrenCount = std::min(maxChildrenCount, vertexCount - disconnectedVertex);
        assert(disconnectedVertex + maxChildrenCount <= vertexCount && "Too many children nodes");

        size_t childrenCount = rng_() % (maxChildrenCount + 1);  // [0; maxChildrenCount]
        if (childrenCount == 0 && v + 1 != vertexCount && disconnectedVertex == v + 1) {
            // The next vertex won't be connected => the whole graph will be disconnected
            childrenCount = 1;
        }
        for (size_t childNum = 0; childNum < childrenCount; ++childNum) {
            assert(disconnectedVertex < vertexCount && "Invalid graph edge");
            graph[v].push_back(disconnectedVertex);
            graph[disconnectedVertex].push_back(v);
            disconnectedVertex++;
        }
    }
    assert(isTree(graph) && "Resulting graph isn't a tree :(");
    return graph;
}

}  // namespace DungeonGenerator

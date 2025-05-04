#include "ModelGenerator.h"

#include <cassert>

namespace DungeonGeneration {

namespace {
/*
Create a room with doors placed this way
    2
  ┌─┴─┐
1─┤   ├─3
  └─┬─┘
    0
*/
Model::Room createRoomFourFixedDoors(
    size_t roomId, double width, double height, Model::Position centerPosition = {0, 0})
{
    using namespace Model;

    const double doorDx = width * 0.4;
    const double doorDy = height * 0.4;

    std::vector<Door> doors{
        Door::createFixedDoor(roomId, Position{.x = 0.0, .y = -doorDy}),
        Door::createFixedDoor(roomId, Position{.x = -doorDx, .y = 0.0}),
        Door::createFixedDoor(roomId, Position{.x = 0.0, .y = doorDy}),
        Door::createFixedDoor(roomId, Position{.x = doorDx, .y = 0.0}),
    };

    return Room(roomId, width, height, std::move(doors), centerPosition);
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
    const size_t roomCount = gridSide * gridSide;
    Model::Rooms rooms;
    rooms.reserve(roomCount);
    for (size_t row = 0; row < gridSide; ++row) {
        for (size_t col = 0; col < gridSide; ++col) {
            const size_t roomId = getRoomId(row, col);
            const Model::Position centerPosition{
                .x = col * (roomWidth + roomDistX), .y = (gridSide - row - 1) * (roomHeight + roomDistY)};
            rooms.emplace_back(createRoomFourFixedDoors(roomId, roomWidth, roomHeight, centerPosition));
        }
    }

    // 2. Add corridors
    std::vector<Model::Corridor> corridors;
    corridors.reserve(roomCount);
    for (size_t row = 0; row < gridSide; ++row) {
        for (size_t col = 0; col < gridSide; ++col) {
            size_t curRoom = getRoomId(row, col);
            if (row < gridSide - 1) {
                size_t roomDown = getRoomId(row + 1, col);
                corridors.push_back({rooms[curRoom].doorsMutable()[0], rooms[roomDown].doorsMutable()[2]});
            }
            if (col < gridSide - 1) {
                size_t roomRight = getRoomId(row, col + 1);
                corridors.push_back({rooms[curRoom].doorsMutable()[3], rooms[roomRight].doorsMutable()[1]});
            }
        }
    }
    return Model::Model{std::move(rooms), std::move(corridors)};
}

/// Generates a dungeon with random tree structure with doors in the center of the rooms.
Model::Model ModelGenerator::generateTreeCenterDoors(size_t roomCount)
{
    assert(roomCount > 0);

    // 1. Create rooms and doors
    Model::Rooms rooms(roomCount);
    for (size_t roomId = 0; roomId < roomCount; ++roomId) {
        const auto [roomWidth, roomHeight] = generateRoom(roomId);
        std::vector<Model::Door> doors{Model::Door::createFixedDoor(roomId, Model::Position{.x = 0.0, .y = 0.0})};
        rooms[roomId] = Model::Room(roomId, roomWidth, roomHeight, std::move(doors));
    }

    // 2. Add corridors
    std::vector<Model::Corridor> corridors;
    corridors.reserve(roomCount - 1);
    for (size_t roomId = 1; roomId < roomCount; ++roomId) {
        size_t otherRoom = rng_() % roomId;
        corridors.push_back({rooms[roomId].doorsMutable()[0], rooms[otherRoom].doorsMutable()[0]});
    }
    return Model::Model{std::move(rooms), std::move(corridors)};
}

/// Generates a dungeon with random tree structure with four fixed doors on each room's side.
/// Rooms are only connected East-West or North-South so that solution is at least feasible.
Model::Model ModelGenerator::generateTreeFixedDoors(size_t roomCount)
{
    assert(roomCount > 0);
    Model::Rooms rooms;
    rooms.reserve(roomCount);
    for (size_t roomId = 0; roomId < roomCount; ++roomId) {
        const auto [roomWidth, roomHeight] = generateRoom(roomId);
        rooms.emplace_back(createRoomFourFixedDoors(roomId, roomWidth, roomHeight));
    }

    // Algorithm: choose random room with id < roomId and connect them East-West or North-South
    std::vector<Model::Corridor> corridors;
    std::vector<std::array<bool, 4>> availableDoors(roomCount, {true, true, true, true});
    for (size_t roomId = 1; roomId < roomCount; ++roomId) {
        bool connectionAdded = false;
        while (!connectionAdded) {
            size_t otherRoom = Random::uniformDiscrete(roomId - 1, rng_);

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
            size_t side = possibleConnections[Random::uniformDiscrete(possibleConnections.size() - 1, rng_)];
            size_t otherSide = (side + 2) % 4;
            corridors.push_back({rooms[roomId].doorsMutable()[side], rooms[otherRoom].doorsMutable()[otherSide]});
            availableDoors[roomId][side] = false;
            availableDoors[otherRoom][otherSide] = false;
            connectionAdded = true;
        }
    }
    return Model::Model{std::move(rooms), std::move(corridors)};
}

/// Generates a dungeon with random tree structure with movable doors. Each edge just adds a pair of movable doors.
Model::Model ModelGenerator::generateTreeMovableDoors(size_t roomCount)
{
    assert(roomCount > 0);

    // 1. Create rooms and doors
    Model::Rooms rooms(roomCount);
    for (size_t roomId = 0; roomId < roomCount; ++roomId) {
        const auto [roomWidth, roomHeight] = generateRoom(roomId);
        rooms[roomId] = Model::Room(roomId, roomWidth, roomHeight, {});
    }

    // 2. Decide what rooms will be connected
    std::vector<std::vector<size_t>> graph = generateTree(roomCount);

    //  3. Add corridors: for each corridor we create a pair of movable rooms
    // (!) We must be very careful with door references in Corridors
    for (size_t roomId = 0; roomId < roomCount; ++roomId) {
        const size_t doorCount = graph[roomId].size();
        rooms[roomId].doorsMutable().reserve(doorCount);
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
            std::vector<Model::Door>& doors1 = rooms[room1].doorsMutable();
            std::vector<Model::Door>& doors2 = rooms[room2].doorsMutable();
            doors1.push_back(Model::Door::createMovableDoor(room1, freeDoorId++));
            doors2.push_back(Model::Door::createMovableDoor(room2, freeDoorId++));
            corridors.push_back({doors1.back(), doors2.back()});
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
        const size_t u = Random::uniformDiscrete(v - 1, rng_);
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

        size_t maxChildrenCount = maxNeighborsCount - 1;
        if (v == 0) {
            maxChildrenCount = (kEnableHubRoom ? kHubNeighborsCount : maxNeighborsCount);
        }
        maxChildrenCount = std::min(maxChildrenCount, vertexCount - disconnectedVertex);
        assert(disconnectedVertex + maxChildrenCount <= vertexCount && "Too many children nodes");

        // Room 0 always has the maximum allowed room count
        size_t childrenCount = (v == 0 ? maxChildrenCount : Random::uniformDiscrete(maxChildrenCount, rng_));
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

ModelGenerator::RoomDimensions ModelGenerator::generateRoom(size_t roomId)
{
    if (kUniformRooms) {
        return kRegularRoomTypes[0].dimensions;
    }
    if (roomId == 0 && kEnableHubRoom) {
        return generateRoomFromDistribution(kHubRoomTypes);
    }
    return generateRoomFromDistribution(kRegularRoomTypes);
}

ModelGenerator::RoomDimensions ModelGenerator::generateRoomFromDistribution(const std::vector<RoomType>& roomTypes)
{
    // TODO: be careful about constant weights copying. For now it's fine because we don't have a lot of room types.
    const size_t n = roomTypes.size();
    std::vector<double> weights(n);
    for (size_t i = 0; i < n; ++i) {
        weights[i] = roomTypes[i].distributionWeight;
    }
    const size_t generatedRoom = Random::fromDistribution(weights, rng_);
    return roomTypes[generatedRoom].dimensions;
}

}  // namespace DungeonGeneration

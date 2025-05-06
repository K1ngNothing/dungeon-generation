#include "GraphGenerator.h"

#include <cassert>

#include "Settings.h"

namespace DungeonGeneration {

namespace {
bool isTree(const GraphGenerator::Graph& graph)
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

GraphGenerator::Graph GraphGenerator::generateTree(size_t vertexCount)
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

GraphGenerator::Graph GraphGenerator::generateConnectedGraph(size_t vertexCount, size_t additionalEdges)
{
    Graph graph = generateTree(vertexCount);

    // TODO: be careful if vertex count will be too high
    std::vector<std::vector<bool>> adjacencyMatrix(vertexCount, std::vector<bool>(vertexCount));
    for (size_t v = 0; v < vertexCount; ++v) {
        adjacencyMatrix[v][v] = true;
        for (const size_t u : graph[v]) {
            adjacencyMatrix[v][u] = true;
            adjacencyMatrix[u][v] = true;
        }
    }

    // Add `additionalEdges` random edges to the tree. Avoid multi-edges.
    for (size_t edgeNum = 0; edgeNum < additionalEdges;) {
        const size_t v = Random::uniformDiscrete(vertexCount - 1, rng_);
        const size_t u = Random::uniformDiscrete(vertexCount - 1, rng_);
        if (adjacencyMatrix[v][u]) {
            continue;
        }

        adjacencyMatrix[v][u] = true;
        adjacencyMatrix[u][v] = true;
        graph[v].push_back(u);
        graph[u].push_back(v);
        edgeNum++;
    }

    return graph;
}

GraphGenerator::Graph GraphGenerator::generateTreePredecessorStrategy(size_t vertexCount)
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

GraphGenerator::Graph GraphGenerator::generateTreeChildCountStrategy(size_t vertexCount)
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

}  // namespace DungeonGeneration

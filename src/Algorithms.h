#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <vector>
#include <string>
#include "Graph.h"

// Result structures
struct PathResult {
    bool found;
    double totalDistance;
    std::vector<int> path;
    double travelTimeMinutes;

    PathResult() : found(false), totalDistance(0.0), travelTimeMinutes(0.0) {}
};

struct TraversalResult {
    std::vector<int> visitedNodes;
    std::vector<std::string> steps;
    bool completed;

    TraversalResult() : completed(false) {}
};

// Algorithm implementations
PathResult dijkstra(const Graph& graph, int sourceId, int destId, bool verbose = true);
TraversalResult bfs(const Graph& graph, int startId, bool verbose = true);
TraversalResult dfs(const Graph& graph, int startId, bool verbose = true);

// Helper functions
void displayPath(const Graph& graph, const PathResult& result);
void displayTraversal(const Graph& graph, const TraversalResult& result, const std::string& algorithmName);

#endif

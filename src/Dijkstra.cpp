#include "Algorithms.h"
#include "utils/colors.h"
#include "utils/keyUtils.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <thread>
#include <chrono>

const double INF = std::numeric_limits<double>::max();

PathResult dijkstra(const Graph& graph, int sourceId, int destId, bool verbose) {
    PathResult result;

    if (!graph.nodeExists(sourceId)) {
        if (verbose) std::cout << "Error: Source node " << sourceId << " not found!\n";
        return result;
    }

    if (!graph.nodeExists(destId)) {
        if (verbose) std::cout << "Error: Destination node " << destId << " not found!\n";
        return result;
    }

    // Initialize
    double dist[MAX_NODES];
    int prev[MAX_NODES];
    bool visited[MAX_NODES];

    for (int i = 0; i < MAX_NODES; i++) {
        dist[i] = INF;
        prev[i] = -1;
        visited[i] = false;
    }

    dist[sourceId] = 0;

    if (verbose) {
        system("cls");
        std::cout << BOLD << "Dijkstra's shortest path algorithm" << RESET << "\n";
        std::cout << "Source: [" << sourceId << "] " << graph.getNodeName(sourceId) << "\n";
        std::cout << "Destination: [" << destId << "] " << graph.getNodeName(destId) << "\n\n";
        std::cout << "Initializing algorithm...\n";
        if (sleepWithEscCheck(500)) {
            std::cout << "\nOperation aborted by user.\n";
            return result;
        }
    }

    int step = 0;
    const Node* nodes = graph.getNodes();

    // Main algorithm
    for (int count = 0; count < graph.getNodeCount(); count++) {
        // Find minimum distance node
        double minDist = INF;
        int u = -1;

        for (int i = 0; i < MAX_NODES; i++) {
            if (nodes[i].active && !visited[i] && dist[i] < minDist) {
                minDist = dist[i];
                u = i;
            }
        }

        if (u == -1 || minDist == INF) break;

        visited[u] = true;
        step++;

        if (verbose) {
            system("cls");
            std::cout << BOLD << "Dijkstra's algorithm - Step " << step << RESET << "\n";
            std::cout << CYAN << "Current Node: [" << nodes[u].id << "] " << nodes[u].name << RESET << "\n";
            std::cout << "Distance from source: " << dist[u] << " km\n\n";

            std::cout << "Distance Table:\n";
            std::cout << std::setw(6) << "ID" << " | "
                      << std::setw(20) << "Name" << " | "
                      << std::setw(12) << "Distance" << " | "
                      << std::setw(10) << "Status" << "\n";
            std::cout << std::string(55, '-') << "\n";

            for (int i = 0; i < MAX_NODES; i++) {
                if (nodes[i].active) {
                    std::cout << std::setw(6) << nodes[i].id << " | "
                              << std::setw(20) << nodes[i].name << " | ";

                    if (dist[i] == INF) {
                        std::cout << std::setw(12) << "INF" << " | ";
                    } else {
                        std::cout << std::setw(12) << std::fixed << std::setprecision(1) << dist[i] << " | ";
                    }

                    if (visited[i]) {
                        std::cout << std::setw(10) << "Visited";
                    } else if (i == u) {
                        std::cout << CYAN << std::setw(10) << "Current" << RESET;
                    } else {
                        std::cout << std::setw(10) << "Pending";
                    }
                    std::cout << "\n";
                }
            }

            if (sleepWithEscCheck(500)) {
                std::cout << "\nOperation aborted by user.\n";
                return result;
            }
        }

        // Update distances
        Edge* edge = nodes[u].adjacencyList;
        while (edge != nullptr) {
            int v = edge->destination;
            double weight = edge->weight;

            if (!visited[v] && dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                prev[v] = nodes[u].id;
            }

            edge = edge->next;
        }

        // Check if we reached destination
        if (u == destId) {
            break;
        }
    }

    // Build path
    if (dist[destId] != INF) {
        result.found = true;
        result.totalDistance = dist[destId];
        result.travelTimeMinutes = (dist[destId] / 60.0) * 60.0; // km / (km/h) * 60

        // Reconstruct path
        std::vector<int> reversePath;
        int current = destId;
        while (current != -1) {
            reversePath.push_back(current);
            bool found = false;
            for (int i = 0; i < MAX_NODES; i++) {
                if (nodes[i].active && nodes[i].id == current) {
                    current = prev[i];
                    found = true;
                    break;
                }
            }
            if (!found) break;
        }

        // Reverse to get correct order
        for (int i = reversePath.size() - 1; i >= 0; i--) {
            result.path.push_back(reversePath[i]);
        }
    }

    return result;
}

void displayPath(const Graph& graph, const PathResult& result) {
    system("cls");
    std::cout << BOLD << "Path result" << RESET << "\n";

    if (!result.found) {
        std::cout << "No path found between the nodes!\n";
        std::cout << "The nodes may be disconnected.\n";
        return;
    }

    std::cout << CYAN << "Path found!\n" << RESET;
    std::cout << "Total Distance: " << std::fixed << std::setprecision(2)
              << result.totalDistance << " km\n";
    std::cout << "Travel Time: " << std::fixed << std::setprecision(1)
              << result.travelTimeMinutes << " minutes\n";
    std::cout << "(at constant velocity of 60 km/h)\n\n";

    std::cout << "Route:\n";
    std::cout << std::string(50, '-') << "\n";

    for (size_t i = 0; i < result.path.size(); i++) {
        int nodeId = result.path[i];
        std::cout << "[" << nodeId << "] " << graph.getNodeName(nodeId);

        if (i < result.path.size() - 1) {
            std::cout << " ---> ";
        }
        std::cout << "\n";
    }

    std::cout << std::string(50, '-') << "\n";
}

#include "Algorithms.h"
#include "utils/colors.h"
#include "utils/keyUtils.h"
#include <iostream>
#include <iomanip>
#include <stack>
#include <thread>
#include <chrono>

TraversalResult dfs(const Graph& graph, int startId, bool verbose) {
    TraversalResult result;

    if (!graph.nodeExists(startId)) {
        if (verbose) std::cout << "Error: Start node " << startId << " not found!\n";
        return result;
    }

    bool visited[MAX_NODES] = {false};
    std::stack<int> stack;
    const Node* nodes = graph.getNodes();

    stack.push(startId);

    if (verbose) {
        system("cls");
        std::cout << BOLD << "Depth-first search (DFS)" << RESET << "\n";
        std::cout << "Starting from: [" << startId << "] " << graph.getNodeName(startId) << "\n\n";
        std::cout << "DFS explores as far as possible along each branch.\n";
        if (sleepWithEscCheck(500)) {
            std::cout << "\nOperation aborted by user.\n";
            return result;
        }
    }

    int step = 0;

    while (!stack.empty()) {
        int currentId = stack.top();
        stack.pop();

        // Find current node index
        int currentIndex = -1;
        for (int i = 0; i < MAX_NODES; i++) {
            if (nodes[i].active && nodes[i].id == currentId) {
                currentIndex = i;
                break;
            }
        }

        if (currentIndex == -1 || visited[currentIndex]) continue;

        visited[currentIndex] = true;
        result.visitedNodes.push_back(currentId);
        step++;

        if (verbose) {
            system("cls");
            std::cout << BOLD << "DFS - Step " << step << RESET << "\n";
            std::cout << CYAN << "Visiting: [" << currentId << "] "
                      << graph.getNodeName(currentId) << RESET << "\n\n";

            std::cout << "Visited Order: ";
            for (size_t i = 0; i < result.visitedNodes.size(); i++) {
                std::cout << "[" << result.visitedNodes[i] << "]";
                if (i < result.visitedNodes.size() - 1) std::cout << " -> ";
            }
            std::cout << "\n\n";

            std::cout << "Node Status:\n";
            std::cout << std::setw(6) << "ID" << " | "
                      << std::setw(20) << "Name" << " | "
                      << std::setw(12) << "Status" << "\n";
            std::cout << std::string(45, '-') << "\n";

            for (int i = 0; i < MAX_NODES; i++) {
                if (nodes[i].active) {
                    std::cout << std::setw(6) << nodes[i].id << " | "
                              << std::setw(20) << nodes[i].name << " | ";

                    if (nodes[i].id == currentId) {
                        std::cout << CYAN << std::setw(12) << "Current" << RESET;
                    } else if (visited[i]) {
                        std::cout << std::setw(12) << "Visited";
                    } else {
                        std::cout << std::setw(12) << "Unvisited";
                    }
                    std::cout << "\n";
                }
            }

            std::cout << "\nStack size: " << stack.size() << "\n";
            if (sleepWithEscCheck(500)) {
                std::cout << "\nOperation aborted by user.\n";
                result.completed = false;
                return result;
            }
        }

        // Add neighbors to stack (in reverse order for consistent traversal)
        // First, collect all neighbors
        std::vector<int> neighbors;
        Edge* edge = nodes[currentIndex].adjacencyList;
        while (edge != nullptr) {
            neighbors.push_back(edge->destination);
            edge = edge->next;
        }

        // Push in reverse order so first neighbor is processed first
        for (int i = neighbors.size() - 1; i >= 0; i--) {
            int neighborId = neighbors[i];

            // Find neighbor index
            int neighborIndex = -1;
            for (int j = 0; j < MAX_NODES; j++) {
                if (nodes[j].active && nodes[j].id == neighborId) {
                    neighborIndex = j;
                    break;
                }
            }

            if (neighborIndex != -1 && !visited[neighborIndex]) {
                stack.push(neighborId);
            }
        }
    }

    result.completed = true;
    return result;
}

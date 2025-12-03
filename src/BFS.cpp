#include "Algorithms.h"
#include "utils/colors.h"
#include <iostream>
#include <iomanip>
#include <queue>
#include <thread>
#include <chrono>

TraversalResult bfs(const Graph& graph, int startId, bool verbose) {
    TraversalResult result;

    if (!graph.nodeExists(startId)) {
        if (verbose) std::cout << "Error: Start node " << startId << " not found!\n";
        return result;
    }

    bool visited[MAX_NODES] = {false};
    std::queue<int> queue;
    const Node* nodes = graph.getNodes();

    queue.push(startId);
    visited[startId] = true;

    if (verbose) {
        system("cls");
        std::cout << BOLD << "Breadth-first search (BFS)" << RESET << "\n";
        std::cout << "Starting from: [" << startId << "] " << graph.getNodeName(startId) << "\n\n";
        std::cout << "BFS explores nodes level by level.\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    int step = 0;

    while (!queue.empty()) {
        int currentId = queue.front();
        queue.pop();

        result.visitedNodes.push_back(currentId);
        step++;

        if (verbose) {
            system("cls");
            std::cout << BOLD << "BFS - Step " << step << RESET << "\n";
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

            std::cout << "\nQueue size: " << queue.size() << "\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        // Find current node index
        int currentIndex = -1;
        for (int i = 0; i < MAX_NODES; i++) {
            if (nodes[i].active && nodes[i].id == currentId) {
                currentIndex = i;
                break;
            }
        }

        if (currentIndex == -1) continue;

        // Add neighbors to queue
        Edge* edge = nodes[currentIndex].adjacencyList;
        while (edge != nullptr) {
            int neighborId = edge->destination;

            // Find neighbor index
            int neighborIndex = -1;
            for (int i = 0; i < MAX_NODES; i++) {
                if (nodes[i].active && nodes[i].id == neighborId) {
                    neighborIndex = i;
                    break;
                }
            }

            if (neighborIndex != -1 && !visited[neighborIndex]) {
                visited[neighborIndex] = true;
                queue.push(neighborId);
            }

            edge = edge->next;
        }
    }

    result.completed = true;
    return result;
}

void displayTraversal(const Graph& graph, const TraversalResult& result, const std::string& algorithmName) {
    system("cls");
    std::cout << BOLD << algorithmName << " result" << RESET << "\n";

    if (!result.completed) {
        std::cout << "Traversal did not complete successfully.\n";
        return;
    }

    std::cout << CYAN << "Traversal completed!\n" << RESET;
    std::cout << "Nodes visited: " << result.visitedNodes.size() << "\n\n";

    std::cout << "Visit Order:\n";
    std::cout << std::string(50, '-') << "\n";

    for (size_t i = 0; i < result.visitedNodes.size(); i++) {
        int nodeId = result.visitedNodes[i];
        std::cout << std::setw(3) << (i + 1) << ". [" << nodeId << "] "
                  << graph.getNodeName(nodeId) << "\n";
    }

    std::cout << std::string(50, '-') << "\n";
}

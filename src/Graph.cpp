#include "Graph.h"
#include <iomanip>
#include <sstream>

Graph::Graph() : nodeCount(0), nextId(0), currentFile("") {
    for (int i = 0; i < MAX_NODES; i++) {
        nodes[i].active = false;
    }
}

Graph::~Graph() {
    for (int i = 0; i < MAX_NODES; i++) {
        if (nodes[i].active) {
            clearEdges(i);
        }
    }
}

int Graph::findNodeIndex(int id) {
    for (int i = 0; i < MAX_NODES; i++) {
        if (nodes[i].active && nodes[i].id == id) {
            return i;
        }
    }
    return -1;
}

void Graph::clearEdges(int nodeIndex) {
    Edge* current = nodes[nodeIndex].adjacencyList;
    while (current != nullptr) {
        Edge* temp = current;
        current = current->next;
        delete temp;
    }
    nodes[nodeIndex].adjacencyList = nullptr;
}

void Graph::updateNextId() {
    int maxId = -1;
    for (int i = 0; i < MAX_NODES; i++) {
        if (nodes[i].active && nodes[i].id > maxId) {
            maxId = nodes[i].id;
        }
    }
    nextId = maxId + 1;
}

void Graph::autoSave() {
    if (!currentFile.empty()) {
        std::ofstream file(currentFile);
        if (!file.is_open()) {
            return;
        }

        file << "# NODES\n";
        for (int i = 0; i < MAX_NODES; i++) {
            if (nodes[i].active) {
                file << "N;" << nodes[i].id << ";" << nodes[i].name << "\n";
            }
        }

        file << "\n# EDGES\n";
        for (int i = 0; i < MAX_NODES; i++) {
            if (nodes[i].active) {
                Edge* current = nodes[i].adjacencyList;
                while (current != nullptr) {
                    file << "E;" << nodes[i].id << ";" << current->destination
                         << ";" << current->weight << "\n";
                    current = current->next;
                }
            }
        }

        file.close();
    }
}

bool Graph::addNode(int id, const std::string& name) {
    if (nodeCount >= MAX_NODES) {
        std::cout << "Error: Maximum node capacity reached!\n";
        return false;
    }

    if (findNodeIndex(id) != -1) {
        std::cout << "Error: Node with ID " << id << " already exists!\n";
        return false;
    }

    // Validate name is not empty
    if (name.empty()) {
        std::cout << "Error: Node name cannot be empty!\n";
        return false;
    }

    for (int i = 0; i < MAX_NODES; i++) {
        if (!nodes[i].active) {
            nodes[i].id = id;
            nodes[i].name = name;
            nodes[i].adjacencyList = nullptr;
            nodes[i].active = true;
            nodeCount++;

            // Update nextId if this ID is >= current nextId
            if (id >= nextId) {
                nextId = id + 1;
            }

            std::cout << "Node added successfully: [" << id << "] " << name << "\n";
            return true;
        }
    }

    return false;
}

bool Graph::addNodeAuto(const std::string& name) {
    if (nodeCount >= MAX_NODES) {
        std::cout << "Error: Maximum node capacity reached!\n";
        return false;
    }

    // Validate name is not empty
    if (name.empty()) {
        std::cout << "Error: Node name cannot be empty!\n";
        return false;
    }

    int id = nextId;

    for (int i = 0; i < MAX_NODES; i++) {
        if (!nodes[i].active) {
            nodes[i].id = id;
            nodes[i].name = name;
            nodes[i].adjacencyList = nullptr;
            nodes[i].active = true;
            nodeCount++;
            nextId++;

            std::cout << "Node added successfully: [" << id << "] " << name << "\n";
            autoSave();
            return true;
        }
    }

    return false;
}

bool Graph::removeNode(int id) {
    int index = findNodeIndex(id);
    if (index == -1) {
        std::cout << "Error: Node with ID " << id << " not found!\n";
        return false;
    }

    // Remove all edges pointing to this node
    for (int i = 0; i < MAX_NODES; i++) {
        if (nodes[i].active && i != index) {
            Edge* current = nodes[i].adjacencyList;
            Edge* prev = nullptr;

            while (current != nullptr) {
                if (current->destination == id) {
                    if (prev == nullptr) {
                        nodes[i].adjacencyList = current->next;
                    } else {
                        prev->next = current->next;
                    }
                    Edge* temp = current;
                    current = current->next;
                    delete temp;
                } else {
                    prev = current;
                    current = current->next;
                }
            }
        }
    }

    // Remove edges from this node
    clearEdges(index);

    nodes[index].active = false;
    nodeCount--;
    std::cout << "Node removed successfully: [" << id << "]\n";
    autoSave();
    return true;
}

bool Graph::addEdge(int sourceId, int destId, double weight) {
    int sourceIndex = findNodeIndex(sourceId);
    int destIndex = findNodeIndex(destId);

    if (sourceIndex == -1) {
        std::cout << "Error: Source node " << sourceId << " not found!\n";
        return false;
    }

    if (destIndex == -1) {
        std::cout << "Error: Destination node " << destId << " not found!\n";
        return false;
    }

    if (weight <= 0) {
        std::cout << "Error: Weight must be positive!\n";
        return false;
    }

    // Check if edge already exists
    Edge* current = nodes[sourceIndex].adjacencyList;
    while (current != nullptr) {
        if (current->destination == destId) {
            std::cout << "Error: Edge already exists! Use update instead.\n";
            return false;
        }
        current = current->next;
    }

    // Add new edge at the beginning
    Edge* newEdge = new Edge(destId, weight);
    newEdge->next = nodes[sourceIndex].adjacencyList;
    nodes[sourceIndex].adjacencyList = newEdge;

    std::cout << "Edge added: " << nodes[sourceIndex].name << " -> "
              << nodes[destIndex].name << " (weight: " << weight << ")\n";
    autoSave();
    return true;
}

bool Graph::removeEdge(int sourceId, int destId) {
    int sourceIndex = findNodeIndex(sourceId);

    if (sourceIndex == -1) {
        std::cout << "Error: Source node " << sourceId << " not found!\n";
        return false;
    }

    Edge* current = nodes[sourceIndex].adjacencyList;
    Edge* prev = nullptr;

    while (current != nullptr) {
        if (current->destination == destId) {
            if (prev == nullptr) {
                nodes[sourceIndex].adjacencyList = current->next;
            } else {
                prev->next = current->next;
            }
            delete current;
            std::cout << "Edge removed successfully!\n";
            autoSave();
            return true;
        }
        prev = current;
        current = current->next;
    }

    std::cout << "Error: Edge not found!\n";
    return false;
}

void Graph::clearNetwork() {
    for (int i = 0; i < MAX_NODES; i++) {
        if (nodes[i].active) {
            clearEdges(i);
            nodes[i].active = false;
        }
    }
    nodeCount = 0;
    nextId = 0;
    currentFile = "";
}

bool Graph::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error: Could not open file '" << filename << "'\n";
        return false;
    }

    // Clear existing network before loading
    clearNetwork();

    std::string line;
    int nodesLoaded = 0;
    int edgesLoaded = 0;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        std::string type;
        std::getline(ss, type, ';');

        if (type == "N") {
            std::string idStr, name;
            std::getline(ss, idStr, ';');
            std::getline(ss, name);

            int id = std::stoi(idStr);
            if (addNode(id, name)) {
                nodesLoaded++;
            }
        } else if (type == "E") {
            std::string srcStr, destStr, weightStr;
            std::getline(ss, srcStr, ';');
            std::getline(ss, destStr, ';');
            std::getline(ss, weightStr);

            int src = std::stoi(srcStr);
            int dest = std::stoi(destStr);
            double weight = std::stod(weightStr);

            if (addEdge(src, dest, weight)) {
                edgesLoaded++;
            }
        }
    }

    file.close();

    // Update nextId based on loaded nodes
    updateNextId();

    // Set current file for auto-save
    currentFile = filename;

    std::cout << "\nNetwork loaded successfully!\n";
    std::cout << "Nodes: " << nodesLoaded << " | Edges: " << edgesLoaded << "\n";
    std::cout << "Auto-save enabled to: " << currentFile << "\n";
    std::cout << "Next available ID: " << nextId << "\n";
    return true;
}

bool Graph::saveToFile(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error: Could not create file '" << filename << "'\n";
        return false;
    }

    file << "# NODES\n";
    for (int i = 0; i < MAX_NODES; i++) {
        if (nodes[i].active) {
            file << "N;" << nodes[i].id << ";" << nodes[i].name << "\n";
        }
    }

    file << "\n# EDGES\n";
    for (int i = 0; i < MAX_NODES; i++) {
        if (nodes[i].active) {
            Edge* current = nodes[i].adjacencyList;
            while (current != nullptr) {
                file << "E;" << nodes[i].id << ";" << current->destination
                     << ";" << current->weight << "\n";
                current = current->next;
            }
        }
    }

    file.close();
    std::cout << "Network saved successfully to '" << filename << "'\n";
    return true;
}

void Graph::showAdjacencyList() {
    if (nodeCount == 0) {
        std::cout << "Network is empty!\n";
        return;
    }

    std::cout << "\n===== ADJACENCY LIST =====\n\n";
    for (int i = 0; i < MAX_NODES; i++) {
        if (nodes[i].active) {
            std::cout << "[" << nodes[i].id << "] " << nodes[i].name << " -> ";

            Edge* current = nodes[i].adjacencyList;
            if (current == nullptr) {
                std::cout << "(no connections)";
            } else {
                while (current != nullptr) {
                    int destIndex = findNodeIndex(current->destination);
                    std::cout << nodes[destIndex].name << "(" << current->weight << ")";
                    if (current->next != nullptr) {
                        std::cout << ", ";
                    }
                    current = current->next;
                }
            }
            std::cout << "\n";
        }
    }
    std::cout << "\n";
}

void Graph::showAdjacencyMatrix() {
    if (nodeCount == 0) {
        std::cout << "Network is empty!\n";
        return;
    }

    std::cout << "\n===== ADJACENCY MATRIX =====\n\n";

    // Header
    std::cout << std::setw(6) << " ";
    for (int i = 0; i < MAX_NODES; i++) {
        if (nodes[i].active) {
            std::cout << std::setw(6) << nodes[i].id;
        }
    }
    std::cout << "\n";

    // Matrix
    for (int i = 0; i < MAX_NODES; i++) {
        if (nodes[i].active) {
            std::cout << std::setw(6) << nodes[i].id;

            for (int j = 0; j < MAX_NODES; j++) {
                if (nodes[j].active) {
                    double weight = 0.0;
                    Edge* current = nodes[i].adjacencyList;
                    while (current != nullptr) {
                        if (current->destination == nodes[j].id) {
                            weight = current->weight;
                            break;
                        }
                        current = current->next;
                    }

                    if (weight > 0) {
                        std::cout << std::setw(6) << std::fixed << std::setprecision(1) << weight;
                    } else {
                        std::cout << std::setw(6) << "-";
                    }
                }
            }
            std::cout << "\n";
        }
    }
    std::cout << "\n";
}

bool Graph::nodeExists(int id) {
    return findNodeIndex(id) != -1;
}

std::string Graph::getNodeName(int id) {
    int index = findNodeIndex(id);
    if (index != -1) {
        return nodes[index].name;
    }
    return "";
}

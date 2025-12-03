#include <iostream>
#include <map>
#include <string>
#include <functional>
#include <memory>
#include <fstream>
#include <iomanip>
#include <conio.h>
#include "utils/colors.h"
#include "utils/fileUtils.h"
#include "utils/inputUtils.h"
#include "utils/tableUtils.h"
#include "Graph.h"
#include "HashTable.h"
#include "Algorithms.h"

// Global instances
Graph cityNetwork;
HashTable vehicleRegistry;

class Menu;

struct MenuItem {
    std::string label;
    std::function<void()> action;
    std::shared_ptr<Menu> submenu;

    MenuItem() : label(""), action(nullptr), submenu(nullptr) {}

    MenuItem(const std::string& lbl, std::function<void()> act = nullptr)
        : label(lbl), action(act), submenu(nullptr) {}

    MenuItem(const std::string& lbl, std::shared_ptr<Menu> sub)
        : label(lbl), action(nullptr), submenu(sub) {}
};

class Menu {
private:
    std::string title;
    std::map<int, MenuItem> items;
    int selectedIndex = 0;

    void clearScreen() {
        system("cls");
    }

    void display() {
        clearScreen();
        std::cout << BOLD << title << RESET << "\n";

        int idx = 0;
        for (const auto& [key, item] : items) {
            if (idx == selectedIndex) {
                std::cout << CYAN << "> ";
            } else {
                std::cout << "  ";
            }
            std::cout << item.label << RESET << "\n";
            idx++;
        }
        std::cout << "[W/S or Arrow Keys] Navigate | [Enter] Select | [ESC] Back\n\n";
    }

    MenuItem& getSelectedItem() {
        int idx = 0;
        for (auto& [key, item] : items) {
            if (idx == selectedIndex) {
                return item;
            }
            idx++;
        }
        return items.begin()->second;
    }

public:
    Menu(const std::string& t) : title(t) {}

    void addItem(int key, const MenuItem& item) {
        items[key] = item;
    }

    bool run() {
        selectedIndex = 0;

        while (true) {
            display();

            int ch = _getch();

            if (ch == 224 || ch == 0) {
                ch = _getch();
                if (ch == 72 || ch == 'H') { // Up arrow
                    selectedIndex = (selectedIndex - 1 + items.size()) % items.size();
                } else if (ch == 80 || ch == 'P') { // Down arrow
                    selectedIndex = (selectedIndex + 1) % items.size();
                }
            } else if (ch == 'w' || ch == 'W') {
                selectedIndex = (selectedIndex - 1 + items.size()) % items.size();
            } else if (ch == 's' || ch == 'S') {
                selectedIndex = (selectedIndex + 1) % items.size();
            } else if (ch == 13) { // Enter
                MenuItem& selected = getSelectedItem();

                if (selected.submenu) {
                    selected.submenu->run();
                } else if (selected.action) {
                    clearScreen();
                    selected.action();
                    std::cout << "\nPress any key to continue...";
                    _getch();
                }
            } else if (ch == 27) { // ESC
                return false;
            }
        }

        return true;
    }
};

// ===== ROAD NETWORK MANAGEMENT =====
void loadNetwork() {
    std::cout << BOLD << "Load network" << RESET << "\n";
    std::string filename = getValidString("Enter filename (without extension): ");

    std::string sanitized = sanitizeFilename(filename);
    if (sanitized.empty()) {
        std::cout << "Error: Invalid filename!\n";
        return;
    }

    std::string fullPath = getDataPath(sanitized);
    std::cout << "Loading from: " << fullPath << "\n\n";
    cityNetwork.loadFromFile(fullPath);
}

void saveNetwork() {
    std::cout << BOLD << "Save network" << RESET << "\n";

    if (cityNetwork.getNodeCount() == 0) {
        std::cout << "Error: Network is empty! Nothing to save.\n";
        return;
    }

    std::string filename = getValidString("Enter filename (without extension): ");

    std::string sanitized = sanitizeFilename(filename);
    if (sanitized.empty()) {
        std::cout << "Error: Invalid filename!\n";
        return;
    }

    std::string fullPath = getDataPath(sanitized);
    std::cout << "Saving to: " << fullPath << "\n\n";
    cityNetwork.saveToFile(fullPath);
}

void addNode() {
    std::cout << BOLD << "Add node" << RESET << "\n";
    std::string name = getValidString("Enter node name: ");
    cityNetwork.addNodeAuto(name);
}

void addEdge() {
    std::cout << BOLD << "Add edge" << RESET << "\n";

    if (cityNetwork.getNodeCount() == 0) {
        std::cout << "Error: No nodes in network! Add nodes first.\n";
        return;
    }

    // Create interactive table for source node selection
    InteractivePaginatedTable sourceTable("SELECT SOURCE NODE", "Name", "Connections");

    const Node* nodes = cityNetwork.getNodes();
    for (int i = 0; i < cityNetwork.getMaxNodes(); i++) {
        if (nodes[i].active) {
            // Count connections
            int connCount = 0;
            Edge* edge = nodes[i].adjacencyList;
            while (edge != nullptr) {
                connCount++;
                edge = edge->next;
            }
            sourceTable.addRow(nodes[i].id, nodes[i].name, std::to_string(connCount) + " edges");
        }
    }

    int sourceId = sourceTable.run();
    if (sourceId == -1) {
        std::cout << "Operation cancelled.\n";
        return;
    }

    // Create interactive table for destination node selection
    InteractivePaginatedTable destTable("SELECT DESTINATION NODE", "Name", "Connections");

    for (int i = 0; i < cityNetwork.getMaxNodes(); i++) {
        if (nodes[i].active) {
            int connCount = 0;
            Edge* edge = nodes[i].adjacencyList;
            while (edge != nullptr) {
                connCount++;
                edge = edge->next;
            }
            destTable.addRow(nodes[i].id, nodes[i].name, std::to_string(connCount) + " edges");
        }
    }

    int destId = destTable.run();
    if (destId == -1) {
        std::cout << "Operation cancelled.\n";
        return;
    }

    // Check for self-loop
    if (sourceId == destId) {
        system("cls");
        std::cout << BOLD << "Add edge" << RESET << "\n";
        std::cout << "Error: Cannot create an edge from a node to itself!\n";
        std::cout << "Self-loops are not allowed in road networks.\n";
        return;
    }

    // Direction selector
    int directionChoice = 0; // 0=bidirectional, 1=directional, 2=counter-directional
    std::string sourceName = cityNetwork.getNodeName(sourceId);
    std::string destName = cityNetwork.getNodeName(destId);

    while (true) {
        system("cls");
        std::cout << BOLD << "Confirm edge direction" << RESET << "\n";

        std::string sourceRole, destRole;

        // Update roles based on direction
        if (directionChoice == 0) { // Bidirectional
            sourceRole = "Bidirectional";
            destRole = "Bidirectional";
        } else if (directionChoice == 1) { // Directional
            sourceRole = "Source";
            destRole = "Destination";
        } else { // Counter-directional
            sourceRole = "Destination";
            destRole = "Source";
        }

        // Display table with roles
        std::cout << CYAN;
        std::cout << std::setw(6) << "ID" << " | "
                  << std::setw(20) << "Name" << " | "
                  << std::setw(15) << "Role" << "\n";
        std::cout << std::string(50, '-') << "\n";
        std::cout << std::setw(6) << sourceId << " | "
                  << std::setw(20) << sourceName << " | "
                  << std::setw(15) << sourceRole << "\n";
        std::cout << std::setw(6) << destId << " | "
                  << std::setw(20) << destName << " | "
                  << std::setw(15) << destRole << "\n";
        std::cout << RESET << "\n";

        std::cout << "Is this configuration correct?\n";
        std::cout << "[A/D or Left/Right] Change direction | [Enter] Confirm | [ESC] Cancel\n";

        int ch = _getch();

        if (ch == 224 || ch == 0) {
            ch = _getch();
            if (ch == 75 || ch == 'K') { // Left arrow
                directionChoice = (directionChoice - 1 + 3) % 3;
            } else if (ch == 77 || ch == 'M') { // Right arrow
                directionChoice = (directionChoice + 1) % 3;
            }
        } else if (ch == 'a' || ch == 'A') {
            directionChoice = (directionChoice - 1 + 3) % 3;
        } else if (ch == 'd' || ch == 'D') {
            directionChoice = (directionChoice + 1) % 3;
        } else if (ch == 13) { // Enter
            break;
        } else if (ch == 27) { // ESC
            std::cout << "Operation cancelled.\n";
            return;
        }
    }

    // Keep the table visible and ask for weight
    system("cls");
    std::cout << BOLD << "Add edge" << RESET << "\n";

    std::string sourceRole, destRole;
    if (directionChoice == 0) {
        sourceRole = "Bidirectional";
        destRole = "Bidirectional";
    } else if (directionChoice == 1) {
        sourceRole = "Source";
        destRole = "Destination";
    } else {
        sourceRole = "Destination";
        destRole = "Source";
    }

    // Display final table with weight column
    std::cout << CYAN;
    std::cout << std::setw(6) << "ID" << " | "
              << std::setw(20) << "Name" << " | "
              << std::setw(15) << "Role" << " | "
              << std::setw(10) << "Weight" << "\n";
    std::cout << std::string(60, '-') << "\n";
    std::cout << std::setw(6) << sourceId << " | "
              << std::setw(20) << sourceName << " | "
              << std::setw(15) << sourceRole << " | "
              << std::setw(10) << "?" << "\n";
    std::cout << std::setw(6) << destId << " | "
              << std::setw(20) << destName << " | "
              << std::setw(15) << destRole << " | "
              << std::setw(10) << "?" << "\n";
    std::cout << RESET << "\n";

    double weight = getValidDouble("Enter weight (distance/time): ");

    // Add edges based on direction choice (no extra screen, addEdge prints confirmation)
    if (directionChoice == 0) { // Bidirectional
        cityNetwork.addEdge(sourceId, destId, weight);
        cityNetwork.addEdge(destId, sourceId, weight);
    } else if (directionChoice == 1) { // Directional
        cityNetwork.addEdge(sourceId, destId, weight);
    } else if (directionChoice == 2) { // Counter-directional
        cityNetwork.addEdge(destId, sourceId, weight);
    }
}

void removeNode() {
    std::cout << BOLD << "Remove node" << RESET << "\n";
    int id = getValidInt("Enter node ID to remove: ");
    cityNetwork.removeNode(id);
}

void removeEdge() {
    std::cout << BOLD << "Remove edge" << RESET << "\n";

    if (cityNetwork.getNodeCount() == 0) {
        std::cout << "Error: No nodes in network!\n";
        return;
    }

    // Create interactive table for source node selection
    InteractivePaginatedTable sourceTable("SELECT SOURCE NODE", "Name", "Connections");

    const Node* nodes = cityNetwork.getNodes();
    for (int i = 0; i < cityNetwork.getMaxNodes(); i++) {
        if (nodes[i].active) {
            int connCount = 0;
            Edge* edge = nodes[i].adjacencyList;
            while (edge != nullptr) {
                connCount++;
                edge = edge->next;
            }
            sourceTable.addRow(nodes[i].id, nodes[i].name, std::to_string(connCount) + " edges");
        }
    }

    int sourceId = sourceTable.run();
    if (sourceId == -1) {
        std::cout << "Operation cancelled.\n";
        return;
    }

    // Create interactive table for destination node selection
    InteractivePaginatedTable destTable("SELECT DESTINATION NODE", "Name", "Connections");

    for (int i = 0; i < cityNetwork.getMaxNodes(); i++) {
        if (nodes[i].active) {
            int connCount = 0;
            Edge* edge = nodes[i].adjacencyList;
            while (edge != nullptr) {
                connCount++;
                edge = edge->next;
            }
            destTable.addRow(nodes[i].id, nodes[i].name, std::to_string(connCount) + " edges");
        }
    }

    int destId = destTable.run();
    if (destId == -1) {
        std::cout << "Operation cancelled.\n";
        return;
    }

    system("cls");
    std::cout << BOLD << "Remove edge" << RESET << "\n";

    // Display selected nodes in cyan table
    std::cout << CYAN;
    std::cout << std::setw(6) << "ID" << " | "
              << std::setw(20) << "Name" << " | "
              << std::setw(15) << "Role" << "\n";
    std::cout << std::string(50, '-') << "\n";
    std::cout << std::setw(6) << sourceId << " | "
              << std::setw(20) << cityNetwork.getNodeName(sourceId) << " | "
              << std::setw(15) << "Source" << "\n";
    std::cout << std::setw(6) << destId << " | "
              << std::setw(20) << cityNetwork.getNodeName(destId) << " | "
              << std::setw(15) << "Destination" << "\n";
    std::cout << RESET << "\n";

    std::cout << "Confirm removal? (y/n): ";
    char confirm;
    std::cin >> confirm;
    std::cin.ignore();

    if (confirm == 'y' || confirm == 'Y') {
        cityNetwork.removeEdge(sourceId, destId);
    } else {
        std::cout << "Operation cancelled.\n";
    }
}

void generateSeedFile() {
    std::cout << BOLD << "Generate seed file" << RESET << "\n";
    std::string filename = getValidString("Enter seed filename (without extension): ");

    std::string sanitized = sanitizeFilename(filename);
    if (sanitized.empty()) {
        std::cout << "Error: Invalid filename!\n";
        return;
    }

    std::string fullPath = getSeedPath(sanitized);
    std::cout << "Generating seed file: " << fullPath << "\n\n";

    std::ofstream file(fullPath);
    if (!file.is_open()) {
        std::cout << "Error: Could not create seed file!\n";
        return;
    }

    file << "# SAMPLE NETWORK - Aguascalientes City\n";
    file << "# NODES\n";
    file << "N;0;UAA\n";
    file << "N;1;Plaza_Patria\n";
    file << "N;2;Centro_Historico\n";
    file << "N;3;Glorieta_Norte\n";
    file << "N;4;Estadio_Victoria\n";
    file << "N;5;Expo_Plaza\n";
    file << "\n# EDGES (source;destination;weight)\n";
    file << "E;0;1;3.5\n";
    file << "E;1;2;2.8\n";
    file << "E;0;2;6.2\n";
    file << "E;2;3;4.1\n";
    file << "E;1;3;5.0\n";
    file << "E;3;4;3.2\n";
    file << "E;4;0;7.5\n";
    file << "E;1;5;2.1\n";
    file << "E;5;3;3.9\n";

    file.close();
    std::cout << "Seed file generated successfully!\n";
    std::cout << "You can now load it using 'Load Network' option.\n";
}

void generateVehicleSeedFile() {
    std::cout << BOLD << "Generate vehicle seed file" << RESET << "\n";

    std::string fullPath = getSeedPath("vehicles");
    std::cout << "Generating vehicle seed file: " << fullPath << "\n\n";

    std::ofstream file(fullPath);
    if (!file.is_open()) {
        std::cout << "Error: Could not create seed file!\n";
        return;
    }

    file << "# SAMPLE VEHICLES\n";
    file << "# Format: V;id;plate;type;currentNodeId;destinationNodeId\n";
    file << "V;0;ABC-123-X;Sedan;0;-1\n";
    file << "V;1;XYZ-999-Y;Truck;1;-1\n";
    file << "V;2;DEF-456-Z;Compact;2;-1\n";
    file << "V;3;GHI-789-A;Sedan;3;-1\n";
    file << "V;4;JKL-012-B;Truck;4;-1\n";

    file.close();
    std::cout << "Vehicle seed file generated successfully!\n";
    std::cout << "You can now load it using 'Load Vehicles' option.\n";
}

// ===== NETWORK VISUALIZATION =====
void showAdjacencyList() {
    cityNetwork.showAdjacencyList();
}

void showAdjacencyMatrix() {
    cityNetwork.showAdjacencyMatrix();
}

// ===== QUERIES AND ALGORITHMS =====
void findShortestPath() {
    std::cout << BOLD << "Find shortest path" << RESET << "\n";

    if (cityNetwork.getNodeCount() < 2) {
        std::cout << "Error: Need at least 2 nodes in the network!\n";
        return;
    }

    // Select source node
    InteractivePaginatedTable sourceTable("SELECT SOURCE NODE", "Name", "Connections");
    const Node* nodes = cityNetwork.getNodes();

    for (int i = 0; i < cityNetwork.getMaxNodes(); i++) {
        if (nodes[i].active) {
            int connCount = 0;
            Edge* edge = nodes[i].adjacencyList;
            while (edge != nullptr) {
                connCount++;
                edge = edge->next;
            }
            sourceTable.addRow(nodes[i].id, nodes[i].name, std::to_string(connCount) + " edges");
        }
    }

    int sourceId = sourceTable.run();
    if (sourceId == -1) {
        std::cout << "Operation cancelled.\n";
        return;
    }

    // Select destination node
    InteractivePaginatedTable destTable("SELECT DESTINATION NODE", "Name", "Connections");

    for (int i = 0; i < cityNetwork.getMaxNodes(); i++) {
        if (nodes[i].active) {
            int connCount = 0;
            Edge* edge = nodes[i].adjacencyList;
            while (edge != nullptr) {
                connCount++;
                edge = edge->next;
            }
            destTable.addRow(nodes[i].id, nodes[i].name, std::to_string(connCount) + " edges");
        }
    }

    int destId = destTable.run();
    if (destId == -1) {
        std::cout << "Operation cancelled.\n";
        return;
    }

    if (sourceId == destId) {
        system("cls");
        std::cout << "Error: Source and destination cannot be the same!\n";
        return;
    }

    // Run Dijkstra with visualization
    PathResult result = dijkstra(cityNetwork, sourceId, destId, true);
    displayPath(cityNetwork, result);
}

void breadthFirstSearch() {
    std::cout << BOLD << "Breadth-first search" << RESET << "\n";

    if (cityNetwork.getNodeCount() == 0) {
        std::cout << "Error: Network is empty!\n";
        return;
    }

    // Select starting node
    InteractivePaginatedTable startTable("SELECT START NODE", "Name", "Connections");
    const Node* nodes = cityNetwork.getNodes();

    for (int i = 0; i < cityNetwork.getMaxNodes(); i++) {
        if (nodes[i].active) {
            int connCount = 0;
            Edge* edge = nodes[i].adjacencyList;
            while (edge != nullptr) {
                connCount++;
                edge = edge->next;
            }
            startTable.addRow(nodes[i].id, nodes[i].name, std::to_string(connCount) + " edges");
        }
    }

    int startId = startTable.run();
    if (startId == -1) {
        std::cout << "Operation cancelled.\n";
        return;
    }

    // Run BFS with visualization
    TraversalResult result = bfs(cityNetwork, startId, true);
    displayTraversal(cityNetwork, result, "BREADTH-FIRST SEARCH");
}

void depthFirstSearch() {
    std::cout << BOLD << "Depth-first search" << RESET << "\n";

    if (cityNetwork.getNodeCount() == 0) {
        std::cout << "Error: Network is empty!\n";
        return;
    }

    // Select starting node
    InteractivePaginatedTable startTable("SELECT START NODE", "Name", "Connections");
    const Node* nodes = cityNetwork.getNodes();

    for (int i = 0; i < cityNetwork.getMaxNodes(); i++) {
        if (nodes[i].active) {
            int connCount = 0;
            Edge* edge = nodes[i].adjacencyList;
            while (edge != nullptr) {
                connCount++;
                edge = edge->next;
            }
            startTable.addRow(nodes[i].id, nodes[i].name, std::to_string(connCount) + " edges");
        }
    }

    int startId = startTable.run();
    if (startId == -1) {
        std::cout << "Operation cancelled.\n";
        return;
    }

    // Run DFS with visualization
    TraversalResult result = dfs(cityNetwork, startId, true);
    displayTraversal(cityNetwork, result, "DEPTH-FIRST SEARCH");
}

// ===== VEHICLE MANAGEMENT =====
void loadVehicles() {
    std::cout << BOLD << "Load vehicles" << RESET << "\n";
    std::string filename = getValidString("Enter filename (without extension): ");

    std::string sanitized = sanitizeFilename(filename);
    if (sanitized.empty()) {
        std::cout << "Error: Invalid filename!\n";
        return;
    }

    std::string fullPath = getDataPath(sanitized);
    std::cout << "Loading from: " << fullPath << "\n\n";
    vehicleRegistry.loadFromFile(fullPath);
}

void saveVehicles() {
    std::cout << BOLD << "Save vehicles" << RESET << "\n";

    if (vehicleRegistry.getVehicleCount() == 0) {
        std::cout << "Error: No vehicles to save!\n";
        return;
    }

    std::string filename = getValidString("Enter filename (without extension): ");

    std::string sanitized = sanitizeFilename(filename);
    if (sanitized.empty()) {
        std::cout << "Error: Invalid filename!\n";
        return;
    }

    std::string fullPath = getDataPath(sanitized);
    std::cout << "Saving to: " << fullPath << "\n\n";
    vehicleRegistry.saveToFile(fullPath);
}

void addVehicle() {
    std::cout << BOLD << "Add vehicle" << RESET << "\n";

    if (cityNetwork.getNodeCount() == 0) {
        std::cout << "Error: No nodes in network! Add nodes first.\n";
        return;
    }

    std::string plate = getValidString("Enter vehicle plate: ");

    // Vehicle type selector with arrows
    int typeChoice = 0;
    std::string types[] = {"Sedan", "Compact", "Truck"};

    while (true) {
        system("cls");
        std::cout << BOLD << "Select vehicle type" << RESET << "\n";
        std::cout << "Plate: " << plate << "\n\n";

        for (int i = 0; i < 3; i++) {
            if (i == typeChoice) {
                std::cout << CYAN << "> " << types[i] << RESET << "\n";
            } else {
                std::cout << "  " << types[i] << "\n";
            }
        }

        std::cout << "\n[W/S or Up/Down] Navigate | [Enter] Confirm | [ESC] Cancel\n";

        int ch = _getch();

        if (ch == 224 || ch == 0) {
            ch = _getch();
            if (ch == 72 || ch == 'H') { // Up
                typeChoice = (typeChoice - 1 + 3) % 3;
            } else if (ch == 80 || ch == 'P') { // Down
                typeChoice = (typeChoice + 1) % 3;
            }
        } else if (ch == 'w' || ch == 'W') {
            typeChoice = (typeChoice - 1 + 3) % 3;
        } else if (ch == 's' || ch == 'S') {
            typeChoice = (typeChoice + 1) % 3;
        } else if (ch == 13) { // Enter
            break;
        } else if (ch == 27) { // ESC
            std::cout << "Operation cancelled.\n";
            return;
        }
    }

    std::string selectedType = types[typeChoice];

    // Select current location
    InteractivePaginatedTable locationTable("SELECT CURRENT LOCATION", "Name", "Connections");
    const Node* nodes = cityNetwork.getNodes();

    for (int i = 0; i < cityNetwork.getMaxNodes(); i++) {
        if (nodes[i].active) {
            int connCount = 0;
            Edge* edge = nodes[i].adjacencyList;
            while (edge != nullptr) {
                connCount++;
                edge = edge->next;
            }
            locationTable.addRow(nodes[i].id, nodes[i].name, std::to_string(connCount) + " edges");
        }
    }

    int currentLocation = locationTable.run();
    if (currentLocation == -1) {
        std::cout << "Operation cancelled.\n";
        return;
    }

    // Add vehicle with no destination initially (-1)
    vehicleRegistry.addVehicleAuto(plate, selectedType, currentLocation, -1);
}

void searchVehicle() {
    std::cout << BOLD << "Search vehicle" << RESET << "\n";
    int id = getValidInt("Enter vehicle ID to search: ");

    Vehicle* vehicle = vehicleRegistry.searchVehicle(id);
    if (vehicle != nullptr) {
        std::cout << "\nVehicle found!\n";
        std::cout << "ID: " << vehicle->id << "\n";
        std::cout << "Plate: " << vehicle->plate << "\n";
        std::cout << "Type: " << vehicle->type << "\n";
        std::cout << "Current Node: " << vehicle->currentNodeId << "\n";
        std::cout << "Destination Node: " << vehicle->destinationNodeId << "\n";
    } else {
        std::cout << "Vehicle not found!\n";
    }
}

void removeVehicle() {
    std::cout << BOLD << "Remove vehicle" << RESET << "\n";
    int id = getValidInt("Enter vehicle ID to remove: ");
    vehicleRegistry.removeVehicle(id);
}

void showAllVehicles() {
    vehicleRegistry.showAllVehicles();
}

void showHashInfo() {
    vehicleRegistry.showHashInfo();
}

void moveVehicle() {
    std::cout << BOLD << "Move vehicle" << RESET << "\n";

    if (vehicleRegistry.getVehicleCount() == 0) {
        std::cout << "Error: No vehicles registered!\n";
        return;
    }

    if (cityNetwork.getNodeCount() < 2) {
        std::cout << "Error: Need at least 2 nodes in network!\n";
        return;
    }

    // Select vehicle
    InteractivePaginatedTable vehicleTable("SELECT VEHICLE TO MOVE", "Plate", "Type", "Location");
    const Vehicle* vehicles = vehicleRegistry.getVehicles();

    for (int i = 0; i < vehicleRegistry.getHashSize(); i++) {
        if (vehicles[i].active) {
            std::string location = cityNetwork.nodeExists(vehicles[i].currentNodeId)
                ? cityNetwork.getNodeName(vehicles[i].currentNodeId)
                : "Unknown";
            vehicleTable.addRow(vehicles[i].id, vehicles[i].plate, vehicles[i].type, location);
        }
    }

    int vehicleId = vehicleTable.run();
    if (vehicleId == -1) {
        std::cout << "Operation cancelled.\n";
        return;
    }

    Vehicle* vehicle = vehicleRegistry.searchVehicle(vehicleId);
    if (vehicle == nullptr) {
        std::cout << "Error: Vehicle not found!\n";
        return;
    }

    // Select destination
    InteractivePaginatedTable destTable("SELECT DESTINATION", "Name", "Connections");
    const Node* nodes = cityNetwork.getNodes();

    for (int i = 0; i < cityNetwork.getMaxNodes(); i++) {
        if (nodes[i].active) {
            int connCount = 0;
            Edge* edge = nodes[i].adjacencyList;
            while (edge != nullptr) {
                connCount++;
                edge = edge->next;
            }
            destTable.addRow(nodes[i].id, nodes[i].name, std::to_string(connCount) + " edges");
        }
    }

    int destId = destTable.run();
    if (destId == -1) {
        std::cout << "Operation cancelled.\n";
        return;
    }

    if (vehicle->currentNodeId == destId) {
        system("cls");
        std::cout << "Error: Vehicle is already at destination!\n";
        return;
    }

    // Calculate route using Dijkstra
    PathResult result = dijkstra(cityNetwork, vehicle->currentNodeId, destId, true);

    if (result.found) {
        // Update vehicle location
        vehicle->currentNodeId = destId;
        vehicle->destinationNodeId = destId;

        // Log successful movement
        vehicleRegistry.logMovement(vehicleId, destId, "success", result.travelTimeMinutes, "");

        displayPath(cityNetwork, result);
        std::cout << "\n" << CYAN << "Vehicle moved successfully!" << RESET << "\n";
        std::cout << "New location: [" << destId << "] " << cityNetwork.getNodeName(destId) << "\n";
    } else {
        // Log failed movement
        vehicleRegistry.logMovement(vehicleId, destId, "failed", 0.0, "No path found");

        displayPath(cityNetwork, result);
        std::cout << "\n" << "Movement failed: No path found!\n";
    }
}

void showMovementHistory() {
    vehicleRegistry.showMovementHistory();
}

// ===== EXIT =====
void exitAction() {
    std::cout << "Exiting application...\n";
    exit(0);
}

int main() {
    // 1. Road Network Management submenu
    auto roadNetworkMenu = std::make_shared<Menu>("Road Network Management");
    roadNetworkMenu->addItem(1, MenuItem("Load Network", loadNetwork));
    roadNetworkMenu->addItem(2, MenuItem("Save Network", saveNetwork));
    roadNetworkMenu->addItem(3, MenuItem("Add Node", addNode));
    roadNetworkMenu->addItem(4, MenuItem("Add Edge", addEdge));
    roadNetworkMenu->addItem(5, MenuItem("Remove Node", removeNode));
    roadNetworkMenu->addItem(6, MenuItem("Remove Edge", removeEdge));
    roadNetworkMenu->addItem(7, MenuItem("Generate Seed File", generateSeedFile));

    // 2. Network Visualization submenu
    auto visualizationMenu = std::make_shared<Menu>("Network Visualization");
    visualizationMenu->addItem(1, MenuItem("Show Adjacency List", showAdjacencyList));
    visualizationMenu->addItem(2, MenuItem("Show Adjacency Matrix", showAdjacencyMatrix));

    // 3. Queries and Algorithms submenu (kept for manual testing)
    auto algorithmsMenu = std::make_shared<Menu>("Network Analysis (Manual)");
    algorithmsMenu->addItem(1, MenuItem("Find Shortest Path", findShortestPath));
    algorithmsMenu->addItem(2, MenuItem("Breadth-First Search", breadthFirstSearch));
    algorithmsMenu->addItem(3, MenuItem("Depth-First Search", depthFirstSearch));

    // 4. Vehicle Management submenu
    auto vehicleMenu = std::make_shared<Menu>("Vehicle Management");
    vehicleMenu->addItem(1, MenuItem("Load Vehicles", loadVehicles));
    vehicleMenu->addItem(2, MenuItem("Save Vehicles", saveVehicles));
    vehicleMenu->addItem(3, MenuItem("Add Vehicle", addVehicle));
    vehicleMenu->addItem(4, MenuItem("Move Vehicle (Uses Dijkstra)", moveVehicle));
    vehicleMenu->addItem(5, MenuItem("Search Vehicle", searchVehicle));
    vehicleMenu->addItem(6, MenuItem("Remove Vehicle", removeVehicle));
    vehicleMenu->addItem(7, MenuItem("Show All Vehicles", showAllVehicles));
    vehicleMenu->addItem(8, MenuItem("Show Movement History", showMovementHistory));
    vehicleMenu->addItem(9, MenuItem("Show Hash Info", showHashInfo));
    vehicleMenu->addItem(10, MenuItem("Generate Vehicle Seed File", generateVehicleSeedFile));

    // Main menu
    Menu mainMenu("Main menu");
    mainMenu.addItem(1, MenuItem("Road Network Management", roadNetworkMenu));
    mainMenu.addItem(2, MenuItem("Network Visualization", visualizationMenu));
    mainMenu.addItem(3, MenuItem("Vehicle Management", vehicleMenu));
    mainMenu.addItem(4, MenuItem("Network Analysis (Manual)", algorithmsMenu));
    mainMenu.addItem(5, MenuItem("Exit", exitAction));

    mainMenu.run();

    return 0;
}

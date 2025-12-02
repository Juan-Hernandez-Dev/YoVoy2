#include <iostream>
#include <map>
#include <string>
#include <functional>
#include <memory>
#include <fstream>
#include <conio.h>
#include "utils/colors.h"
#include "utils/fileUtils.h"
#include "utils/inputUtils.h"
#include "Graph.h"
#include "HashTable.h"

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
    std::cout << "=== LOAD NETWORK ===\n\n";
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
    std::cout << "=== SAVE NETWORK ===\n\n";

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
    std::cout << "=== ADD NODE ===\n\n";
    std::string name = getValidString("Enter node name: ");
    cityNetwork.addNodeAuto(name);
}

void addEdge() {
    std::cout << "=== ADD EDGE ===\n\n";

    int sourceId = getValidInt("Enter source node ID: ");
    int destId = getValidInt("Enter destination node ID: ");
    double weight = getValidDouble("Enter weight (distance/time): ");

    cityNetwork.addEdge(sourceId, destId, weight);
}

void removeNode() {
    std::cout << "=== REMOVE NODE ===\n\n";
    int id = getValidInt("Enter node ID to remove: ");
    cityNetwork.removeNode(id);
}

void removeEdge() {
    std::cout << "=== REMOVE EDGE ===\n\n";
    int sourceId = getValidInt("Enter source node ID: ");
    int destId = getValidInt("Enter destination node ID: ");
    cityNetwork.removeEdge(sourceId, destId);
}

void generateSeedFile() {
    std::cout << "=== GENERATE SEED FILE ===\n\n";
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

// ===== NETWORK VISUALIZATION =====
void showAdjacencyList() {
    cityNetwork.showAdjacencyList();
}

void showAdjacencyMatrix() {
    cityNetwork.showAdjacencyMatrix();
}

// ===== QUERIES AND ALGORITHMS =====
void findShortestPath() {
    std::cout << "Finding shortest path (Dijkstra)...\n";
    // TODO: Implement Dijkstra's algorithm
}

void breadthFirstSearch() {
    std::cout << "Performing Breadth-First Search...\n";
    // TODO: Implement BFS
}

void depthFirstSearch() {
    std::cout << "Performing Depth-First Search...\n";
    // TODO: Implement DFS
}

// ===== VEHICLE MANAGEMENT =====
void loadVehicles() {
    std::cout << "=== LOAD VEHICLES ===\n\n";
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
    std::cout << "=== SAVE VEHICLES ===\n\n";

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
    std::cout << "=== ADD VEHICLE ===\n\n";

    std::string plate = getValidString("Enter vehicle plate: ");
    std::string type = getValidString("Enter vehicle type (Sedan/Truck/SUV/etc): ");
    int origin = getValidInt("Enter origin node ID: ");
    int dest = getValidInt("Enter destination node ID: ");

    vehicleRegistry.addVehicleAuto(plate, type, origin, dest);
}

void searchVehicle() {
    std::cout << "=== SEARCH VEHICLE ===\n\n";
    int id = getValidInt("Enter vehicle ID to search: ");

    Vehicle* vehicle = vehicleRegistry.searchVehicle(id);
    if (vehicle != nullptr) {
        std::cout << "\nVehicle found!\n";
        std::cout << "ID: " << vehicle->id << "\n";
        std::cout << "Plate: " << vehicle->plate << "\n";
        std::cout << "Type: " << vehicle->type << "\n";
        std::cout << "Origin Node: " << vehicle->originNodeId << "\n";
        std::cout << "Destination Node: " << vehicle->destinationNodeId << "\n";
    } else {
        std::cout << "Vehicle not found!\n";
    }
}

void removeVehicle() {
    std::cout << "=== REMOVE VEHICLE ===\n\n";
    int id = getValidInt("Enter vehicle ID to remove: ");
    vehicleRegistry.removeVehicle(id);
}

void showAllVehicles() {
    vehicleRegistry.showAllVehicles();
}

void showHashInfo() {
    vehicleRegistry.showHashInfo();
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

    // 3. Queries and Algorithms submenu
    auto algorithmsMenu = std::make_shared<Menu>("Queries and algorithms");
    algorithmsMenu->addItem(1, MenuItem("Find Shortest Path (Dijkstra)", findShortestPath));
    algorithmsMenu->addItem(2, MenuItem("Breadth-First Search (BFS)", breadthFirstSearch));
    algorithmsMenu->addItem(3, MenuItem("Depth-First Search (DFS)", depthFirstSearch));

    // 4. Vehicle Management submenu
    auto vehicleMenu = std::make_shared<Menu>("Vehicle Management");
    vehicleMenu->addItem(1, MenuItem("Load Vehicles", loadVehicles));
    vehicleMenu->addItem(2, MenuItem("Save Vehicles", saveVehicles));
    vehicleMenu->addItem(3, MenuItem("Add Vehicle", addVehicle));
    vehicleMenu->addItem(4, MenuItem("Search Vehicle", searchVehicle));
    vehicleMenu->addItem(5, MenuItem("Remove Vehicle", removeVehicle));
    vehicleMenu->addItem(6, MenuItem("Show All Vehicles", showAllVehicles));
    vehicleMenu->addItem(7, MenuItem("Show Hash Info", showHashInfo));

    // Main menu
    Menu mainMenu("Main menu");
    mainMenu.addItem(1, MenuItem("Road Network Management", roadNetworkMenu));
    mainMenu.addItem(2, MenuItem("Network Visualization", visualizationMenu));
    mainMenu.addItem(3, MenuItem("Queries and Algorithms", algorithmsMenu));
    mainMenu.addItem(4, MenuItem("Vehicle Management", vehicleMenu));
    mainMenu.addItem(5, MenuItem("Exit", exitAction));

    mainMenu.run();

    return 0;
}

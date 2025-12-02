#include <iostream>
#include <map>
#include <string>
#include <functional>
#include <memory>
#include <conio.h>
#include "utils/colors.h"

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
    std::cout << "Loading network from file...\n";
    // TODO: Implement file loading logic
}

void saveNetwork() {
    std::cout << "Saving network to file...\n";
    // TODO: Implement file saving logic
}

void addNode() {
    std::cout << "Adding node to network...\n";
    // TODO: Implement add node logic
}

void addEdge() {
    std::cout << "Adding edge to network...\n";
    // TODO: Implement add edge logic
}

void removeNode() {
    std::cout << "Removing node from network...\n";
    // TODO: Implement remove node logic
}

void removeEdge() {
    std::cout << "Removing edge from network...\n";
    // TODO: Implement remove edge logic
}

// ===== NETWORK VISUALIZATION =====
void showAdjacencyList() {
    std::cout << "Displaying adjacency list...\n";
    // TODO: Implement adjacency list display
}

void showAdjacencyMatrix() {
    std::cout << "Displaying adjacency matrix...\n";
    // TODO: Implement adjacency matrix display
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
void addVehicle() {
    std::cout << "Adding vehicle...\n";
    // TODO: Implement add vehicle logic
}

void searchVehicle() {
    std::cout << "Searching for vehicle...\n";
    // TODO: Implement search vehicle logic
}

void removeVehicle() {
    std::cout << "Removing vehicle...\n";
    // TODO: Implement remove vehicle logic
}

void showHashInfo() {
    std::cout << "Displaying hash table information...\n";
    // TODO: Implement hash info display
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
    vehicleMenu->addItem(1, MenuItem("Add Vehicle", addVehicle));
    vehicleMenu->addItem(2, MenuItem("Search Vehicle", searchVehicle));
    vehicleMenu->addItem(3, MenuItem("Remove Vehicle", removeVehicle));
    vehicleMenu->addItem(4, MenuItem("Show Hash Info", showHashInfo));

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

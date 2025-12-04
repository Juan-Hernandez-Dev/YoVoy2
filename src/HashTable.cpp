#include "HashTable.h"
#include "utils/colors.h"
#include <iomanip>
#include <sstream>

HashTable::HashTable() : vehicleCount(0), nextId(0), currentFile("") {
    for (int i = 0; i < HASH_SIZE; i++) {
        table[i].active = false;
    }
}

HashTable::~HashTable() {
    // Nothing to clean up (no dynamic memory)
}

int HashTable::hashFunction(int id) {
    return id % HASH_SIZE;
}

int HashTable::findSlot(int id) {
    int index = hashFunction(id);
    int originalIndex = index;
    int probeCount = 0;

    while (table[index].active && table[index].id != id && probeCount < HASH_SIZE) {
        index = (index + 1) % HASH_SIZE;  // Linear probing
        probeCount++;
    }

    if (probeCount >= HASH_SIZE) {
        return -1;  // Table is full
    }

    return index;
}

void HashTable::updateNextId() {
    int maxId = -1;
    for (int i = 0; i < HASH_SIZE; i++) {
        if (table[i].active && table[i].id > maxId) {
            maxId = table[i].id;
        }
    }
    nextId = maxId + 1;
}

void HashTable::autoSave() {
    if (!currentFile.empty()) {
        std::ofstream file(currentFile);
        if (!file.is_open()) {
            return;
        }

        file << "# VEHICLES\n";
        for (int i = 0; i < HASH_SIZE; i++) {
            if (table[i].active) {
                file << "V;" << table[i].id << ";" << table[i].plate << ";"
                     << table[i].type << ";" << table[i].currentNodeId << ";"
                     << table[i].destinationNodeId << "\n";
            }
        }

        file.close();
    }
}

bool HashTable::addVehicle(int id, const std::string& plate, const std::string& type, int origin, int dest) {
    if (vehicleCount >= HASH_SIZE) {
        std::cout << "Error: Hash table is full!\n";
        return false;
    }

    if (plate.empty()) {
        std::cout << "Error: Plate cannot be empty!\n";
        return false;
    }

    if (type.empty()) {
        std::cout << "Error: Vehicle type cannot be empty!\n";
        return false;
    }

    int index = findSlot(id);
    if (index == -1) {
        std::cout << "Error: Could not find slot for vehicle!\n";
        return false;
    }

    if (table[index].active && table[index].id == id) {
        std::cout << "Error: Vehicle with ID " << id << " already exists!\n";
        return false;
    }

    table[index] = Vehicle(id, plate, type, origin, dest);
    vehicleCount++;

    if (id >= nextId) {
        nextId = id + 1;
    }

    std::cout << "Vehicle added successfully: [" << id << "] " << plate << " (" << type << ")\n";
    autoSave();
    return true;
}

bool HashTable::addVehicleAuto(const std::string& plate, const std::string& type, int origin, int dest) {
    if (vehicleCount >= HASH_SIZE) {
        std::cout << "Error: Hash table is full!\n";
        return false;
    }

    if (plate.empty()) {
        std::cout << "Error: Plate cannot be empty!\n";
        return false;
    }

    if (type.empty()) {
        std::cout << "Error: Vehicle type cannot be empty!\n";
        return false;
    }

    int id = nextId;
    int index = findSlot(id);

    if (index == -1) {
        std::cout << "Error: Could not find slot for vehicle!\n";
        return false;
    }

    table[index] = Vehicle(id, plate, type, origin, dest);
    vehicleCount++;
    nextId++;

    std::cout << "Vehicle added successfully: [" << id << "] " << plate << " (" << type << ")\n";
    autoSave();
    return true;
}

bool HashTable::removeVehicle(int id) {
    int index = hashFunction(id);
    int originalIndex = index;
    int probeCount = 0;

    while (probeCount < HASH_SIZE) {
        if (table[index].active && table[index].id == id) {
            table[index].active = false;
            vehicleCount--;
            std::cout << "Vehicle removed successfully: [" << id << "]\n";
            autoSave();
            return true;
        }

        if (!table[index].active && table[index].id == -1) {
            break;  // Never been used, vehicle doesn't exist
        }

        index = (index + 1) % HASH_SIZE;
        probeCount++;
    }

    std::cout << "Error: Vehicle with ID " << id << " not found!\n";
    return false;
}

Vehicle* HashTable::searchVehicle(int id) {
    int index = hashFunction(id);
    int originalIndex = index;
    int probeCount = 0;

    while (probeCount < HASH_SIZE) {
        if (table[index].active && table[index].id == id) {
            return &table[index];
        }

        if (!table[index].active && table[index].id == -1) {
            break;
        }

        index = (index + 1) % HASH_SIZE;
        probeCount++;
    }

    return nullptr;
}

bool HashTable::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error: Could not open file '" << filename << "'\n";
        return false;
    }

    clearTable();

    std::string line;
    int vehiclesLoaded = 0;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        std::string type;
        std::getline(ss, type, ';');

        if (type == "V") {
            std::string idStr, plate, vType, originStr, destStr;
            std::getline(ss, idStr, ';');
            std::getline(ss, plate, ';');
            std::getline(ss, vType, ';');
            std::getline(ss, originStr, ';');
            std::getline(ss, destStr);

            int id = std::stoi(idStr);
            int origin = std::stoi(originStr);
            int dest = std::stoi(destStr);

            if (addVehicle(id, plate, vType, origin, dest)) {
                vehiclesLoaded++;
            }
        }
    }

    file.close();
    updateNextId();
    currentFile = filename;

    std::cout << "\nVehicles loaded successfully!\n";
    std::cout << "Total vehicles: " << vehiclesLoaded << "\n";
    std::cout << "Auto-save enabled to: " << currentFile << "\n";
    return true;
}

bool HashTable::saveToFile(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error: Could not create file '" << filename << "'\n";
        return false;
    }

    file << "# VEHICLES\n";
    for (int i = 0; i < HASH_SIZE; i++) {
        if (table[i].active) {
            file << "V;" << table[i].id << ";" << table[i].plate << ";"
                 << table[i].type << ";" << table[i].currentNodeId << ";"
                 << table[i].destinationNodeId << "\n";
        }
    }

    file.close();
    std::cout << "Vehicles saved successfully to '" << filename << "'\n";
    return true;
}

void HashTable::clearTable() {
    for (int i = 0; i < HASH_SIZE; i++) {
        table[i].active = false;
    }
    vehicleCount = 0;
    nextId = 0;
    currentFile = "";
}

void HashTable::showAllVehicles() {
    if (vehicleCount == 0) {
        std::cout << "No vehicles registered!\n";
        return;
    }

    std::cout << "\n" << BOLD << "Registered vehicles" << RESET << "\n";
    std::cout << std::setw(5) << "ID" << " | "
              << std::setw(12) << "Plate" << " | "
              << std::setw(15) << "Type" << " | "
              << std::setw(8) << "Origin" << " | "
              << std::setw(8) << "Dest" << "\n";
    std::cout << std::string(65, '-') << "\n";

    for (int i = 0; i < HASH_SIZE; i++) {
        if (table[i].active) {
            std::cout << std::setw(5) << table[i].id << " | "
                      << std::setw(12) << table[i].plate << " | "
                      << std::setw(15) << table[i].type << " | "
                      << std::setw(8) << table[i].currentNodeId << " | "
                      << std::setw(8) << table[i].destinationNodeId << "\n";
        }
    }
    std::cout << "\n";
}

void HashTable::showHashInfo() {
    std::cout << "\n" << BOLD << "Hash table statistics" << RESET << "\n";

    // Calculate statistics
    int usedSlots = 0;
    for (int i = 0; i < HASH_SIZE; i++) {
        if (table[i].active || table[i].id != -1) {
            usedSlots++;
        }
    }
    double loadFactor = (double)vehicleCount / HASH_SIZE;
    int availableCapacity = HASH_SIZE - vehicleCount;
    double usagePercent = (double)vehicleCount / HASH_SIZE * 100;
    int collisions = usedSlots - vehicleCount;

    // Main statistics table
    std::cout << std::setw(25) << "Metric" << " | "
              << std::setw(15) << "Value" << " | "
              << std::setw(30) << "Status" << "\n";
    std::cout << std::string(75, '-') << "\n";

    // Vehicles stored
    std::cout << std::setw(25) << "Vehicles registered" << " | "
              << std::setw(15) << vehicleCount << " | ";
    if (vehicleCount == 0) {
        std::cout << std::setw(30) << "Empty registry";
    } else if (vehicleCount < 50) {
        std::cout << std::setw(30) << "Light usage";
    } else if (vehicleCount < 70) {
        std::cout << std::setw(30) << "Moderate usage";
    } else {
        std::cout << std::setw(30) << "Heavy usage";
    }
    std::cout << "\n";

    // Storage capacity
    std::cout << std::setw(25) << "Storage capacity" << " | "
              << std::setw(15) << HASH_SIZE << " | "
              << std::setw(30) << "Maximum slots" << "\n";

    // Available space
    std::cout << std::setw(25) << "Available space" << " | "
              << std::setw(15) << availableCapacity << " | "
              << std::setw(30) << "Slots remaining" << "\n";

    // Usage percentage
    std::cout << std::setw(25) << "Usage percentage" << " | "
              << std::setw(14) << std::fixed << std::setprecision(1) << usagePercent << "%" << " | ";
    if (usagePercent < 50) {
        std::cout << std::setw(30) << "Plenty of room";
    } else if (usagePercent < 70) {
        std::cout << std::setw(30) << "Getting busy";
    } else if (usagePercent < 90) {
        std::cout << std::setw(30) << "Consider archiving";
    } else {
        std::cout << std::setw(30) << "Nearly full";
    }
    std::cout << "\n";

    // Load factor
    std::cout << std::setw(25) << "Load factor" << " | "
              << std::setw(15) << std::fixed << std::setprecision(3) << loadFactor << " | ";
    if (loadFactor < 0.5) {
        std::cout << GREEN << std::setw(30) << "Excellent performance" << RESET;
    } else if (loadFactor < 0.7) {
        std::cout << GREEN << std::setw(30) << "Good performance" << RESET;
    } else if (loadFactor < 0.9) {
        std::cout << YELLOW << std::setw(30) << "Fair performance" << RESET;
    } else {
        std::cout << YELLOW << std::setw(30) << "Performance degrading" << RESET;
    }
    std::cout << "\n";

    // Occupied slots
    std::cout << std::setw(25) << "Occupied slots" << " | "
              << std::setw(15) << (std::to_string(usedSlots) + " / " + std::to_string(HASH_SIZE)) << " | "
              << std::setw(30) << "Actual positions used" << "\n";

    // Collisions
    std::cout << std::setw(25) << "Collisions handled" << " | "
              << std::setw(15) << collisions << " | ";
    if (collisions == 0) {
        std::cout << GREEN << std::setw(30) << "Perfect hashing" << RESET;
    } else if (collisions < 5) {
        std::cout << std::setw(30) << "Minimal collisions";
    } else if (collisions < 10) {
        std::cout << std::setw(30) << "Some collisions";
    } else {
        std::cout << YELLOW << std::setw(30) << "Many collisions" << RESET;
    }
    std::cout << "\n";

    // Next ID
    std::cout << std::setw(25) << "Next vehicle ID" << " | "
              << std::setw(15) << nextId << " | "
              << std::setw(30) << "Auto-increment" << "\n";

    // Data source
    std::cout << std::setw(25) << "Data source" << " | "
              << std::setw(15);
    if (!currentFile.empty()) {
        // Extract just the filename
        size_t lastSlash = currentFile.find_last_of("/\\");
        std::string filename = (lastSlash != std::string::npos)
            ? currentFile.substr(lastSlash + 1)
            : currentFile;
        std::cout << filename;
    } else {
        std::cout << "Not saved";
    }
    std::cout << " | ";
    if (!currentFile.empty()) {
        std::cout << std::setw(30) << "Loaded from file";
    } else {
        std::cout << YELLOW << std::setw(30) << "Unsaved changes" << RESET;
    }
    std::cout << "\n";

    std::cout << "\n";
}

void HashTable::logMovement(int vehicleId, int destNodeId, const std::string& status,
                           double travelTime, const std::string& failReason) {
    std::ofstream file("data/.movements.csv", std::ios::app);
    if (!file.is_open()) {
        return;
    }

    file << vehicleId << ";" << destNodeId << ";" << status << ";"
         << travelTime << ";" << failReason << "\n";

    file.close();
}

void HashTable::showMovementHistory() {
    std::ifstream file("data/.movements.csv");
    if (!file.is_open()) {
        std::cout << "No movement history found.\n";
        return;
    }

    std::cout << "\n" << BOLD << "Movement history" << RESET << "\n";
    std::cout << std::setw(10) << "Vehicle ID" << " | "
              << std::setw(12) << "Destination" << " | "
              << std::setw(10) << "Status" << " | "
              << std::setw(12) << "Time (min)" << " | "
              << "Reason\n";
    std::cout << std::string(70, '-') << "\n";

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string vidStr, destStr, status, timeStr, reason;

        std::getline(ss, vidStr, ';');
        std::getline(ss, destStr, ';');
        std::getline(ss, status, ';');
        std::getline(ss, timeStr, ';');
        std::getline(ss, reason);

        std::cout << std::setw(10) << vidStr << " | "
                  << std::setw(12) << destStr << " | "
                  << std::setw(10) << status << " | "
                  << std::setw(12) << timeStr << " | "
                  << reason << "\n";
    }

    file.close();
    std::cout << "\n";
}

#include "HashTable.h"
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
                     << table[i].type << ";" << table[i].originNodeId << ";"
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
                 << table[i].type << ";" << table[i].originNodeId << ";"
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

    std::cout << "\n===== REGISTERED VEHICLES =====\n\n";
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
                      << std::setw(8) << table[i].originNodeId << " | "
                      << std::setw(8) << table[i].destinationNodeId << "\n";
        }
    }
    std::cout << "\n";
}

void HashTable::showHashInfo() {
    std::cout << "\n===== HASH TABLE INFORMATION =====\n\n";
    std::cout << "Table Size: " << HASH_SIZE << "\n";
    std::cout << "Vehicles Stored: " << vehicleCount << "\n";
    std::cout << "Load Factor: " << std::fixed << std::setprecision(2)
              << (double)vehicleCount / HASH_SIZE << "\n";
    std::cout << "Next Available ID: " << nextId << "\n";

    if (!currentFile.empty()) {
        std::cout << "Current File: " << currentFile << "\n";
    } else {
        std::cout << "Current File: None (not loaded from file)\n";
    }

    // Show collision statistics
    int usedSlots = 0;
    for (int i = 0; i < HASH_SIZE; i++) {
        if (table[i].active || table[i].id != -1) {
            usedSlots++;
        }
    }
    std::cout << "Used Slots: " << usedSlots << "\n";
    std::cout << "\n";
}

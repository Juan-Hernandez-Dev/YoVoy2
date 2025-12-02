#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <string>
#include <fstream>
#include <iostream>

const int HASH_SIZE = 101;  // Prime number for better distribution

struct Vehicle {
    int id;
    std::string plate;
    std::string type;
    int originNodeId;
    int destinationNodeId;
    bool active;

    Vehicle() : id(-1), plate(""), type(""), originNodeId(-1), destinationNodeId(-1), active(false) {}
    Vehicle(int i, const std::string& p, const std::string& t, int o, int d)
        : id(i), plate(p), type(t), originNodeId(o), destinationNodeId(d), active(true) {}
};

class HashTable {
private:
    Vehicle table[HASH_SIZE];
    int vehicleCount;
    int nextId;
    std::string currentFile;

    int hashFunction(int id);
    int findSlot(int id);  // Linear probing
    void autoSave();
    void updateNextId();

public:
    HashTable();
    ~HashTable();

    // CRUD Operations
    bool addVehicle(int id, const std::string& plate, const std::string& type, int origin, int dest);
    bool addVehicleAuto(const std::string& plate, const std::string& type, int origin, int dest);
    bool removeVehicle(int id);
    Vehicle* searchVehicle(int id);

    // File I/O
    bool loadFromFile(const std::string& filename);
    bool saveToFile(const std::string& filename);
    void clearTable();

    // Display
    void showAllVehicles();
    void showHashInfo();

    // Getters
    int getVehicleCount() const { return vehicleCount; }
    int getNextId() const { return nextId; }
    std::string getCurrentFile() const { return currentFile; }
};

#endif

#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <fstream>
#include <iostream>

const int MAX_NODES = 50;

struct Edge {
    int destination;
    double weight;
    Edge* next;

    Edge(int dest, double w) : destination(dest), weight(w), next(nullptr) {}
};

struct Node {
    int id;
    std::string name;
    Edge* adjacencyList;
    bool active;

    Node() : id(-1), name(""), adjacencyList(nullptr), active(false) {}
};

class Graph {
private:
    Node nodes[MAX_NODES];
    int nodeCount;
    int nextId;
    std::string currentFile;  // Track the currently loaded file

    int findNodeIndex(int id);
    void clearEdges(int nodeIndex);
    void updateNextId();
    void autoSave();  // Auto-save to current file

public:
    Graph();
    ~Graph();

    // CRUD Operations
    bool addNode(int id, const std::string& name);
    bool addNodeAuto(const std::string& name);  // Auto-increment version
    bool removeNode(int id);
    bool addEdge(int sourceId, int destId, double weight);
    bool removeEdge(int sourceId, int destId);

    // File I/O
    bool loadFromFile(const std::string& filename);
    bool saveToFile(const std::string& filename);
    void clearNetwork();

    // Display
    void showAdjacencyList();
    void showAdjacencyMatrix();

    // Getters
    int getNodeCount() const { return nodeCount; }
    int getNextId() const { return nextId; }
    bool nodeExists(int id) const;
    std::string getNodeName(int id) const;
    std::string getCurrentFile() const { return currentFile; }
    const Node* getNodes() const { return nodes; }
    int getMaxNodes() const { return MAX_NODES; }
};

#endif

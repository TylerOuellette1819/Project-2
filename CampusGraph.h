// Purpose: Undirected graph for campus buildings and walkways with BFS
// Author: Samah (Member B)
#ifndef CAMPUSGRAPH_H
#define CAMPUSGRAPH_H

#include <string>
#include <vector>
#include <queue>
#include <map>

class CampusGraph {
public:
    CampusGraph();

    // Load buildings from file
    void loadBuildings(const std::string& filepath);

    // Load connections from file
    void loadConnections(const std::string& filepath);

    // Display all buildings and their neighbors
    void displayGraph() const;

    // Display building counts and degrees
    void displayCountsAndDegrees() const;

    // BFS to find all reachable buildings from start
    void bfsReachability(const std::string& startBuilding) const;

    // Find shortest path (in number of edges) from start to destination using BFS
    void shortestPath(const std::string& startBuilding, const std::string& destBuilding) const;

    // Generate connectivity report
    void connectivityReport() const;

    // Helper methods for other modules
    int findBuilding(const std::string& name) const;
    std::string getBuildingName(const std::string& name) const;
    int getBuildingCount() const { return static_cast<int>(buildings_.size()); }
    bool hasBuilding(const std::string& name) const;

private:
    struct Building {
        std::string name; // Canonical name from buildings.txt
    };

    std::string trim(const std::string& str) const;
    std::string toLower(const std::string& str) const;
    bool equalsIgnoreCase(const std::string& a, const std::string& b) const;

    std::vector<Building> buildings_;
    std::vector<std::vector<int>> adjacencyList_; // Adjacency list for undirected graph
};

#endif // CAMPUSGRAPH_H

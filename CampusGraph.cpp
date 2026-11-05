// Purpose: Implementation of undirected graph, BFS, and shortest path
// Author: Samah (Member B)
#include "CampusGraph.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <climits>
#include <vector>
#include <queue>

std::string CampusGraph::trim(const std::string& str) const {
    const std::string whitespace = " \t\n\r\f\v";
    size_t first = str.find_first_not_of(whitespace);
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(whitespace);
    return str.substr(first, (last - first + 1));
}

std::string CampusGraph::toLower(const std::string& str) const {
    std::string result = str;
    for (char& ch : result) {
        ch = std::tolower(static_cast<unsigned char>(ch));
    }
    return result;
}

bool CampusGraph::equalsIgnoreCase(const std::string& a, const std::string& b) const {
    std::string la = toLower(a);
    std::string lb = toLower(b);
    return la == lb;
}

static bool isIgnorableLine(const std::string& line) {
    std::string trimmed = line;
    const std::string whitespace = " \t\n\r\f\v";
    size_t first = trimmed.find_first_not_of(whitespace);
    if (first == std::string::npos) return true;
    if (trimmed[first] == '#') return true;
    return false;
}

static std::vector<std::string> splitByDelimiter(const std::string& line, char delimiter) {
    std::vector<std::string> fields;
    std::stringstream ss(line);
    std::string field;
    while (std::getline(ss, field, delimiter)) {
        fields.push_back(field);
    }
    return fields;
}

static std::string getBaseName(const std::string& path) {
    size_t pos = path.find_last_of("/\\");
    if (pos == std::string::npos) return path;
    return path.substr(pos + 1);
}

CampusGraph::CampusGraph() {}

int CampusGraph::findBuilding(const std::string& name) const {
    std::string searchName = trim(name);
    for (size_t i = 0; i < buildings_.size(); ++i) {
        if (equalsIgnoreCase(buildings_[i].name, searchName)) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

std::string CampusGraph::getBuildingName(const std::string& name) const {
    int idx = findBuilding(name);
    if (idx == -1) return trim(name);
    return buildings_[idx].name;
}

bool CampusGraph::hasBuilding(const std::string& name) const {
    return findBuilding(name) != -1;
}

void CampusGraph::loadBuildings(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cout << "Error: cannot open " << getBaseName(filepath) 
                  << "; treating it as empty.\n";
        std::cout << getBaseName(filepath) << ": 0 accepted, 0 rejected\n";
        return;
    }

    int acceptedCount = 0;
    int rejectedCount = 0;
    std::string line;
    int lineNumber = 0;

    while (std::getline(file, line)) {
        ++lineNumber;
        if (isIgnorableLine(line)) {
            continue;
        }

        std::string buildingName = trim(line);
        if (buildingName.empty()) {
            std::cout << getBaseName(filepath) << ":" << lineNumber << ": empty building name\n";
            ++rejectedCount;
            continue;
        }

        if (findBuilding(buildingName) != -1) {
            std::cout << getBaseName(filepath) << ":" << lineNumber 
                      << ": duplicate building name '" << buildingName << "'\n";
            ++rejectedCount;
            continue;
        }

        Building b;
        b.name = buildingName;
        buildings_.push_back(b);
        adjacencyList_.resize(buildings_.size());
        ++acceptedCount;
    }

    file.close();
    std::cout << getBaseName(filepath) << ": " << acceptedCount << " accepted, " 
              << rejectedCount << " rejected\n";
}

void CampusGraph::loadConnections(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cout << "Error: cannot open " << getBaseName(filepath) 
                  << "; treating it as empty.\n";
        std::cout << getBaseName(filepath) << ": 0 accepted, 0 rejected\n";
        return;
    }

    int acceptedCount = 0;
    int rejectedCount = 0;
    std::string line;
    int lineNumber = 0;

    while (std::getline(file, line)) {
        ++lineNumber;
        if (isIgnorableLine(line)) {
            continue;
        }

        std::vector<std::string> fields = splitByDelimiter(line, '|');
        if (fields.size() != 2) {
            std::cout << getBaseName(filepath) << ":" << lineNumber 
                      << ": expected 2 fields separated by '|', found " 
                      << fields.size() << "\n";
            ++rejectedCount;
            continue;
        }

        std::string buildingA = trim(fields[0]);
        std::string buildingB = trim(fields[1]);

        if (buildingA.empty() || buildingB.empty()) {
            std::cout << getBaseName(filepath) << ":" << lineNumber 
                      << ": empty building name in connection\n";
            ++rejectedCount;
            continue;
        }

        int idxA = findBuilding(buildingA);
        int idxB = findBuilding(buildingB);
        
        if (idxA == -1) {
            std::cout << getBaseName(filepath) << ":" << lineNumber 
                      << ": unknown building '" << buildingA << "'\n";
            ++rejectedCount;
            continue;
        }
        if (idxB == -1) {
            std::cout << getBaseName(filepath) << ":" << lineNumber 
                      << ": unknown building '" << buildingB << "'\n";
            ++rejectedCount;
            continue;
        }

        if (idxA == idxB) {
            std::cout << getBaseName(filepath) << ":" << lineNumber 
                      << ": self-connection '" << buildings_[idxA].name << "|" 
                      << buildings_[idxB].name << "' is not allowed\n";
            ++rejectedCount;
            continue;
        }

        bool alreadyConnected = false;
        for (int neighbor : adjacencyList_[idxA]) {
            if (neighbor == idxB) {
                alreadyConnected = true;
                break;
            }
        }
        if (alreadyConnected) {
            std::cout << getBaseName(filepath) << ":" << lineNumber 
                      << ": duplicate connection '" << buildings_[idxA].name 
                      << "|" << buildings_[idxB].name << "'\n";
            ++rejectedCount;
            continue;
        }

        adjacencyList_[idxA].push_back(idxB);
        adjacencyList_[idxB].push_back(idxA);
        ++acceptedCount;
    }

    file.close();
    std::cout << getBaseName(filepath) << ": " << acceptedCount << " accepted, " 
              << rejectedCount << " rejected\n";
}

void CampusGraph::displayGraph() const {
    if (buildings_.empty()) {
        std::cout << "No buildings are loaded.\n";
        return;
    }

    for (size_t i = 0; i < buildings_.size(); ++i) {
        std::cout << buildings_[i].name << "\n";
        if (adjacencyList_[i].empty()) {
            std::cout << "  (no connections)\n";
        } else {
            for (int neighborIdx : adjacencyList_[i]) {
                std::cout << "  " << buildings_[neighborIdx].name << "\n";
            }
        }
    }
}

void CampusGraph::displayCountsAndDegrees() const {
    if (buildings_.empty()) {
        std::cout << "No buildings are loaded.\n";
        return;
    }

    int totalConnections = 0;
    for (size_t i = 0; i < adjacencyList_.size(); ++i) {
        totalConnections += static_cast<int>(adjacencyList_[i].size());
    }
    totalConnections /= 2;

    for (size_t i = 0; i < buildings_.size(); ++i) {
        std::cout << buildings_[i].name << ": " << adjacencyList_[i].size() << "\n";
    }
    std::cout << "Total buildings: " << buildings_.size() << "\n";
    std::cout << "Total connections: " << totalConnections << "\n";
}

void CampusGraph::bfsReachability(const std::string& startBuilding) const {
    int startIdx = findBuilding(startBuilding);
    if (startIdx == -1) {
        std::cout << "Error: unknown building '" << trim(startBuilding) << "'.\n";
        return;
    }

    if (buildings_.empty()) {
        return;
    }

    std::vector<bool> visited(buildings_.size(), false);
    std::queue<int> q;
    std::vector<int> distance(buildings_.size(), -1);

    visited[startIdx] = true;
    distance[startIdx] = 0;
    q.push(startIdx);

    std::vector<int> visitOrder;

    while (!q.empty()) {
        int current = q.front();
        q.pop();
        visitOrder.push_back(current);

        for (int neighbor : adjacencyList_[current]) {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                distance[neighbor] = distance[current] + 1;
                q.push(neighbor);
            }
        }
    }

    int reachableCount = static_cast<int>(visitOrder.size());
    for (int idx : visitOrder) {
        std::cout << buildings_[idx].name << " (distance " << distance[idx] << ")\n";
    }
    std::cout << "Reachable: " << reachableCount << " of " << buildings_.size() << " buildings\n";
}

void CampusGraph::shortestPath(const std::string& startBuilding, const std::string& destBuilding) const {
    int startIdx = findBuilding(startBuilding);
    int destIdx = findBuilding(destBuilding);

    if (startIdx == -1) {
        std::cout << "Error: unknown building '" << trim(startBuilding) << "'.\n";
        return;
    }
    if (destIdx == -1) {
        std::cout << "Error: unknown building '" << trim(destBuilding) << "'.\n";
        return;
    }

    if (startIdx == destIdx) {
        std::cout << buildings_[startIdx].name << "\n";
        std::cout << "Number of connections: 0\n";
        return;
    }

    std::vector<bool> visited(buildings_.size(), false);
    std::vector<int> parent(buildings_.size(), -1);
    std::queue<int> q;

    visited[startIdx] = true;
    parent[startIdx] = -1;
    q.push(startIdx);

    bool found = false;
    while (!q.empty()) {
        int current = q.front();
        q.pop();
        for (int neighbor : adjacencyList_[current]) {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                parent[neighbor] = current;
                if (neighbor == destIdx) {
                    found = true;
                    break;
                }
                q.push(neighbor);
            }
        }
        if (found) break;
    }

    if (!visited[destIdx]) {
        std::cout << "No path exists between " << buildings_[startIdx].name 
                  << " and " << buildings_[destIdx].name << ".\n";
        return;
    }

    std::vector<int> path;
    int cur = destIdx;
    while (cur != -1) {
        path.push_back(cur);
        if (cur == startIdx) break;
        cur = parent[cur];
    }
    std::reverse(path.begin(), path.end());

    for (int idx : path) {
        std::cout << buildings_[idx].name << "\n";
    }
    std::cout << "Number of connections: " << (path.size() - 1) << "\n";
}

void CampusGraph::connectivityReport() const {
    if (buildings_.empty()) {
        std::cout << "No buildings are loaded.\n";
        return;
    }

    int totalConnections = 0;
    for (size_t i = 0; i < adjacencyList_.size(); ++i) {
        totalConnections += static_cast<int>(adjacencyList_[i].size());
    }
    totalConnections /= 2;

    std::cout << "Total buildings: " << buildings_.size() << "\n";
    std::cout << "Total connections: " << totalConnections << "\n";

    int maxDegree = -1;
    for (size_t i = 0; i < adjacencyList_.size(); ++i) {
        if (static_cast<int>(adjacencyList_[i].size()) > maxDegree) {
            maxDegree = static_cast<int>(adjacencyList_[i].size());
        }
    }
    std::cout << "Highest degree (" << maxDegree << "): ";
    bool first = true;
    for (size_t i = 0; i < buildings_.size(); ++i) {
        if (static_cast<int>(adjacencyList_[i].size()) == maxDegree) {
            if (!first) std::cout << ", ";
            std::cout << buildings_[i].name;
            first = false;
        }
    }
    std::cout << "\n";

    std::vector<std::string> isolated;
    for (size_t i = 0; i < buildings_.size(); ++i) {
        if (adjacencyList_[i].empty()) {
            isolated.push_back(buildings_[i].name);
        }
    }
    if (isolated.empty()) {
        std::cout << "Isolated buildings: None\n";
    } else {
        std::cout << "Isolated buildings: ";
        for (size_t i = 0; i < isolated.size(); ++i) {
            std::cout << isolated[i];
            if (i < isolated.size() - 1) std::cout << ", ";
        }
        std::cout << "\n";
    }

    std::vector<bool> visited(buildings_.size(), false);
    int componentCount = 0;
    for (size_t i = 0; i < buildings_.size(); ++i) {
        if (!visited[i]) {
            std::queue<int> q;
            visited[i] = true;
            q.push(static_cast<int>(i));
            while (!q.empty()) {
                int current = q.front();
                q.pop();
                for (int neighbor : adjacencyList_[current]) {
                    if (!visited[neighbor]) {
                        visited[neighbor] = true;
                        q.push(neighbor);
                    }
                }
            }
            ++componentCount;
        }
    }
    std::cout << "Connected components: " << componentCount << "\n";
}

// Purpose: Facility data structure and database for managing campus facilities
// Author: Tyler (Member A)
#ifndef FACILITY_H
#define FACILITY_H

#include <string>
#include <vector>
#include "HashTable.h"

class CampusGraph; // Forward declaration

struct Facility {
    std::string id;        // Facility ID (stored in uppercase, format FXXX)
    std::string name;      // Facility name
    std::string building;  // Building name (canonical from buildings.txt)
    std::string category;  // Facility category
};

class FacilityDatabase {
public:
    FacilityDatabase();

    // Load facilities from file, validate against graph, build hash table
    void loadFromFile(const std::string& filepath, const CampusGraph& graph);

    // Display all facilities in file order
    void displayAll() const;

    // Search by facility ID using hash table
    void searchById(const std::string& rawId) const;

    // Search by facility name (exact match, case-insensitive)
    void searchByName(const std::string& rawName) const;

    // Search by category (exact match, case-insensitive)
    void searchByCategory(const std::string& rawCategory) const;

    // Generate facility category report
    void categoryReport() const;

    // Getters
    int getCount() const { return static_cast<int>(facilities_.size()); }
    const std::vector<Facility>& getAllFacilities() const { return facilities_; }

    // ID validation helpers
    static std::string normalizeId(const std::string& rawId);
    static bool isValidIdFormat(const std::string& normalizedId);

private:
    void printFacility(const Facility& facility) const;
    std::string trim(const std::string& str) const;
    std::string toUpper(const std::string& str) const;
    bool equalsIgnoreCase(const std::string& a, const std::string& b) const;

    std::vector<Facility> facilities_;  // Stored in file order
    HashTable idHashTable_;             // Hash table for ID lookups
};

#endif // FACILITY_H

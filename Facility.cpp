// Purpose: Implementation of facility database, loading, validation, and searches
// Author: Tyler (Member A)
#include "Facility.h"
#include "CampusGraph.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <sstream>

FacilityDatabase::FacilityDatabase() : idHashTable_(HashTable::INITIAL_CAPACITY) {}

// Helper functions
std::string FacilityDatabase::trim(const std::string& str) const {
    const std::string whitespace = " \t\n\r\f\v";
    size_t first = str.find_first_not_of(whitespace);
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(whitespace);
    return str.substr(first, (last - first + 1));
}

std::string FacilityDatabase::toUpper(const std::string& str) const {
    std::string result = str;
    for (char& ch : result) {
        ch = std::toupper(static_cast<unsigned char>(ch));
    }
    return result;
}

bool FacilityDatabase::equalsIgnoreCase(const std::string& a, const std::string& b) const {
    if (a.length() != b.length()) return false;
    for (size_t i = 0; i < a.length(); ++i) {
        if (std::tolower(static_cast<unsigned char>(a[i])) != 
            std::tolower(static_cast<unsigned char>(b[i]))) {
            return false;
        }
    }
    return true;
}

std::string FacilityDatabase::normalizeId(const std::string& rawId) {
    std::string trimmed = rawId;
    const std::string whitespace = " \t\n\r\f\v";
    size_t first = trimmed.find_first_not_of(whitespace);
    if (first == std::string::npos) return "";
    size_t last = trimmed.find_last_not_of(whitespace);
    trimmed = trimmed.substr(first, (last - first + 1));
    // Convert to uppercase
    for (char& ch : trimmed) {
        ch = std::toupper(static_cast<unsigned char>(ch));
    }
    return trimmed;
}

bool FacilityDatabase::isValidIdFormat(const std::string& normalizedId) {
    if (normalizedId.length() != 4) return false;
    if (normalizedId[0] != 'F') return false;
    // Next 3 must be digits
    for (int i = 1; i < 4; ++i) {
        if (!std::isdigit(static_cast<unsigned char>(normalizedId[i]))) {
            return false;
        }
    }
    return true;
}

static bool isIgnorableLine(const std::string& line) {
    std::string trimmed = line;
    const std::string whitespace = " \t\n\r\f\v";
    size_t first = trimmed.find_first_not_of(whitespace);
    if (first == std::string::npos) return true; // blank line
    if (trimmed[first] == '#') return true; // comment line
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

void FacilityDatabase::loadFromFile(const std::string& filepath, const CampusGraph& graph) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cout << "Error: cannot open " << getBaseName(filepath) 
                  << "; treating it as empty.\n";
        // Print load summary
        std::cout << getBaseName(filepath) << ": 0 accepted, 0 rejected (hash capacity " 
                  << idHashTable_.capacity() << ", entries " << idHashTable_.size() 
                  << ", collisions " << idHashTable_.collisions() << ")\n";
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
        if (fields.size() != 4) {
            std::cout << getBaseName(filepath) << ":" << lineNumber 
                      << ": expected 4 fields separated by '|', found " 
                      << fields.size() << "\n";
            ++rejectedCount;
            continue;
        }

        std::string rawId = trim(fields[0]);
        std::string name = trim(fields[1]);
        std::string buildingName = trim(fields[2]);
        std::string category = trim(fields[3]);

        // Check for empty required fields
        if (rawId.empty()) {
            std::cout << getBaseName(filepath) << ":" << lineNumber << ": empty facility ID\n";
            ++rejectedCount;
            continue;
        }
        if (name.empty()) {
            std::cout << getBaseName(filepath) << ":" << lineNumber << ": empty facility name\n";
            ++rejectedCount;
            continue;
        }
        if (buildingName.empty()) {
            std::cout << getBaseName(filepath) << ":" << lineNumber << ": empty building name\n";
            ++rejectedCount;
            continue;
        }
        if (category.empty()) {
            std::cout << getBaseName(filepath) << ":" << lineNumber << ": empty facility category\n";
            ++rejectedCount;
            continue;
        }

        // Normalize and validate ID
        std::string normalizedId = normalizeId(rawId);
        if (!isValidIdFormat(normalizedId)) {
            std::cout << getBaseName(filepath) << ":" << lineNumber 
                      << ": invalid facility ID '" << rawId 
                      << "' (an ID is F followed by exactly three digits)\n";
            ++rejectedCount;
            continue;
        }

        // Check if building exists
        if (graph.findBuilding(buildingName) == -1) {
            std::cout << getBaseName(filepath) << ":" << lineNumber 
                      << ": unknown building '" << buildingName << "'\n";
            ++rejectedCount;
            continue;
        }

        // Get canonical building name from graph
        std::string canonicalBuilding = graph.getBuildingName(buildingName);

        // Try to insert into hash table (duplicate check)
        int facilityIndex = static_cast<int>(facilities_.size());
        if (!idHashTable_.insert(normalizedId, facilityIndex)) {
            // Duplicate ID (case-insensitive) - keep first, reject this one
            std::cout << getBaseName(filepath) << ":" << lineNumber 
                      << ": duplicate facility ID '" << normalizedId << "'\n";
            ++rejectedCount;
            continue;
        }

        // Create and store facility
        Facility facility;
        facility.id = normalizedId;
        facility.name = name;
        facility.building = canonicalBuilding;
        facility.category = category;
        
        facilities_.push_back(facility);
        ++acceptedCount;
    }

    file.close();

    // Print load summary with hash table diagnostics
    std::cout << getBaseName(filepath) << ": " << acceptedCount << " accepted, " 
              << rejectedCount << " rejected";
    if (getBaseName(filepath) == "facilities.txt") {
        std::cout << " (hash capacity " << idHashTable_.capacity() 
                  << ", entries " << idHashTable_.size() 
                  << ", collisions " << idHashTable_.collisions() << ")";
    }
    std::cout << "\n";
}

void FacilityDatabase::printFacility(const Facility& facility) const {
    std::cout << facility.id << " | " << facility.name << " | " 
              << facility.building << " | " << facility.category << "\n";
}

void FacilityDatabase::displayAll() const {
    if (facilities_.empty()) {
        std::cout << "No facilities are loaded.\n";
        return;
    }
    for (const auto& facility : facilities_) {
        printFacility(facility);
    }
    std::cout << "Total facilities: " << facilities_.size() << "\n";
}

void FacilityDatabase::searchById(const std::string& rawId) const {
    std::string normalizedId = normalizeId(rawId);
    
    if (!isValidIdFormat(normalizedId)) {
        std::cout << "Invalid ID format: '" << rawId 
                  << "'. An ID is F followed by exactly three digits (for example F001).\n";
        return;
    }

    int index = -1;
    if (idHashTable_.find(normalizedId, index)) {
        if (index >= 0 && index < static_cast<int>(facilities_.size())) {
            std::cout << "Found: ";
            printFacility(facilities_[index]);
        } else {
            std::cout << "Not found: no facility with ID " << normalizedId << ".\n";
        }
    } else {
        std::cout << "Not found: no facility with ID " << normalizedId << ".\n";
    }
}

void FacilityDatabase::searchByName(const std::string& rawName) const {
    std::string searchName = trim(rawName);
    if (searchName.empty()) {
        std::cout << "Error: name cannot be blank.\n";
        return;
    }

    int matches = 0;
    for (const auto& facility : facilities_) {
        if (equalsIgnoreCase(facility.name, searchName)) {
            printFacility(facility);
            ++matches;
        }
    }
    std::cout << "Matches: " << matches << "\n";
}

void FacilityDatabase::searchByCategory(const std::string& rawCategory) const {
    std::string searchCategory = trim(rawCategory);
    if (searchCategory.empty()) {
        std::cout << "Error: category cannot be blank.\n";
        return;
    }

    int matches = 0;
    for (const auto& facility : facilities_) {
        if (equalsIgnoreCase(facility.category, searchCategory)) {
            printFacility(facility);
            ++matches;
        }
    }
    if (matches == 0) {
        std::cout << "No facilities match category '" << searchCategory << "'. ";
    }
    std::cout << "Matches: " << matches << "\n";
}

void FacilityDatabase::categoryReport() const {
    if (facilities_.empty()) {
        std::cout << "No facilities are available.\n";
        std::cout << "Total facilities: 0\n";
        return;
    }

    // Group by category (case-insensitive), keep first spelling
    struct CategoryStats {
        std::string displayName;
        int count;
    };
    std::vector<CategoryStats> categories;
    
    for (const auto& facility : facilities_) {
        // Find if category already exists (case-insensitive)
        bool found = false;
        for (auto& cat : categories) {
            if (equalsIgnoreCase(cat.displayName, facility.category)) {
                ++cat.count;
                found = true;
                break;
            }
        }
        if (!found) {
            CategoryStats stats;
            stats.displayName = facility.category;
            stats.count = 1;
            categories.push_back(stats);
        }
    }

    // Display report
    std::cout << std::fixed << std::setprecision(1);
    int total = static_cast<int>(facilities_.size());
    for (const auto& cat : categories) {
        double percentage = (total > 0) ? (cat.count * 100.0 / total) : 0.0;
        std::cout << cat.displayName << "  " << cat.count << "  " << percentage << "%\n";
    }
    std::cout << "Total  " << total << "  100.0%\n";
}

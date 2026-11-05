// Purpose: Implementation of custom hash table with separate chaining
// Author: Tyler (Member A)
#include "HashTable.h"
#include <algorithm>
#include <cctype>

// Helper to trim whitespace
static std::string trim(const std::string& str) {
    const std::string whitespace = " \t\n\r\f\v";
    size_t first = str.find_first_not_of(whitespace);
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(whitespace);
    return str.substr(first, (last - first + 1));
}

// Helper to convert to uppercase
static std::string toUpper(const std::string& str) {
    std::string result = str;
    for (char& ch : result) {
        ch = std::toupper(static_cast<unsigned char>(ch));
    }
    return result;
}

HashTable::HashTable(int capacity)
    : capacity_(capacity > 0 ? capacity : INITIAL_CAPACITY),
      size_(0),
      collisions_(0) {
    buckets_.resize(capacity_);
}

unsigned long HashTable::hashFunction(const std::string& normalizedKey, int capacity) {
    // djb2 hash algorithm
    // h = 5381; for each char: h = h * 33 + c
    unsigned long hash = 5381UL;
    for (char ch : normalizedKey) {
        hash = ((hash << 5) + hash) + static_cast<unsigned char>(ch); // hash * 33 + c
    }
    return hash % static_cast<unsigned long>(capacity);
}

bool HashTable::insert(const std::string& key, int value) {
    std::string normalized = toUpper(trim(key));
    if (normalized.empty()) {
        return false;
    }

    unsigned long index = hashFunction(normalized, capacity_);
    std::vector<Entry>& bucket = buckets_[index];

    // Check for duplicate key (case-insensitive)
    for (const auto& entry : bucket) {
        if (entry.key == normalized) {
            return false; // Duplicate - do not overwrite
        }
    }

    // Count collision if bucket is not empty (accepted insertion into non-empty bucket)
    if (!bucket.empty()) {
        ++collisions_;
    }

    Entry newEntry;
    newEntry.key = normalized;
    newEntry.value = value;
    bucket.push_back(newEntry);
    ++size_;
    return true;
}

bool HashTable::find(const std::string& key, int& value) const {
    std::string normalized = toUpper(trim(key));
    if (normalized.empty()) {
        return false;
    }

    unsigned long index = hashFunction(normalized, capacity_);
    const std::vector<Entry>& bucket = buckets_[index];

    for (const auto& entry : bucket) {
        if (entry.key == normalized) {
            value = entry.value;
            return true;
        }
    }
    return false;
}

bool HashTable::contains(const std::string& key) const {
    int ignored = 0;
    return find(key, ignored);
}

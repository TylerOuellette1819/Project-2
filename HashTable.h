// Purpose: Custom hash table keyed by facility ID using separate chaining
// Author: Tyler (Member A)
#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <string>
#include <vector>

class HashTable {
public:
    // Initial bucket count - with separate chaining, chains can grow
    static const int INITIAL_CAPACITY = 17;

    explicit HashTable(int capacity = INITIAL_CAPACITY);

    // Insert a key-value pair. Returns false if duplicate key (case-insensitive),
    // leaving existing entry unchanged. Key is normalized to uppercase.
    bool insert(const std::string& key, int value);

    // Look up a key. Returns true if found, stores value in out parameter.
    bool find(const std::string& key, int& value) const;

    // Check if key exists
    bool contains(const std::string& key) const;

    // Getters for statistics
    int size() const { return size_; }
    int capacity() const { return capacity_; }
    int collisions() const { return collisions_; }

    // Custom string hash function (djb2-style) - does not use std::hash
    static unsigned long hashFunction(const std::string& normalizedKey, int capacity);

private:
    struct Entry {
        std::string key;  // Stored in uppercase
        int value;        // Index into facilities vector
    };

    std::vector<std::vector<Entry>> buckets_;
    int capacity_;
    int size_;
    int collisions_;
};

#endif // HASHTABLE_H

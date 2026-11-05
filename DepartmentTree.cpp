// Purpose: Implementation of general department tree with traversals
// Author: Roland (Member C)
#include "DepartmentTree.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <functional>

DepartmentTree::DepartmentTree() : root_(nullptr) {}

DepartmentTree::~DepartmentTree() {
    destroyTree(root_);
}

void DepartmentTree::destroyTree(DepartmentNode* node) {
    if (node == nullptr) return;
    for (auto child : node->children) {
        destroyTree(child);
    }
    delete node;
}

std::string DepartmentTree::trim(const std::string& str) const {
    const std::string whitespace = " \t\n\r\f\v";
    size_t first = str.find_first_not_of(whitespace);
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(whitespace);
    return str.substr(first, (last - first + 1));
}

std::string DepartmentTree::toLower(const std::string& str) const {
    std::string result = str;
    for (char& ch : result) {
        ch = std::tolower(static_cast<unsigned char>(ch));
    }
    return result;
}

bool DepartmentTree::equalsIgnoreCase(const std::string& a, const std::string& b) const {
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

DepartmentTree::DepartmentNode* DepartmentTree::findNode(const std::string& name) const {
    return findNodeRecursive(root_, name);
}

DepartmentTree::DepartmentNode* DepartmentTree::findNodeRecursive(DepartmentNode* node, const std::string& name) const {
    if (node == nullptr) return nullptr;
    if (equalsIgnoreCase(node->name, name)) {
        return node;
    }
    for (auto child : node->children) {
        DepartmentNode* result = findNodeRecursive(child, name);
        if (result != nullptr) return result;
    }
    return nullptr;
}

void DepartmentTree::loadFromFile(const std::string& filepath) {
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

        std::string deptName = trim(fields[0]);
        std::string parentName = trim(fields[1]);

        if (deptName.empty()) {
            std::cout << getBaseName(filepath) << ":" << lineNumber << ": empty department name\n";
            ++rejectedCount;
            continue;
        }
        if (parentName.empty()) {
            std::cout << getBaseName(filepath) << ":" << lineNumber << ": empty parent name\n";
            ++rejectedCount;
            continue;
        }

        // Check for duplicate
        if (findNode(deptName) != nullptr) {
            std::cout << getBaseName(filepath) << ":" << lineNumber 
                      << ": duplicate department name '" << deptName << "'\n";
            ++rejectedCount;
            continue;
        }

        if (toLower(parentName) == "none") {
            // Must be root (first valid record)
            if (root_ != nullptr) {
                std::cout << getBaseName(filepath) << ":" << lineNumber 
                          << ": root already exists\n";
                ++rejectedCount;
                continue;
            }
            DepartmentNode* newNode = new DepartmentNode();
            newNode->name = deptName;
            newNode->parent = nullptr;
            root_ = newNode;
            ++acceptedCount;
        } else {
            // Must have a parent that was accepted earlier
            DepartmentNode* parentNode = findNode(parentName);
            if (parentNode == nullptr) {
                std::cout << getBaseName(filepath) << ":" << lineNumber 
                          << ": unknown parent '" << parentName << "'\n";
                ++rejectedCount;
                continue;
            }
            DepartmentNode* newNode = new DepartmentNode();
            newNode->name = deptName;
            newNode->parent = parentNode;
            parentNode->children.push_back(newNode);
            ++acceptedCount;
        }
    }

    file.close();
    std::cout << getBaseName(filepath) << ": " << acceptedCount << " accepted, " 
              << rejectedCount << " rejected\n";
}

bool DepartmentTree::addDepartment(const std::string& name, const std::string& parentName, std::string& errorMsg) {
    std::string deptName = trim(name);
    std::string parName = trim(parentName);

    if (deptName.empty()) {
        errorMsg = "department name cannot be empty";
        return false;
    }

    // Check for duplicate
    if (findNode(deptName) != nullptr) {
        errorMsg = "duplicate department name '" + deptName + "'";
        return false;
    }

    if (root_ == nullptr) {
        // Tree is empty - becomes root
        DepartmentNode* newNode = new DepartmentNode();
        newNode->name = deptName;
        newNode->parent = nullptr;
        root_ = newNode;
        return true;
    }

    // Tree not empty - need valid parent
    if (parName.empty() || toLower(parName) == "none") {
        errorMsg = "parent required when tree is not empty";
        return false;
    }

    DepartmentNode* parentNode = findNode(parName);
    if (parentNode == nullptr) {
        errorMsg = "unknown parent '" + parName + "'";
        return false;
    }

    DepartmentNode* newNode = new DepartmentNode();
    newNode->name = deptName;
    newNode->parent = parentNode;
    parentNode->children.push_back(newNode);
    return true;
}

void DepartmentTree::search(const std::string& name) const {
    std::string searchName = trim(name);
    if (searchName.empty()) {
        std::cout << "Error: name cannot be blank.\n";
        return;
    }

    DepartmentNode* node = findNode(searchName);
    if (node == nullptr) {
        std::cout << "Department '" << searchName << "' not found.\n";
        return;
    }

    std::string parentStr = (node->parent == nullptr) ? "None" : node->parent->name;
    int depth = 0;
    DepartmentNode* current = node->parent;
    while (current != nullptr) {
        ++depth;
        current = current->parent;
    }

    std::cout << "Department: " << node->name << " / Parent: " << parentStr 
              << " / Direct children: " << node->children.size() 
              << " / Depth: " << depth << "\n";
}

void DepartmentTree::displayHierarchyRecursive(DepartmentNode* node, int depth) const {
    if (node == nullptr) return;
    // Print indentation (4 spaces per depth)
    for (int i = 0; i < depth; ++i) {
        std::cout << "    ";
    }
    std::cout << node->name << "\n";
    // Children in insertion order
    for (auto child : node->children) {
        displayHierarchyRecursive(child, depth + 1);
    }
}

void DepartmentTree::displayHierarchy() const {
    if (root_ == nullptr) {
        std::cout << "No departments are loaded.\n";
        return;
    }
    displayHierarchyRecursive(root_, 0);
}

void DepartmentTree::displayPreorder() const {
    if (root_ == nullptr) {
        std::cout << "No departments are loaded.\n";
        return;
    }
    std::vector<std::string> result;
    std::function<void(DepartmentNode*)> preorder = [&](DepartmentNode* node) {
        if (!node) return;
        result.push_back(node->name);
        for (auto child : node->children) {
            preorder(child);
        }
    };
    preorder(root_);
    for (size_t i = 0; i < result.size(); ++i) {
        std::cout << result[i];
        if (i < result.size() - 1) std::cout << ", ";
    }
    std::cout << "\n";
}

void DepartmentTree::displayPostorder() const {
    if (root_ == nullptr) {
        std::cout << "No departments are loaded.\n";
        return;
    }
    std::vector<std::string> result;
    std::function<void(DepartmentNode*)> postorder = [&](DepartmentNode* node) {
        if (!node) return;
        for (auto child : node->children) {
            postorder(child);
        }
        result.push_back(node->name);
    };
    postorder(root_);
    for (size_t i = 0; i < result.size(); ++i) {
        std::cout << result[i];
        if (i < result.size() - 1) std::cout << ", ";
    }
    std::cout << "\n";
}

void DepartmentTree::displayLevelOrder() const {
    if (root_ == nullptr) {
        std::cout << "No departments are loaded.\n";
        return;
    }
    std::vector<std::string> result;
    std::queue<DepartmentNode*> q;
    q.push(root_);
    while (!q.empty()) {
        DepartmentNode* current = q.front();
        q.pop();
        result.push_back(current->name);
        for (auto child : current->children) {
            q.push(child);
        }
    }
    for (size_t i = 0; i < result.size(); ++i) {
        std::cout << result[i];
        if (i < result.size() - 1) std::cout << ", ";
    }
    std::cout << "\n";
}

int DepartmentTree::countTotal(DepartmentNode* node) const {
    if (node == nullptr) return 0;
    int count = 1;
    for (auto child : node->children) {
        count += countTotal(child);
    }
    return count;
}

int DepartmentTree::countLeaves(DepartmentNode* node) const {
    if (node == nullptr) return 0;
    if (node->children.empty()) return 1;
    int count = 0;
    for (auto child : node->children) {
        count += countLeaves(child);
    }
    return count;
}

int DepartmentTree::getHeight(DepartmentNode* node) const {
    if (node == nullptr) return -1;
    if (node->children.empty()) return 0;
    int maxHeight = -1;
    for (auto child : node->children) {
        int h = getHeight(child);
        if (h > maxHeight) maxHeight = h;
    }
    return maxHeight + 1;
}

int DepartmentTree::getTotalDepartments() const {
    return countTotal(root_);
}

void DepartmentTree::displayStatistics() const {
    if (root_ == nullptr) {
        std::cout << "No departments are loaded.\n";
        std::cout << "Total departments: 0\n";
        std::cout << "Leaf departments: 0\n";
        std::cout << "Tree height: N/A\n";
        return;
    }
    int total = countTotal(root_);
    int leaves = countLeaves(root_);
    int height = getHeight(root_);
    std::cout << "Total departments: " << total << "\n";
    std::cout << "Leaf departments: " << leaves << "\n";
    std::cout << "Tree height: " << height << "\n";
}

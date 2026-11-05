// Purpose: General tree for department hierarchy with traversals
// Author: Roland (Member C)
#ifndef DEPARTMENTTREE_H
#define DEPARTMENTTREE_H

#include <string>
#include <vector>
#include <queue>

class DepartmentTree {
public:
    DepartmentTree();
    ~DepartmentTree();

    // Disable copying
    DepartmentTree(const DepartmentTree&) = delete;
    DepartmentTree& operator=(const DepartmentTree&) = delete;

    // Load departments from file
    void loadFromFile(const std::string& filepath);

    // Add department (menu operation)
    bool addDepartment(const std::string& name, const std::string& parentName, std::string& errorMsg);

    // Search for department
    void search(const std::string& name) const;

    // Display hierarchy with indentation
    void displayHierarchy() const;

    // Traversals
    void displayPreorder() const;
    void displayPostorder() const;
    void displayLevelOrder() const;

    // Statistics
    void displayStatistics() const;

    bool isEmpty() const { return root_ == nullptr; }
    int getTotalDepartments() const;

private:
    struct DepartmentNode {
        std::string name;
        DepartmentNode* parent;
        std::vector<DepartmentNode*> children;
    };

    std::string trim(const std::string& str) const;
    std::string toLower(const std::string& str) const;
    bool equalsIgnoreCase(const std::string& a, const std::string& b) const;

    DepartmentNode* findNode(const std::string& name) const;
    DepartmentNode* findNodeRecursive(DepartmentNode* node, const std::string& name) const;
    
    void displayHierarchyRecursive(DepartmentNode* node, int depth) const;
    
    int countTotal(DepartmentNode* node) const;
    int countLeaves(DepartmentNode* node) const;
    int getHeight(DepartmentNode* node) const;
    
    void destroyTree(DepartmentNode* node);

    DepartmentNode* root_;
};

#endif // DEPARTMENTTREE_H

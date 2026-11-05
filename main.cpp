// =====================================================
// MAIN - Smart Campus Navigation and Facility Analytics
// Connects: Tyler (Facilities + Hash Table), 
//           Samah (Campus Graph + BFS),
//           Roland (Department Tree)
// =====================================================

// Purpose: Program entry point. Loads the four data files in order, coordinates
//          warnings/summaries, and runs the repeating Final menu.
#include <iostream>
#include <string>

#include "CampusGraph.h"
#include "DepartmentTree.h"
#include "Facility.h"
#include "FileUtil.h"

namespace {

void printMenu() {
    std::cout << "\nSmart Campus Navigation and Facility Analytics\n";
    std::cout << "----------------------------------------------\n";
    std::cout << "1. View all facilities\n";
    std::cout << "2. Search facility by ID\n";
    std::cout << "3. Search facility by name\n";
    std::cout << "4. Search facility by category\n";
    std::cout << "5. View campus graph\n";
    std::cout << "6. Display graph counts and degrees\n";
    std::cout << "7. Find reachable buildings (BFS)\n";
    std::cout << "8. Find shortest path (BFS)\n";
    std::cout << "9. Add department\n";
    std::cout << "10. Search department\n";
    std::cout << "11. Display department hierarchy\n";
    std::cout << "12. Preorder traversal\n";
    std::cout << "13. Postorder traversal\n";
    std::cout << "14. Level-order traversal\n";
    std::cout << "15. Department statistics\n";
    std::cout << "16. Facility category report\n";
    std::cout << "17. Graph connectivity report\n";
    std::cout << "0. Exit\n";
}

bool readLine(const std::string& prompt, std::string& value) {
    std::cout << prompt;
    if (!std::getline(std::cin, value)) {
        std::cout << "\nInput closed.\n";
        return false;
    }
    value = FileUtil::trim(value);
    return true;
}

bool readRequiredName(const std::string& prompt, std::string& value) {
    if (!readLine(prompt, value)) {
        return false;
    }
    if (value.empty()) {
        std::cout << "Error: name cannot be blank.\n";
        return false;
    }
    return true;
}

void addDepartmentInteractive(DepartmentTree& tree) {
    std::string name;
    if (!readRequiredName("Enter department name: ", name)) {
        return;
    }

    std::string parent = "NONE";
    if (!tree.isEmpty()) {
        if (!readRequiredName("Enter parent department name: ", parent)) {
            return;
        }
    }

    std::string error;
    if (!tree.addDepartment(name, parent, error)) {
        std::cout << "Error: " << error << ".\n";
    } else {
        std::cout << "Added department '" << name << "'.\n";
    }
}

}  // namespace

int main(int argc, char** argv) {
    std::string dataDirectory = (argc > 1) ? argv[1] : "data";

    CampusGraph graph;
    FacilityDatabase facilities;
    DepartmentTree departments;

    std::cout << "Loading campus data from '" << dataDirectory << "'...\n\n";

    graph.loadBuildings(FileUtil::joinPath(dataDirectory, "buildings.txt"));
    std::cout << "\n";

    graph.loadConnections(FileUtil::joinPath(dataDirectory, "connections.txt"));
    std::cout << "\n";

    facilities.loadFromFile(FileUtil::joinPath(dataDirectory, "facilities.txt"), graph);
    std::cout << "\n";

    departments.loadFromFile(FileUtil::joinPath(dataDirectory, "departments.txt"));

    while (true) {
        printMenu();
        std::string input;
        if (!readLine("Enter choice: ", input)) {
            break;
        }

        int choice = -1;
        if (!FileUtil::parseNumber(input, choice) || choice < 0 || choice > 17) {
            std::cout << "Invalid menu choice. Enter a number from 0 to 17.\n";
            continue;
        }

        if (choice == 0) {
            std::cout << "Goodbye.\n";
            break;
        }

        std::cout << "\n";
        switch (choice) {
            case 1:
                facilities.displayAll();
                break;
            case 2: {
                std::string id;
                if (readLine("Enter facility ID: ", id)) {
                    facilities.searchById(id);
                }
                break;
            }
            case 3: {
                std::string name;
                if (readRequiredName("Enter facility name: ", name)) {
                    facilities.searchByName(name);
                }
                break;
            }
            case 4: {
                std::string category;
                if (readRequiredName("Enter category: ", category)) {
                    facilities.searchByCategory(category);
                }
                break;
            }
            case 5:
                graph.displayGraph();
                break;
            case 6:
                graph.displayCountsAndDegrees();
                break;
            case 7: {
                std::string start;
                if (readRequiredName("Enter starting building: ", start)) {
                    graph.bfsReachability(start);
                }
                break;
            }
            case 8: {
                std::string start;
                std::string destination;
                if (readRequiredName("Enter starting building: ", start) &&
                    readRequiredName("Enter destination building: ", destination)) {
                    graph.shortestPath(start, destination);
                }
                break;
            }
            case 9:
                addDepartmentInteractive(departments);
                break;
            case 10: {
                std::string name;
                if (readRequiredName("Enter department name: ", name)) {
                    departments.search(name);
                }
                break;
            }
            case 11:
                departments.displayHierarchy();
                break;
            case 12:
                departments.displayPreorder();
                break;
            case 13:
                departments.displayPostorder();
                break;
            case 14:
                departments.displayLevelOrder();
                break;
            case 15:
                departments.displayStatistics();
                break;
            case 16:
                facilities.categoryReport();
                break;
            case 17:
                graph.connectivityReport();
                break;
            default:
                std::cout << "Invalid menu choice. Enter a number from 0 to 17.\n";
                break;
        }
    }

    return 0;
}

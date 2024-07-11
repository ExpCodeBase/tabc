#include "utils.h"
#include <fstream>
#include <sstream>

std::vector<Edge> readEdgesFromFile(const std::string& filename) {
    std::vector<Edge> edges;
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        int u, v, t;
        if (!(iss >> u >> v >> t)) { break; } // error
        edges.emplace_back(u, v, t);
    }
    return edges;
}



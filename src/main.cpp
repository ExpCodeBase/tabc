#include "graph.h"
#include "query.h"
#include "utils.h"
#include <iostream>

int main() {
    // Read the graph from file
    auto edges = readEdgesFromFile("data/graph.e");

    // Build the superior-optimized index
    auto index = buildSuperiorOptimizedIndex(edges);

    // Example query
    int alpha = 2, beta = 1;
    int ts = 3, te = 10;
    auto result = querySuperiorOptimizedIndex(index, alpha, beta, ts, te);

    // Output the result
    std::cout << "Vertices in the (" << alpha << ", " << beta << ")-core for time window <" << ts << ", " << te << ">: ";
    for (int u : result) {
        std::cout << u << " ";
    }
    std::cout << std::endl;

    return 0;
}
#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <unordered_map>
#include <set>
#include <tuple>

using Edge = std::tuple<int, int, int>;
using Index = std::unordered_map<std::pair<int, int>, std::vector<QualifiedTimeWindow>>;

Index buildSuperiorOptimizedIndex(const std::vector<Edge>& edges);

#endif // GRAPH_H
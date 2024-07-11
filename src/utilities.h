#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <tuple>

using Edge = std::tuple<int, int, int>;

std::vector<Edge> readEdgesFromFile(const std::string& filename);

#endif // UTILS_H
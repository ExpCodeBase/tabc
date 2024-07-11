#ifndef QUERY_H
#define QUERY_H

#include "graph.h"
#include <set>

std::set<int> querySuperiorOptimizedIndex(const Index& index, int alpha, int beta, int ts, int te);

#endif // QUERY_H
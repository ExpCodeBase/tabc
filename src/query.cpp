#include <queue>
#include <set>
#include <unordered_map>
#include <vector>

using namespace std;


struct QualifiedTimeWindow {
    int ts, te;
    set<int> vertices;
    set<pair<int, int>> links; // Linked coreness pairs
};

using Index = unordered_map<pair<int, int>, vector<QualifiedTimeWindow>>;

set<int> querySuperiorOptimizedIndex(const unordered_map<pair<int, int>, vector<QualifiedTimeWindow>>& index, int alpha, int beta, int ts, int te) {
    set<int> result;
    queue<pair<int, int>> q;
    set<pair<int, int>> visited;

    // Find the closest dominant coreness pair
    pair<int, int> closestPair = {alpha, beta};
    if (index.find(closestPair) == index.end()) {
        int minDist = INT_MAX;
        for (const auto& [key, qtwList] : index) {
            int dist = abs(key.first - alpha) + abs(key.second - beta);
            if (dist < minDist) {
                minDist = dist;
                closestPair = key;
            }
        }
    }

    if (closestPair == make_pair(alpha, beta) && index.find(closestPair) == index.end()) {
        return result; // No valid result for this query
    }

    q.push(closestPair);

    while (!q.empty()) {
        auto [alpha_, beta_] = q.front();
        q.pop();

        if (visited.find({alpha_, beta_}) != visited.end()) continue;
        visited.insert({alpha_, beta_});

        if (index.find({alpha_, beta_}) != index.end()) {
            for (const auto& qtw : index.at({alpha_, beta_})) {
                if (qtw.ts <= ts && qtw.te >= te) {
                    result.insert(qtw.vertices.begin(), qtw.vertices.end());
                }
            }

            for (const auto& linked_pair : index.at({alpha_, beta_}).front().links) {
                q.push(linked_pair);
            }
        }
    }

    return result;
}
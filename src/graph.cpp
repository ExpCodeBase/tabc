#include "graph.h"
#include "utils.h"
#include <fstream>
#include <sstream>


struct CorenessPair {
    int alpha, beta;
    set<pair<int, int>> qtw; // Qualified Time Windows
};


struct QualifiedTimeWindow {
    int ts, te;
    set<int> vertices;
    set<pair<int, int>> links; // Linked coreness pairs
};


int shortestABCoreTime(const vector<tuple<int, int, int>>& edges, int u, int ts, int alpha, int beta) {
    unordered_map<int, int> degree;
    for (const auto& [u, v, t] : edges) {
        if (t >= ts && (u == u || v == u)) {
            degree[u]++;
        }
    }
    int te = ts;
    while (degree[u] < alpha && te <= tmax) {
        te++;
        for (const auto& [u, v, t] : edges) {
            if (t == te && (u == u || v == u)) {
                degree[u]++;
            }
        }
    }
    return te;
}

set<pair<int, int>> computeQualifiedTimeWindows(const vector<tuple<int, int, int>>& edges, int u, int alpha, int beta) {
    set<pair<int, int>> qtw;
    for (int ts = 1; ts <= tmax; ++ts) {
        int te = shortestABCoreTime(edges, u, ts, alpha, beta);
        if (te <= tmax) {
            qtw.insert({ts, te});
        }
    }
    return qtw;
}


unordered_map<int, vector<CorenessPair>> buildVertexBasedIndex(const vector<tuple<int, int, int>>& edges) {
    unordered_map<int, vector<CorenessPair>> index;
    set<int> vertices;

    // Extract all vertices
    for (const auto& [u, v, t] : edges) {
        vertices.insert(u);
        vertices.insert(v);
    }

    // Build index for each vertex
    for (int u : vertices) {
        vector<CorenessPair> cpList;
        for (int alpha = 1; alpha <= maxAlpha; ++alpha) {
            for (int beta = 1; beta <= maxBeta; ++beta) {
                CorenessPair cp;
                cp.alpha = alpha;
                cp.beta = beta;
                cp.qtw = computeQualifiedTimeWindows(edges, u, alpha, beta);
                if (!cp.qtw.empty()) {
                    cpList.push_back(cp);
                }
            }
        }
        index[u] = cpList;
    }

    return index;
}


vector<QualifiedTimeWindow> computeQualifiedTimeWindowsForCorenessPair(const vector<tuple<int, int, int>>& edges, int alpha, int beta) {
    vector<QualifiedTimeWindow> qtwList;
    set<int> vertices;

    // Extract all vertices
    for (const auto& [u, v, t] : edges) {
        vertices.insert(u);
        vertices.insert(v);
    }

    // Compute qualified time windows for each vertex and aggregate them
    for (int u : vertices) {
        set<pair<int, int>> qtw = computeQualifiedTimeWindows(edges, u, alpha, beta);
        for (const auto& [ts, te] : qtw) {
            bool found = false;
            for (auto& qtwEntry : qtwList) {
                if (qtwEntry.ts == ts && qtwEntry.te == te) {
                    qtwEntry.vertices.insert(u);
                    found = true;
                    break;
                }
            }
            if (!found) {
                QualifiedTimeWindow newQtw;
                newQtw.ts = ts;
                newQtw.te = te;
                newQtw.vertices.insert(u);
                qtwList.push_back(newQtw);
            }
        }
    }

    // Sort the qualified time windows by start time
    sort(qtwList.begin(), qtwList.end(), [](const QualifiedTimeWindow& a, const QualifiedTimeWindow& b) {
        return a.ts < b.ts;
    });

    return qtwList;
}


unordered_map<pair<int, int>, vector<QualifiedTimeWindow>> buildQueryOptimizedIndex(const vector<tuple<int, int, int>>& edges) {
    unordered_map<pair<int, int>, vector<QualifiedTimeWindow>> index;

    // For each coreness pair, compute the qualified time windows
    for (int alpha = 1; alpha <= maxAlpha; ++alpha) {
        for (int beta = 1; beta <= maxBeta; ++beta) {
            vector<QualifiedTimeWindow> qtwList = computeQualifiedTimeWindowsForCorenessPair(edges, alpha, beta);
            index[{alpha, beta}] = qtwList;
        }
    }

    return index;
}

set<pair<int, int>> computeDominantCorenessPairs(const vector<tuple<int, int, int>>& edges, int u, int ts, int te) {
    unordered_map<int, int> degree;
    set<pair<int, int>> dcp;
    for (const auto& [u, v, t] : edges) {
        if (t >= ts && t <= te && (u == u || v == u)) {
            degree[u]++;
        }
    }
    int te_ = ts;
    while (te_ <= te) {
        for (const auto& [u, v, t] : edges) {
            if (t == te_ && (u == u || v == u)) {
                degree[u]++;
            }
        }
        if (degree[u] >= alpha && degree[u] >= beta) {
            dcp.insert({alpha, beta});
        }
        te_++;
    }
}

unordered_map<int, set<pair<int, int>>> computeDominantCorenessHubs(const vector<tuple<int, int, int>>& edges, const set<int>& vertices, int ts, int te) {
    unordered_map<int, set<pair<int, int>>> dcp;
    unordered_map<int, set<pair<int, int>>> cp;

    // Compute coreness pairs for each vertex
    for (int u : vertices) {
        cp[u] = computeDominantCorenessPairs(edges, u, ts, te);
    }

    // Filter dominant coreness pairs for upper layer vertices
    for (int u : vertices) {
        int beta_m = 0;
        for (const auto& [alpha, beta] : cp[u]) {
            beta_m = max(beta_m, beta);
        }
        dcp[u].insert({alpha, beta_m});
    }

    // Filter dominant coreness pairs for lower layer vertices
    for (int v : vertices) {
        int beta_m = 0;
        for (const auto& [alpha, beta] : cp[v]) {
            beta_m = max(beta_m, beta);
        }
        dcp[v].insert({alpha, beta_m});
    }
}


void linkDominantCorenessHubs(unordered_map<pair<int, int>, vector<QualifiedTimeWindow>>& index) {
    for (auto& [hub, qtwList] : index) {
        int alpha = hub.first, beta = hub.second;
        int alpha_out = alpha, beta_out = beta;
        int alpha_in = alpha, beta_in = beta;
        bool sFlag_out = true, sFlag_in = true;

        // DirectLink_out for l_out and r_out
        while (beta_out <= beta_m) {
            beta_out++;
            if (index.find({alpha, beta_out}) != index.end()) {
                qtwList.front().links.insert({alpha, beta_out});
                index[{alpha, beta_out}].front().links.insert(hub);
                sFlag_out = false;
                break;
            }
        }
        if (sFlag_out) {
            while (alpha_out <= alpha_m) {
                alpha_out++;
                if (index.find({alpha_out, beta}) != index.end()) {
                    qtwList.front().links.insert({alpha_out, beta});
                    index[{alpha_out, beta}].front().links.insert(hub);
                    sFlag_out = false;
                    break;
                }
            }
        }

        // DirectLink_in for l_in and r_in
        while (beta_in <= beta_m) {
            beta_in++;
            if (index.find({alpha, beta_in}) != index.end()) {
                qtwList.front().links.insert({alpha, beta_in});
                index[{alpha, beta_in}].front().links.insert(hub);
                sFlag_in = false;
                break;
            }
        }
        if (sFlag_in) {
            while (alpha_in <= alpha_m) {
                alpha_in++;
                if (index.find({alpha_in, beta}) != index.end()) {
                    qtwList.front().links.insert({alpha_in, beta});
                    index[{alpha_in, beta}].front().links.insert(hub);
                    sFlag_in = false;
                    break;
                }
            }
        }

        // SkipLink_out for m_out
        if (sFlag_out) {
            while (beta_out <= beta_m) {
                beta_out++;
                while (alpha_out <= alpha_m) {
                    alpha_out++;
                    if (index.find({alpha_out, beta_out}) != index.end()) {
                        qtwList.front().links.insert({alpha_out, beta_out});
                        index[{alpha_out, beta_out}].front().links.insert(hub);
                        sFlag_out = false;
                        break;
                    }
                }
                if (!sFlag_out) break;
            }
        }

        // SkipLink_in for m_in
        if (sFlag_in) {
            while (beta_in <= beta_m) {
                beta_in++;
                while (alpha_in <= alpha_m) {
                    alpha_in++;
                    if (index.find({alpha_in, beta_in}) != index.end()) {
                        qtwList.front().links.insert({alpha_in, beta_in});
                        index[{alpha_in, beta_in}].front().links.insert(hub);
                        sFlag_in = false;
                        break;
                    }
                }
                if (!sFlag_in) break;
            }
        }
    }
}


unordered_map<pair<int, int>, vector<QualifiedTimeWindow>> buildSuperiorOptimizedIndex(const vector<tuple<int, int, int>>& edges) {
    unordered_map<pair<int, int>, vector<QualifiedTimeWindow>> index;
    set<int> vertices;

    // Extract all vertices
    for (const auto& [u, v, t] : edges) {
        vertices.insert(u);
        vertices.insert(v);
    }

    // Build index for each vertex
    for (int u : vertices) {
        set<pair<int, int>> dcp = computeDominantCorenessPairs(edges, u, 1, tmax);
        for (const auto& [alpha, beta] : dcp) {
            vector<QualifiedTimeWindow> qtwList = computeQualifiedTimeWindowsForCorenessPair(edges, alpha, beta);
            for (auto& qtw : qtwList) {
                qtw.vertices.insert(u);
                index[{alpha, beta}].push_back(qtw);
            }
        }
    }

    // Sort the qualified time windows by start time
    for (auto& [key, qtwList] : index) {
        sort(qtwList.begin(), qtwList.end(), [](const QualifiedTimeWindow& a, const QualifiedTimeWindow& b) {
            return a.ts < b.ts;
        });
    }

    // Link dominant coreness hubs
    linkDominantCorenessHubs(index);

    return index;
}


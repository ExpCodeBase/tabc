#include "indConst.h"
#include "utility.h"
#include <algorithm>
#include <ctime>
#include <iostream>
#include <set>
#include <cmath>
#include <list>

#include <fstream>
#include <unistd.h>


using namespace std;


template<typename T>
ostream& operator<< (ostream& out, const vector<T>& v) {
    out << "[";
    size_t last = v.size() - 1;
    for (size_t i = 0; i < v.size(); ++i) {
        out << v[i];
        if (i != last)
            out << ", ";
    }
    out << "]";
    return out;
}

int maxkcorenumber_test_optimal(BiGraph& g) {
	vector<vid_t> left_vertices_to_be_peeled;
	vector<vid_t> right_vertices_to_be_peeled;

	int left_remain_nodes_num = g.num_v1;
	vector<vid_t> left_remain_nodes; left_remain_nodes.resize(g.num_v1);
	for (int i = 0; i < left_remain_nodes.size(); i++) {
		left_remain_nodes[i] = i;
	}
	int left_remain_nodes_tmp_num = 0;
	vector<vid_t> left_remain_nodes_tmp; left_remain_nodes_tmp.resize(g.num_v1);

	int right_remain_nodes_num = g.num_v2;
	vector<vid_t> right_remain_nodes; right_remain_nodes.resize(g.num_v2);
	for (int i = 0; i < right_remain_nodes.size(); i++) {
		right_remain_nodes[i] = i;
	}
	int right_remain_nodes_tmp_num = 0;
	vector<vid_t> right_remain_nodes_tmp; right_remain_nodes_tmp.resize(g.num_v2);

	int kc = 1;
	for (kc = 1; kc <= g.v2_max_degree + 1; kc++) {
		
		bool stop = true;
		left_remain_nodes_tmp_num = 0;
		for (int i = 0; i < left_remain_nodes_num; i++) {
			vid_t u = left_remain_nodes[i];
			if (!g.left_delete[u]) {
				stop = false;
				left_remain_nodes_tmp[left_remain_nodes_tmp_num] = u;
				left_remain_nodes_tmp_num++;
				if (g.degree_v1[u] < kc) {
					left_vertices_to_be_peeled.push_back(u);
				}
			}
		}
		swap(left_remain_nodes, left_remain_nodes_tmp);
		left_remain_nodes_num = left_remain_nodes_tmp_num;
		if (stop) break;

		stop = true;
		right_remain_nodes_tmp_num = 0;
		for (int i = 0; i < right_remain_nodes_num; i++) {
			vid_t v = right_remain_nodes[i];
			if (!g.right_delete[v]) {
				stop = false;
				right_remain_nodes_tmp[right_remain_nodes_tmp_num] = v;
				right_remain_nodes_tmp_num++;
				if (g.degree_v2[v] < kc) {
					right_vertices_to_be_peeled.push_back(v);
				}
			}
		}
		swap(right_remain_nodes, right_remain_nodes_tmp);
		right_remain_nodes_num = right_remain_nodes_tmp_num;
		if (stop) break;

		while (!left_vertices_to_be_peeled.empty() || !right_vertices_to_be_peeled.empty()) {
			// peel left
			for (auto j = left_vertices_to_be_peeled.begin(); j != left_vertices_to_be_peeled.end(); j++) {
				vid_t u = *j;
				if (g.left_delete[u]) continue;
				for (int k = 0; k < g.neighbor_v1[u].size(); k++) {
					vid_t v = g.neighbor_v1[u][k];
					if (g.right_delete[v]) continue;
					g.degree_v2[v]--;
					//g.degree_v1[u]--;
					if (g.degree_v2[v] == 0) {
						g.right_delete[v] = true;
					}
					if (g.degree_v2[v] < kc) {
						right_vertices_to_be_peeled.push_back(v);
					}
				}
				g.degree_v1[u] = 0;
				g.left_delete[u] = true;
			}
			left_vertices_to_be_peeled.clear();
			// peel right
			for (auto j = right_vertices_to_be_peeled.begin(); j != right_vertices_to_be_peeled.end(); j++) {
				vid_t v = *j;
				if (g.right_delete[v]) continue;

				/*int effictive_neigh_number = g.degree_v2[v];
				vector<vid_t> tmp_neigh; tmp_neigh.resize(effictive_neigh_number);
				vector<int> swap_candidtate; swap_candidtate.resize(effictive_neigh_number);
				int add_index = 0;
				int swap_index = 0;
				int iii = 0;
				for (int k = 0; k < g.neighbor_v2[v].size(); k++) {
				vid_t u = g.neighbor_v2[v][k];
				if (g.left_delete[u]) {
				if (k < effictive_neigh_number) swap_candidtate[add_index] = k;
				add_index++;
				continue;
				}
				tmp_neigh[iii] = u;
				if (k >= effictive_neigh_number) {
				swap(g.neighbor_v2[v][swap_candidtate[swap_index]], g.neighbor_v2[v][k]);
				swap_index++;
				}
				iii++;
				if (iii >= effictive_neigh_number) break;
				}*/


				for (int k = 0; k < g.neighbor_v2[v].size(); k++) {
					vid_t u = g.neighbor_v2[v][k];
					if (g.left_delete[u]) continue;
					//g.degree_v2[v]--;
					g.degree_v1[u]--;
					if (g.degree_v1[u] == 0) {
						g.left_delete[u] = true;
					}
					if (g.degree_v1[u] < kc) {
						left_vertices_to_be_peeled.push_back(u);
					}
				}
				g.degree_v2[v] = 0;
				g.right_delete[v] = true;
			}
			right_vertices_to_be_peeled.clear();
		}
	}
	// restore g
	fill_n(g.left_delete.begin(), g.left_delete.size(), false);
	for (vid_t u = 0; u < g.num_v1; u++) {
		g.degree_v1[u] = g.neighbor_v1[u].size();
	}
	fill_n(g.right_delete.begin(), g.right_delete.size(), false);
	for (vid_t v = 0; v < g.num_v2; v++) {
		g.degree_v2[v] = g.neighbor_v2[v].size();
	}
	return kc - 2;
}

void print_index_detail(BiGraph& g) {
	int q1 = 0, q2 =0; // count # of skylines
	for (vid_t u = 0; u < g.num_v1; u++) {
		cout << "u" << u + 1 << " : ";
		for (int i = 1; i < g.left_index[u].size(); i++) {
			cout << g.left_index[u][i] << " ";
		}
		q1+=g.left_index[u].size()-1;
		cout << q1 << endl;
	}
	for (vid_t v = 0; v < g.num_v2; v++) {
		cout << "v" << v + 1 << " : ";
		for (int i = 1; i < g.right_index[v].size(); i++) {
			cout << g.right_index[v][i] << " ";
		}
		q2+=g.right_index[v].size()-1;
		cout << q2 << endl;
	}
}

void binsort_paper(vector<list<vid_t>>& left_bin, vector<list<vid_t>>& right_bin,
	vector<list<vid_t>::iterator>& left_list_pointer, vector<list<vid_t>::iterator>& right_list_pointer, BiGraph& tmp) {
	left_bin.clear(); right_bin.clear(); left_list_pointer.clear(); right_list_pointer.clear();
	left_bin.resize(tmp.v1_max_degree + 1);
	left_list_pointer.resize(tmp.getV1Num());
	for (int i = 0; i < tmp.getV1Num(); i++) {
		left_bin[tmp.getV1Degree(i)].push_back(i);
		left_list_pointer[i] = left_bin[tmp.getV1Degree(i)].end();
		left_list_pointer[i]--;
	}
	right_list_pointer.resize(tmp.getV2Num());
	right_bin.resize(tmp.v2_max_degree + 1);
	for (int i = 0; i < tmp.getV2Num(); i++) {
		right_bin[tmp.getV2Degree(i)].push_back(i);
		right_list_pointer[i] = right_bin[tmp.getV2Degree(i)].end();
		right_list_pointer[i]--;
	}
}

void reorder_left_paper(vector<list<vid_t>>& bin, vector<list<vid_t>::iterator>& list_pointer, int new_degree, int old_degree, vid_t vertex) {
	bin[old_degree].erase(list_pointer[vertex]);
	bin[new_degree].push_back(vertex);
	list_pointer[vertex] = bin[new_degree].end();
	list_pointer[vertex]--;
}

void reorder_right_paper(vector<list<vid_t>>& bin, vector<list<vid_t>::iterator>& list_pointer, int new_degree, int old_degree, vid_t vertex) {
	bin[old_degree].erase(list_pointer[vertex]);
	bin[new_degree].push_back(vertex);
	list_pointer[vertex] = bin[new_degree].end();
	list_pointer[vertex]--;
}

void crossUpdate(BiGraph& g, int left_k, int k_x, vid_t v) {
	for (int right_k = 1; right_k < g.right_index[v].size(); right_k++) {
		if (right_k <= k_x) {
			if (g.right_index[v][right_k] < left_k) {
				g.right_index[v][right_k] = left_k;
			}
		}
		else {
			break;
		}
	}
}

bool compare_deg_array(const pair<int, int>& p1, const pair<int, int>& p2) {
	return p1.second < p2.second;
}

void alphaCopyPeel(int left_k, BiGraph& tmp, BiGraph& g) {
	BiGraph newtmp; int ts = 0;
	vector<list<vid_t>> left_bin;
	vector<list<vid_t>::iterator> left_list_pointer;
	vector<list<vid_t>> right_bin;
	vector<list<vid_t>::iterator> right_list_pointer;
	binsort_paper(left_bin, right_bin, left_list_pointer, right_list_pointer, tmp);
	for (int right_k = 1; right_k <= g.v2_max_degree + 1; right_k++) {
		bool left_unchange = false;
		bool right_unchange = false;
		while (!left_unchange || !right_unchange) {
			left_unchange = true;
			right_unchange = true;
			// peel left
			for (int i = 1; i < left_k; i++) {
				for (auto j = left_bin[i].begin(); j != left_bin[i].end();) {
					vid_t u = *j;
					vector<vid_t> neigh = tmp.getV1Neighbors(u);
					int old_degree = tmp.getV1Degree(u); // should equal to i
					for (int k = 0; k < old_degree; k++) {
						vid_t v = neigh[k];
						int old_deg = tmp.getV2Degree(v); // should equal to tmp.getV2Degree(v) + 1
						tmp.deleteEdge(u, v, ts);
						reorder_right_paper(right_bin, right_list_pointer, tmp.getV2Degree(v), old_deg, v);
						// degree = 0 will not be considered in next round
						if (tmp.getV2Degree(v) == 0 && right_k - 1 > 0) {
							crossUpdate(g, left_k, right_k - 1, v);
						}
					}
					++j; // j becomes unavailable after reordering
					reorder_left_paper(left_bin, left_list_pointer, tmp.getV1Degree(u), old_degree, u);
					if (right_k - 1 > 0) {
						g.left_index[u][left_k] = right_k - 1;
					}
					left_unchange = false;
				}
			}
			// peel right
			for (int i = 1; i < right_k; i++) {
				for (auto j = right_bin[i].begin(); j != right_bin[i].end(); ) {
					vid_t v = *j;
					vector<vid_t> neigh = tmp.getV2Neighbors(v);
					int old_degree = tmp.getV2Degree(v);
					for (int k = 0; k < old_degree; k++) {
						vid_t u = neigh[k];
						int old_deg = tmp.getV1Degree(u);
						tmp.deleteEdge(u, v, ts);
						reorder_left_paper(left_bin, left_list_pointer, tmp.getV1Degree(u), old_deg, u);
						// degree = 0 will not be considered in next round
						if (tmp.getV1Degree(u) == 0 && right_k - 1 > 0) {
							g.left_index[u][left_k] = right_k - 1;
						}
					}
					++j;
					reorder_right_paper(right_bin, right_list_pointer, tmp.getV2Degree(v), old_degree, v);
					if (right_k - 1 > 0) {
						crossUpdate(g, left_k, right_k - 1, v);
					}
					right_unchange = false;
				}
			}
		}
		if (right_k == 1) {
			newtmp = tmp;
		}
	}
	tmp = newtmp;
}

void __alphaCopyPeel(int left_k, BiGraph& g) {
	vector<bool> left_deletion_next_round;
	vector<bool> right_deletion_next_round;
	vector<int> left_degree_next_round;
	vector<int> right_degree_next_round;
	vector<vid_t> left_vertices_to_be_peeled;
	vector<vid_t> right_vertices_to_be_peeled;
	// definition of dyn array
	vector<int> right_k_x_index; right_k_x_index.resize(g.v2_max_degree + 2);
	fill_n(right_k_x_index.begin(), right_k_x_index.size(), -1); right_k_x_index[0] = 0; right_k_x_index[g.v2_max_degree + 1] = g.num_v2;
	vector<int> right_vertices_index; right_vertices_index.resize(g.num_v2);
	vector<pair<int, int>> degree_array; degree_array.resize(g.num_v2);
	// end of definition
	for (vid_t u = 0; u < g.getV1Num(); u++) {
		if (g.degree_v1[u] < left_k && !g.left_delete[u]) {
			left_vertices_to_be_peeled.push_back(u);
		}
	}
	// initialize dyn array
	for (vid_t v = 0; v < g.num_v2; v++) {
		degree_array[v] = make_pair(v, g.degree_v2[v]);
	}
	sort(degree_array.begin(), degree_array.end(), compare_deg_array);
	for (int i = 0; i < degree_array.size(); i++) {
		if (right_k_x_index[degree_array[i].second] == -1) {
			right_k_x_index[degree_array[i].second] = i;
		}
		right_vertices_index[degree_array[i].first] = i;
	}
	for (int i = right_k_x_index.size() - 1; i >= 0; i--) {
		if (right_k_x_index[i] == -1) {
			right_k_x_index[i] = right_k_x_index[i + 1];
		}
	}
	// end of initialization
	for (int right_k = 1; right_k <= g.v2_max_degree + 1; right_k++) {
		if (right_k_x_index[right_k - 1] == g.num_v2) break;
		for (int i = right_k_x_index[right_k - 1]; i < right_k_x_index[right_k]; i++) {
			right_vertices_to_be_peeled.push_back(degree_array[i].first);
		}
		while (!left_vertices_to_be_peeled.empty() || !right_vertices_to_be_peeled.empty()) {
			// peel left
			for (auto j = left_vertices_to_be_peeled.begin(); j != left_vertices_to_be_peeled.end(); j++) {
				vid_t u = *j;
				if (g.left_delete[u]) continue;
				for (int k = 0; k < g.neighbor_v1[u].size(); k++) {
					vid_t v = g.neighbor_v1[u][k];
					if (g.right_delete[v]) continue;
					g.degree_v2[v]--;
					g.degree_v1[u]--;
					if (g.getV2Degree(v) == 0 && right_k - 1 > 0) {
						crossUpdate(g, left_k, right_k - 1, v);
						g.right_delete[v] = true;
					}
					if (g.degree_v2[v] < right_k && !g.right_delete[v]) {
						right_vertices_to_be_peeled.push_back(v);
					}
					if (g.degree_v2[v] >= right_k - 1) {
						int olddegree = g.degree_v2[v] + 1;
						vid_t stack = degree_array[right_k_x_index[olddegree]].first;
						swap(degree_array[right_k_x_index[olddegree]], degree_array[right_vertices_index[v]]);
						swap(right_vertices_index[stack], right_vertices_index[v]);
						right_k_x_index[olddegree]++;
					}
				}
				g.left_delete[u] = true;
				if (right_k - 1 > 0) {
					g.left_index[u][left_k] = right_k - 1;
				}
			}
			left_vertices_to_be_peeled.clear();
			// peel right
			for (auto j = right_vertices_to_be_peeled.begin(); j != right_vertices_to_be_peeled.end(); j++) {
				vid_t v = *j;
				if (g.right_delete[v]) continue;
				for (int k = 0; k < g.neighbor_v2[v].size(); k++) {
					vid_t u = g.neighbor_v2[v][k];
					if (g.left_delete[u]) continue;
					g.degree_v2[v]--;
					g.degree_v1[u]--;
					if (g.getV1Degree(u) == 0 && right_k - 1 > 0) {
						g.left_index[u][left_k] = right_k - 1;
						g.left_delete[u] = true;
					}
					if (g.degree_v1[u] < left_k && !g.left_delete[u]) {
						left_vertices_to_be_peeled.push_back(u);
					}
				}
				g.right_delete[v] = true;
				if (right_k - 1 > 0) {
					crossUpdate(g, left_k, right_k - 1, v);
				}
			}
			right_vertices_to_be_peeled.clear();
		}
		if (right_k == 1) {
			left_degree_next_round = g.degree_v1;
			right_degree_next_round = g.degree_v2;
			left_deletion_next_round = g.left_delete;
			right_deletion_next_round = g.right_delete;
		}
	}
	g.degree_v1 = left_degree_next_round;
	g.degree_v2 = right_degree_next_round;
	g.left_delete = left_deletion_next_round;
	g.right_delete = right_deletion_next_round;
	g.v1_max_degree = 0;
	g.v2_max_degree = 0;
	for (vid_t u = 0; u < g.degree_v1.size(); u++) {
		if (g.v1_max_degree < g.degree_v1[u]) g.v1_max_degree = g.degree_v1[u];
	}
	for (vid_t v = 0; v < g.degree_v2.size(); v++) {
		if (g.v2_max_degree < g.degree_v2[v]) g.v2_max_degree = g.degree_v2[v];
	}
}

void crossUpdate_for_kcore(BiGraph& g, int alpha, int k_x, vid_t v) {
	for (int beta = k_x; beta > 0; beta--) {
		if (g.right_index[v][beta] < alpha) {
			g.right_index[v][beta] = alpha;
		}
		else {
			break;
		}
	}
}

void alphaCopyPeel_for_kcore(int left_k, BiGraph& g) {
	int dd_;
	int pre_left_k_ = left_k - 1;
	vector<bool> left_deletion_next_round;
	vector<bool> right_deletion_next_round;
	vector<int> left_degree_next_round;
	vector<int> right_degree_next_round;
	vector<vid_t> left_vertices_to_be_peeled;
	vector<vid_t> right_vertices_to_be_peeled;
	for (vid_t u = 0; u < g.getV1Num(); u++) {
		if (g.degree_v1[u] < left_k && !g.left_delete[u]) {
			left_vertices_to_be_peeled.push_back(u);
		}
	}
	int right_remain_nodes_num = g.num_v2;
	vector<vid_t> right_remain_nodes; right_remain_nodes.resize(g.num_v2);
	for (int i = 0; i < right_remain_nodes.size(); i++) {
		right_remain_nodes[i] = i;
	}
	int right_remain_nodes_tmp_num = 0;
	vector<vid_t> right_remain_nodes_tmp; right_remain_nodes_tmp.resize(g.num_v2);
	bool update_flag = false;
	for (int right_k = 1; right_k <= g.v2_max_degree + 1; right_k++) {
		if (right_k - 1 > 0) {
			update_flag = true;
		}
		int pre_ = right_k - 1;
		bool stop = true;
		right_remain_nodes_tmp_num = 0;
		for (int i = 0; i < right_remain_nodes_num; i++) {
			vid_t v = right_remain_nodes[i];
			if (!g.right_delete[v]) {
				stop = false;
				right_remain_nodes_tmp[right_remain_nodes_tmp_num] = v;
				right_remain_nodes_tmp_num++;
				if (g.degree_v2[v] < right_k) {
					right_vertices_to_be_peeled.push_back(v);
				}
			}
		}
		swap(right_remain_nodes, right_remain_nodes_tmp);
		right_remain_nodes_num = right_remain_nodes_tmp_num;
		if (stop) break;
		while (!left_vertices_to_be_peeled.empty() || !right_vertices_to_be_peeled.empty()) {
			// peel left
			int oo_ = left_vertices_to_be_peeled.size();
			for (int j = 0; j < oo_; j++) {
				vid_t u = left_vertices_to_be_peeled[j];
				if (g.left_delete[u]) continue;
				vector<vid_t>& tmp_neigh_ = g.neighbor_v1[u];
				int ss = tmp_neigh_.size();
				for (int k = 0; k < ss; k++) {
					vid_t v = tmp_neigh_[k];
					if (g.right_delete[v]) continue;
					dd_ = --g.degree_v2[v];
					if (update_flag && dd_ == 0) {
						crossUpdate_for_kcore(g, left_k, pre_, v);
						g.right_delete[v] = true;
					}
					if (dd_ == pre_) {
						right_vertices_to_be_peeled.push_back(v);
					}
				}
				g.degree_v1[u] = 0;
				g.left_delete[u] = true;
				if (update_flag) {
					g.left_index[u][left_k] = pre_;
				}
			}
			left_vertices_to_be_peeled.clear();
			// peel right
			oo_ = right_vertices_to_be_peeled.size();
			for (int j = 0; j < oo_; j++) {
				vid_t v = right_vertices_to_be_peeled[j];
				if (g.right_delete[v]) continue;
				vector<vid_t>& tmp_neigh_ = g.neighbor_v2[v];
				int ss = tmp_neigh_.size();
				for (int k = 0; k < ss; k++) {
					vid_t u = tmp_neigh_[k];
					if (g.left_delete[u]) continue;
					dd_ = --g.degree_v1[u];
					if (update_flag && dd_ == 0) {
						g.left_index[u][left_k] = pre_;
						g.left_delete[u] = true;
					}
					if (dd_ == pre_left_k_) {
						left_vertices_to_be_peeled.push_back(u);
					}
				}
				g.degree_v2[v] = 0;
				g.right_delete[v] = true;
				if (update_flag) {
					crossUpdate_for_kcore(g, left_k, pre_, v);
				}
			}
			right_vertices_to_be_peeled.clear();
		}
		if (right_k == 1) {
			left_degree_next_round = g.degree_v1;
			right_degree_next_round = g.degree_v2;
			left_deletion_next_round = g.left_delete;
			right_deletion_next_round = g.right_delete;
		}
	}
	g.degree_v1 = left_degree_next_round;
	g.degree_v2 = right_degree_next_round;
	g.left_delete = left_deletion_next_round;
	g.right_delete = right_deletion_next_round;
	g.v1_max_degree = 0;
	g.v2_max_degree = 0;
	for (vid_t u = 0; u < g.degree_v1.size(); u++) {
		if (g.v1_max_degree < g.degree_v1[u]) g.v1_max_degree = g.degree_v1[u];
	}
	for (vid_t v = 0; v < g.degree_v2.size(); v++) {
		if (g.v2_max_degree < g.degree_v2[v]) g.v2_max_degree = g.degree_v2[v];
	}
}

void coreDomComp(BiGraph& g) { // print_out ver.
	int left_degree_max = 0;
	int m = 0;
	for (int i = 0; i < g.getV1Num(); i++) {
		if (left_degree_max < g.getV1Degree(i)) left_degree_max = g.getV1Degree(i);
		m += g.getV1Degree(i);
	}
	int right_degree_max = 0;
	for (int i = 0; i < g.getV2Num(); i++) {
		if (right_degree_max < g.getV2Degree(i)) right_degree_max = g.getV2Degree(i);
	}
	// init g's max degree and index
	g.v1_max_degree = left_degree_max;
	g.v2_max_degree = right_degree_max;
	g.left_index.resize(g.getV1Num());
	g.right_index.resize(g.getV2Num());
	g.left_delete.resize(g.getV1Num());
	g.right_delete.resize(g.getV2Num());
	fill_n(g.left_delete.begin(), g.left_delete.size(), false);
	fill_n(g.right_delete.begin(), g.right_delete.size(), false);
	for (int i = 0; i < g.getV1Num(); i++) {
		g.left_index[i].resize(g.getV1Degree(i) + 1);
		fill_n(g.left_index[i].begin(), g.left_index[i].size(), 0);
	}
	for (int i = 0; i < g.getV2Num(); i++) {
		g.right_index[i].resize(g.getV2Degree(i) + 1);
		fill_n(g.right_index[i].begin(), g.right_index[i].size(), 0);
	}
	for (int left_k = 1; left_k <= g.v1_max_degree; left_k++) {
		__alphaCopyPeel(left_k, g);
		if (PRINT_INDEX_DETAIL) {
			print_index_detail(g);
			cout << endl;
		}
	}
	
	// skyline computation
	
	vector<int> rtmp; // store the skyline label to be pushed into q1, q2;
	vector<vector<int>> q1, q2; // store the skyline array for each u or v;
	int uSky = 0, vSky = 0;
	for (vid_t u = 0; u < g.num_v1; u++) {
		cout << "u" << u + 1 << " : ";
		for (int i = 2; i < g.left_index[u].size(); i++) {
			if (g.left_index[u][i-1]>g.left_index[u][i]){
				rtmp.push_back(i-1);
				rtmp.push_back(g.left_index[u][i-1]);
				q1.push_back(rtmp);
				rtmp.clear();
			}
		}
		rtmp.push_back(g.left_index[u].size()-1);
		rtmp.push_back(g.left_index[u][g.left_index[u].size()-1]);
		q1.push_back(rtmp);
		cout << q1 << endl;
		uSky += q1.size();
		rtmp.clear();
		q1.clear();
	}
	for (vid_t v = 0; v < g.num_v2; v++) {
		cout << "v" << v + 1 << " : ";
		for (int i = 2; i < g.right_index[v].size(); i++) {
			if (g.right_index[v][i-1]>g.right_index[v][i]){
				
				rtmp.push_back(g.right_index[v][i-1]);
				rtmp.push_back(i-1);
				q2.push_back(rtmp);
				rtmp.clear();
			}
		}
		
		rtmp.push_back(g.right_index[v][g.right_index[v].size()-1]);
		rtmp.push_back(g.right_index[v].size()-1);
		q2.push_back(rtmp);
		cout << q2 << endl;
		vSky += q2.size();
		rtmp.clear();
		q2.clear();
	}
	cout << "# of skylines for upper layer: " << uSky << " , # of skylines for lower layer: " << vSky << endl;


	// restore g
	fill_n(g.left_delete.begin(), g.left_delete.size(), false);
	g.v1_max_degree = left_degree_max;
	for (vid_t u = 0; u < g.num_v1; u++) {
		g.degree_v1[u] = g.neighbor_v1[u].size();
	}
	fill_n(g.right_delete.begin(), g.right_delete.size(), false);
	g.v2_max_degree = right_degree_max;
	for (vid_t v = 0; v < g.num_v2; v++) {
		g.degree_v2[v] = g.neighbor_v2[v].size();
	}
}

void domIndexDelta(BiGraph& g, unordered_map<string, vector<vector<int>>>& skyLable, int &delta) {
	cout << "Decomposition start..." << "\n";
	
	int left_degree_max = 0;
	for (int i = 0; i < g.getV1Num(); i++) {
		if (left_degree_max < g.getV1Degree(i)) left_degree_max = g.getV1Degree(i);
	}
	int right_degree_max = 0;
	for (int i = 0; i < g.getV2Num(); i++) {
		if (right_degree_max < g.getV2Degree(i)) right_degree_max = g.getV2Degree(i);
	}
	// init g's max degree and index
	g.v1_max_degree = left_degree_max;
	g.v2_max_degree = right_degree_max;
	g.left_index.resize(g.getV1Num());
	g.right_index.resize(g.getV2Num());
	g.left_delete.resize(g.getV1Num());
	g.right_delete.resize(g.getV2Num());
	fill_n(g.left_delete.begin(), g.left_delete.size(), false);
	fill_n(g.right_delete.begin(), g.right_delete.size(), false);
	for (int i = 0; i < g.getV1Num(); i++) {
		g.left_index[i].resize(g.getV1Degree(i) + 1);
		fill_n(g.left_index[i].begin(), g.left_index[i].size(), 0);
	}
	for (int i = 0; i < g.getV2Num(); i++) {
		g.right_index[i].resize(g.getV2Degree(i) + 1);
		fill_n(g.right_index[i].begin(), g.right_index[i].size(), 0);
	}
	//int km = maxkcorenumber_test_optimal(g);
	//cout << "max kcore number: " << km << endl;
	int beta_s = 0;
	for (int left_k = 1; left_k <= g.v1_max_degree; left_k++) {
		alphaCopyPeel_for_kcore(left_k, g);
		if (PRINT_INDEX_DETAIL) {
			print_index_detail(g);
			cout << endl;
		}
		beta_s = 0;
		for (vid_t u = 0; u < g.num_v1; u++) {
			if (g.degree_v1[u] <= left_k) continue;
			int right_k = g.left_index[u][left_k];
			if (beta_s < right_k) beta_s = right_k;
		}
		if (beta_s < left_k){
			beta_s = left_k-1;
			break;
		}
	}
	// restore g
	fill_n(g.left_delete.begin(), g.left_delete.size(), false);
	g.v1_max_degree = left_degree_max;
	for (vid_t u = 0; u < g.num_v1; u++) {
		g.degree_v1[u] = g.neighbor_v1[u].size();
	}
	fill_n(g.right_delete.begin(), g.right_delete.size(), false);
	g.v2_max_degree = right_degree_max;
	for (vid_t v = 0; v < g.num_v2; v++) {
		g.degree_v2[v] = g.neighbor_v2[v].size();
	}

	if (PRINT_INDEX_DETAIL) {
		cout << "inverse" << endl;
		cout << endl;
	}
	inv(g);
	for (int left_k = 1; left_k <= beta_s; left_k++) {
		alphaCopyPeel_for_kcore(left_k, g);
		if (PRINT_INDEX_DETAIL) {
			inv(g);
			print_index_detail(g);
			cout << endl;
			inv(g);
		}
	}
	inv(g);
	// restore g
	fill_n(g.left_delete.begin(), g.left_delete.size(), false);
	g.v1_max_degree = left_degree_max;
	for (vid_t u = 0; u < g.num_v1; u++) {
		g.degree_v1[u] = g.neighbor_v1[u].size();
	}
	fill_n(g.right_delete.begin(), g.right_delete.size(), false);
	g.v2_max_degree = right_degree_max;
	for (vid_t v = 0; v < g.num_v2; v++) {
		g.degree_v2[v] = g.neighbor_v2[v].size();
	}

	// testing

	cout << g.v1_max_degree << " " << g.v2_max_degree << endl;
	cout << "d_max: " << max(g.v1_max_degree,g.v2_max_degree)  << endl;
	cout << "delta: " << beta_s << endl;
	

	// cout << "----------------------------------" << "\n";
	// cout << "Skyline lables start..." << "\n";

	// vector<int> rtmp;
	// vector<vector<int>> q1, q2;
	// string vertex;

	// int uSky = 0, vSky = 0;
	// for (vid_t u = 0; u < g.num_v1; u++) {
	// 	vertex = "u";
	// 	vertex += to_string(u+1);
		
	// 	for (int i = 2; i < g.left_index[u].size(); i++) {
	// 		if (g.left_index[u][i-1]>g.left_index[u][i]){
	// 			rtmp.push_back(i-1);
	// 			rtmp.push_back(g.left_index[u][i-1]);
	// 			q1.push_back(rtmp);
	// 			rtmp.clear();
	// 		}
	// 	}
	// 	rtmp.push_back(g.left_index[u].size()-1);
	// 	rtmp.push_back(g.left_index[u][g.left_index[u].size()-1]);
	// 	q1.push_back(rtmp);
	// 	skyLable.emplace(vertex, q1);
		
	// 	uSky += q1.size();
	// 	rtmp.clear();
	// 	q1.clear();
	// }
	
	// for (vid_t v = 0; v < g.num_v2; v++) {
	// 	vertex = "v";
	// 	vertex += to_string(v+1);
		
	// 	for (int i = 2; i < g.right_index[v].size(); i++) {
	// 		if (g.right_index[v][i-1]>g.right_index[v][i]){
				
	// 			rtmp.push_back(g.right_index[v][i-1]);
	// 			rtmp.push_back(i-1);
	// 			q2.push_back(rtmp);
	// 			rtmp.clear();
	// 		}
	// 	}
		
	// 	rtmp.push_back(g.right_index[v][g.right_index[v].size()-1]);
	// 	rtmp.push_back(g.right_index[v].size()-1);
	// 	q2.push_back(rtmp);
	// 	skyLable.emplace(vertex, q2);
		
	// 	vSky += q2.size();
	// 	rtmp.clear();
	// 	q2.clear();
	// }

	// cout << "----------------------------------" << "\n";
	// cout << "# of skylines for upper layer: " << uSky << " , # of skylines for lower layer: " << vSky << endl;



	delta = beta_s;
}

void build_path_left(vector<int> build_start, 
	BiGraph& g,
	unordered_map<vector<int>, skyline_index_hub*, VectorHasher>& skyline_index_lv0,
	unordered_map<string, vector<skyline_index_hub*>>& skyline_index_v2h,
    unordered_map<string, vector<skyline_index_ccblock*>>& skyline_index_v2ccb, 
	unordered_map<string, skyline_index_ccblock*>& skyline_index_v2par, 
	unordered_map<vector<int>, skyline_index_ccblock*, VectorHasher>& skyline_index_lv1){

	vector<bool> left_pushed; vector<bool> right_pushed;
	vector<bool> left_pushed_glb; vector<bool> right_pushed_glb;

	left_pushed.resize(g.num_v1, false); 
	right_pushed.resize(g.num_v2, false);
	left_pushed_glb.resize(g.num_v1, false); 
	right_pushed_glb.resize(g.num_v2, false);

	vector<int> tar; int ts = 0;
	skyline_index_hub* cur_bd = skyline_index_lv0[build_start];
	skyline_index_hub* cur_lk = skyline_index_lv0[build_start];

	vector<vector<int>> vt_tbre;
	vector<vector<int>> vt_ngp;

	// build based on hub

	// global state labeling
	skyline_index_hub* cur_st = cur_bd;
	while(cur_st){
		for(auto vt: cur_st->nodeset){
			if(vt[0]=='u'){
				vt.erase(vt.begin());
				int vtid = stoi(vt)-1;
				if(!left_pushed_glb[vtid]) left_pushed_glb[vtid] = true;
			} else {
				vt.erase(vt.begin());
				int vtid = stoi(vt)-1;
				if(!right_pushed_glb[vtid]) right_pushed_glb[vtid] = true;
			}
		}
		cur_st = cur_st->right_c;	
	}

	// building
	while(cur_bd){

		fill_n(left_pushed.begin(), left_pushed.size(), false);
		fill_n(right_pushed.begin(), right_pushed.size(), false);

		// delete edges of blocks above
		
		if(cur_bd->left_p){
			for(auto vt: cur_bd->left_p->nodeset){
				if(vt[0]=='u'){
					vt.erase(vt.begin());
					int vtid = stoi(vt)-1;
					for(auto e: g.getV1Neighbors(vtid)){
						vector<int> tmp;
						if(g.isEdge(vtid, e)){
							g.deleteEdge(vtid,e,ts);
							tmp = {vtid, e};
							vt_tbre.push_back(tmp);
						}
					}
				} else {
					vt.erase(vt.begin());
					int vtid = stoi(vt)-1;
					for(auto e: g.getV2Neighbors(vtid)){
						vector<int> tmp;
						if(g.isEdge(e, vtid)){
							g.deleteEdge(e,vtid,ts);
							tmp = {e, vtid};
							vt_tbre.push_back(tmp);
						}
					}
				}
			}
		}
		
		tar = cur_bd->hubid;

		if(tar.size()<3) {
			tar.push_back(0);
		} else {
			tar[2] = 0;
		}

		// state labeling of vertices in nodeset
		for(auto vt: cur_bd->nodeset){
			if(vt[0]=='u'){
				vt.erase(vt.begin());
				int vtid = stoi(vt)-1;
				if(!left_pushed[vtid]) left_pushed[vtid] = true;
			} else {
				vt.erase(vt.begin());
				int vtid = stoi(vt)-1;
				if(!right_pushed[vtid]) right_pushed[vtid] = true;
			}
		}
		

		queue<string> ccQue;
		string now;
		
		// ccQue for BFS to build each tar with cc_id
		for(auto vt: cur_bd->nodeset){
			
			if(vt[0]=='u'){
				vt.erase(vt.begin());
				int vtid = stoi(vt)-1;
				if(left_pushed[vtid]){
					vt = "u" + vt;
					ccQue.push(vt);
				}
			} else {
				vt.erase(vt.begin());
				int vtid = stoi(vt)-1;
				if(right_pushed[vtid]){
					vt = "v" + vt;
					ccQue.push(vt);
				}
			}

			// stopFlag for ccid iteration
			bool stopFlag = true;
			bool tightFlag = false;
			bool closeFlag = true;
			int minUntight = g.v1_max_degree;
			string untight;
			
			while(!ccQue.empty()){

				now = ccQue.front();
				ccQue.pop();

				if(!skyline_index_lv1[tar]) skyline_index_lv1[tar] = new skyline_index_ccblock;

				int vtid = 0;
				
				if(now[0]=='u'){
					now.erase(now.begin());
					vtid = stoi(now)-1;
					string nowPu = "u" + now;
					if(left_pushed[vtid]){
						left_pushed[vtid] = false;
						left_pushed_glb[vtid] = false;
					}
					for(auto e: g.getV1Neighbors(vtid)){
						if(right_pushed[e]){
							string nowNei = "v" + to_string(e+1);
							right_pushed[e] = false;
							right_pushed_glb[e] = false;
							ccQue.push(nowNei);
							stopFlag = false;
						}
						else if(right_pushed_glb[e]){
							right_pushed_glb[e] = false;
							string nowNei = "v" + to_string(e+1);
							ccQue.push(nowNei);
							
							stopFlag = false;
						}
					}
				} else {
					now.erase(now.begin());
					vtid = stoi(now)-1;
					string nowPu = "v" + now;
					if(right_pushed[vtid]){
						right_pushed[vtid] = false;
						right_pushed_glb[vtid] = false;
		
					}
					for(auto e: g.getV2Neighbors(vtid)){
						if(left_pushed[e]){
							string nowNei = "u" + to_string(e+1);
							left_pushed[e] = false;
							left_pushed_glb[e] = false;
							ccQue.push(nowNei);
							stopFlag = false;
						}
						else if(left_pushed_glb[e]){
							left_pushed_glb[e] = false;
							string nowNei = "u" + to_string(e+1);
							ccQue.push(nowNei);
							stopFlag = false;
						}
					}
					
				}
			}

			if(!stopFlag){
				tar[2]++;
			}

		}


		// global state labeling
		skyline_index_hub* cur_st1 = cur_bd->right_c;
		while(cur_st1){
			for(auto vt: cur_st1->nodeset){
				if(vt[0]=='u'){
					vt.erase(vt.begin());
					int vtid = stoi(vt)-1;
					if(!left_pushed_glb[vtid]) left_pushed_glb[vtid] = true;
				} else {
					vt.erase(vt.begin());
					int vtid = stoi(vt)-1;
					if(!right_pushed_glb[vtid]) right_pushed_glb[vtid] = true;
				}
			}
			cur_st1 = cur_st1->right_c;	
		}


		// jumping to next level
		cur_bd = cur_bd->right_c;

	}

	// restore edges for next path 

	for(auto e: vt_tbre){
		g.addEdge(e[0],e[1],ts);
	}
	for(auto e: vt_ngp){
		g.addEdge(e[0],e[1],ts);
	}
	
	vt_tbre.clear();
	vt_ngp.clear();
	tar.clear();
	skyline_index_v2par.clear();

}

void link_path_right(vector<int> link_start,
	BiGraph& g,
	unordered_map<vector<int>, skyline_index_hub*, VectorHasher>& skyline_index_lv0,
	unordered_map<string, vector<skyline_index_hub*>>& skyline_index_v2h,
    unordered_map<string, vector<skyline_index_ccblock*>>& skyline_index_v2ccb, 
	unordered_map<string, skyline_index_ccblock*>& skyline_index_v2par, 
	unordered_map<vector<int>, skyline_index_ccblock*, VectorHasher>& skyline_index_lv1){

	vector<bool> left_pushed; vector<bool> right_pushed;
	vector<bool> left_pushed_glb; vector<bool> right_pushed_glb;

	left_pushed.resize(g.num_v1, false); 
	right_pushed.resize(g.num_v2, false);
	left_pushed_glb.resize(g.num_v1, false); 
	right_pushed_glb.resize(g.num_v2, false);

	vector<int> tar; int ts = 0;
	skyline_index_hub* cur_bd = skyline_index_lv0[link_start];
	skyline_index_hub* cur_lk = skyline_index_lv0[link_start];

	vector<vector<int>> vt_tbre;
	vector<vector<int>> vt_ngp;

	// global state labeling
	skyline_index_hub* cur_st = cur_bd;
	while(cur_st){
		for(auto vt: cur_st->nodeset){
			if(vt[0]=='u'){
				vt.erase(vt.begin());
				int vtid = stoi(vt)-1;
				if(!left_pushed_glb[vtid]) left_pushed_glb[vtid] = true;
			} else {
				vt.erase(vt.begin());
				int vtid = stoi(vt)-1;
				if(!right_pushed_glb[vtid]) right_pushed_glb[vtid] = true;
			}
		}
		cur_st = cur_st->left_c;	
	}

	// building
	while(cur_bd){

		fill_n(left_pushed.begin(), left_pushed.size(), false);
		fill_n(right_pushed.begin(), right_pushed.size(), false);

		// delete edges of blocks above
		
		if(cur_bd->right_p){
			for(auto vt: cur_bd->right_p->nodeset){
				if(vt[0]=='u'){
					vt.erase(vt.begin());
					int vtid = stoi(vt)-1;
					for(auto e: g.getV1Neighbors(vtid)){
						vector<int> tmp;
						if(g.isEdge(vtid, e)){
							g.deleteEdge(vtid,e,ts);
							tmp = {vtid, e};
							vt_tbre.push_back(tmp);
						}
					}
				} else {
					vt.erase(vt.begin());
					int vtid = stoi(vt)-1;
					for(auto e: g.getV2Neighbors(vtid)){
						vector<int> tmp;
						if(g.isEdge(e, vtid)){
							g.deleteEdge(e,vtid,ts);
							tmp = {e, vtid};
							vt_tbre.push_back(tmp);
						}
					}
				}
			}
		}
		// cout << "vt_tbre size: " << vt_tbre.size() << endl;
		
		tar = cur_bd->hubid;
		

		// state labeling of vertices in nodeset
		for(auto vt: cur_bd->nodeset){
			if(vt[0]=='u'){
				vt.erase(vt.begin());
				int vtid = stoi(vt)-1;
				if(!left_pushed[vtid]) left_pushed[vtid] = true;
			} else {
				vt.erase(vt.begin());
				int vtid = stoi(vt)-1;
				if(!right_pushed[vtid]) right_pushed[vtid] = true;
			}
			// cout << vt << endl;
		}
		

		queue<string> ccQue;
		string now;
		
		// ccQue for BFS to build each tar with cc_id
		for(auto vt: cur_bd->nodeset){
			// cout << vt << endl;
			
			if(vt[0]=='u'){
				vt.erase(vt.begin());
				int vtid = stoi(vt)-1;
				if(left_pushed[vtid]){
					vt = "u" + vt;
					ccQue.push(vt);
					for(auto p: skyline_index_v2ccb[vt]){
						if(p->hubid == tar){
							tar.push_back(p->ccid);
							break;
						}
					}
				}
			} else {
				vt.erase(vt.begin());
				int vtid = stoi(vt)-1;
				if(right_pushed[vtid]){
					vt = "v" + vt;
					ccQue.push(vt);
					for(auto p: skyline_index_v2ccb[vt]){
						if(p->hubid == tar){
							tar.push_back(p->ccid);
							break;
						}
					}
				}
			}

			

			// stopFlag for ccid iteration
			// bool stopFlag = true;
			bool tightFlag = false;
			bool closeFlag = true;
			int minUntight = g.v2_max_degree;
			string untight;
			
			
			while(!ccQue.empty()){

				now = ccQue.front();
				ccQue.pop();
				int vtid = 0;

				
				
				if(now[0]=='u'){

					now.erase(now.begin());
					vtid = stoi(now)-1;
					string nowPu = "u" + now;
					if(left_pushed[vtid]){
						left_pushed[vtid] = false;
						left_pushed_glb[vtid] = false;
					}

					for(auto e: g.getV1Neighbors(vtid)){
						if(right_pushed[e]){
							string nowNei = "v" + to_string(e+1);
							right_pushed[e] = false;
							right_pushed_glb[e] = false;
							ccQue.push(nowNei);
						}
						else if(right_pushed_glb[e]){
							right_pushed_glb[e] = false;
							string nowNei = "v" + to_string(e+1);
							ccQue.push(nowNei);

							for(auto e: skyline_index_v2h[nowNei]){
								if(e->hubid == cur_bd->left_c->hubid){
									skyline_index_v2par[nowNei] = skyline_index_lv1[tar];
									tightFlag = true;
									break;
								}
							}

							if(!tightFlag && !skyline_index_v2par[nowNei]){
								for(auto e: skyline_index_v2h[nowNei]){
									if(e->hubid[0] == cur_bd->left_c->hubid[0]){
										if(e->hubid[1] < minUntight){
											minUntight = e->hubid[1];
											untight = nowNei;		
										}
										closeFlag = false;
										break;
									}
								}
							}

						}
					}
				} else {
					now.erase(now.begin());
					vtid = stoi(now)-1;
					string nowPu = "v" + now;
					if(right_pushed[vtid]){
						right_pushed[vtid] = false;
						right_pushed_glb[vtid] = false;
					}

					for(auto e: g.getV2Neighbors(vtid)){
						if(left_pushed[e]){
							string nowNei = "u" + to_string(e+1);
							left_pushed[e] = false;
							left_pushed_glb[e] = false;
							ccQue.push(nowNei);
						}
						else if(left_pushed_glb[e]){
							left_pushed_glb[e] = false;
							string nowNei = "u" + to_string(e+1);
							ccQue.push(nowNei);

							for(auto e: skyline_index_v2h[nowNei]){
								if(e->hubid == cur_bd->left_c->hubid){
									skyline_index_v2par[nowNei] = skyline_index_lv1[tar];
									tightFlag = true;
									break;
								}
							}

							if(!tightFlag && !skyline_index_v2par[nowNei]){
								for(auto e: skyline_index_v2h[nowNei]){
									if(e->hubid[0] == cur_bd->left_c->hubid[0]){
										if(e->hubid[1] < minUntight){
											minUntight = e->hubid[1];
											untight = nowNei;		
										}
										closeFlag = false;
										break;
									}
								}
							}

						}
					}
					
				}
				
				// tar.clear();

			}

			if(!tightFlag && !closeFlag){
				skyline_index_v2par[untight] = skyline_index_lv1[tar];

			}
			
			if(tar.size()==3) tar.pop_back();
			
		}


		// global state labeling
		skyline_index_hub* cur_st2 = cur_bd->left_c;
		while(cur_st2){
			for(auto vt: cur_st2->nodeset){
				if(vt[0]=='u'){
					vt.erase(vt.begin());
					int vtid = stoi(vt)-1;
					if(!left_pushed_glb[vtid]) left_pushed_glb[vtid] = true;
				} else {
					vt.erase(vt.begin());
					int vtid = stoi(vt)-1;
					if(!right_pushed_glb[vtid]) right_pushed_glb[vtid] = true;
				}
			}
			cur_st2 = cur_st2->left_c;	
		}


		// jumping to next level
		cur_bd = cur_bd->left_c;

	}

	
	// link based on v2par

	// cur_lk = cur_lk->left_c;
	while(cur_lk){
		for(auto vt: cur_lk->nodeset){
			if(skyline_index_v2par[vt]){
				for(auto e: skyline_index_v2ccb[vt]){
					if(e->hubid == cur_lk->hubid
					&& !e->right_p){
						e->right_p = skyline_index_v2par[vt];
						skyline_index_v2par[vt]->left_c.push_back(e);
					
					}

				}
			}
				
		}

		cur_lk = cur_lk->left_c;
	}

	// restore edges for next path 
	ts = 0;
	// cout << "number of edges before adding: " << g.num_edges << endl;
	for(auto e: vt_tbre){
		g.addEdge(e[0],e[1],ts);
	}
	for(auto e: vt_ngp){
		g.addEdge(e[0],e[1],ts);
	}
	// cout << "number of edges after adding: " << g.num_edges << endl;
	vt_tbre.clear();
	vt_ngp.clear();
	tar.clear();
	skyline_index_v2par.clear();

}


void build_skyline_index_lv0(BiGraph&g, 
	unordered_map<string, vector<vector<int>>>& skyLable,
	unordered_map<vector<int>, vector<string>, VectorHasher>& skyNode,
	unordered_map<vector<int>, skyline_index_hub*, VectorHasher>& skyline_index_lv0, 
	unordered_map<string, vector<skyline_index_hub*>>& skyline_index_v2h,
	int& delta){

		
	skyline_index_lv0.clear();

	// skyLable to skyNode
	for (const auto &p : skyLable) {
		for(const auto skyid : p.second){
			vector<int> invld_skyid = {0,0};
 			if(skyid == invld_skyid) continue;
			skyNode[skyid].push_back(p.first);
		}
    }
	
	// skyline_index_hub->nodeset constructing from skyNodes
	for(const auto &p: skyNode) {
		if(!skyline_index_lv0[p.first]) skyline_index_lv0[p.first] = new skyline_index_hub;
		skyline_index_lv0[p.first]->hubid = p.first;
		for(const auto vt: p.second){
			skyline_index_v2h[vt].push_back(skyline_index_lv0[p.first]);
			skyline_index_lv0[p.first]->nodeset.push_back(vt);
		}
		
	}


	// skyline_index_hub->pointers linking
	for(const auto &p: skyline_index_lv0) {
		
		int alpha = skyline_index_lv0[p.first]->hubid[0];
		int beta = skyline_index_lv0[p.first]->hubid[1];

		if(! skyline_index_lv0[p.first]->left_c){
			int offset = 1;
			vector<int> tar={alpha};
			while(beta + offset <= g.v2_max_degree){
				if(tar.size() != 1) tar.pop_back();
				// tar.push_back(alpha);
				tar.push_back(beta+offset);
				if(skyline_index_lv0.contains(tar)){
					skyline_index_lv0[p.first]->left_c = skyline_index_lv0[tar];
					if(!skyline_index_lv0[tar]->right_p) skyline_index_lv0[tar]->right_p = skyline_index_lv0[p.first];
					break;
				} else {
					offset++;
				}
			}
		}

		if(! skyline_index_lv0[p.first]->right_c){
			int offset = 1;
			vector<int> tar = {alpha+offset, beta};
			while(alpha + offset <= g.v1_max_degree){
				tar[0] = alpha+offset;
				if(skyline_index_lv0.contains(tar)){
					skyline_index_lv0[p.first]->right_c = skyline_index_lv0[tar];
					if(!skyline_index_lv0[tar]->left_p) skyline_index_lv0[tar]->left_p = skyline_index_lv0[p.first];
					break;
				} else {
					offset++;
				}
			}
		}

		if(! skyline_index_lv0[p.first]->left_p){
			int offset = -1;
			vector<int> tar = {alpha+offset, beta};
			while(alpha + offset > 0){
				tar[0] = alpha+offset;
				if(skyline_index_lv0.contains(tar)){
					skyline_index_lv0[p.first]->left_p = skyline_index_lv0[tar];
					if(!skyline_index_lv0[tar]->right_c) skyline_index_lv0[tar]->right_c = skyline_index_lv0[p.first];
					break;
				} else {
					offset--;
				}
			}
		}

		if(! skyline_index_lv0[p.first]->right_p){
			int offset = -1;
			vector<int> tar = {alpha};
			while(beta + offset > 0){
				if(tar.size()!=1) tar.pop_back();
				tar.push_back(beta+offset);
				if(skyline_index_lv0.contains(tar)){
					skyline_index_lv0[p.first]->right_p = skyline_index_lv0[tar];
					if(!skyline_index_lv0[tar]->left_c) skyline_index_lv0[tar]->left_c = skyline_index_lv0[p.first];
					break;
				} else {
					offset--;
				}
			}
		}

		if(! skyline_index_lv0[p.first]->left_c && ! skyline_index_lv0[p.first]->right_c){
			int offseta = 1, offsetb = 1;
			bool stopflag = false;
			vector<int> tar = {alpha, beta};
			while(alpha + offseta <= g.v1_max_degree){
				tar[0] = alpha+offseta;
				while(beta + offsetb <= g.v2_max_degree){		
					tar[1] = beta+offsetb;
					if(skyline_index_lv0.contains(tar)){
						skyline_index_lv0[p.first]->mid_c = skyline_index_lv0[tar];
						if(!skyline_index_lv0[tar]->mid_p) skyline_index_lv0[tar]->mid_p = skyline_index_lv0[p.first];
						stopflag = true;
						break;
					} else {
						offsetb++;
					}
				}
				if(stopflag){
					break;
				} else {
					offseta++;
					offsetb = 1;
				}

			}

		}


		if(! skyline_index_lv0[p.first]->left_p && ! skyline_index_lv0[p.first]->right_p){
			int offseta = -1, offsetb = -1;
			bool stopflag = false;
			vector<int> tar = {alpha, beta};
			while(alpha + offseta > 0){
				tar[0] = alpha+offseta;
				while(beta + offsetb > 0){		
					tar[1] = beta+offsetb;
					if(skyline_index_lv0.contains(tar)){
						skyline_index_lv0[p.first]->mid_p = skyline_index_lv0[tar];
						if(!skyline_index_lv0[tar]->mid_c) skyline_index_lv0[tar]->mid_c = skyline_index_lv0[p.first];
						stopflag = true;
						break;
					} else {
						offsetb--;
					}
				}
				if(stopflag){
					break;
				} else {
					offseta--;
					offsetb = -1;
				}

			}
		}
		
	}

	for(const auto &p: skyline_index_lv0){
		if(skyline_index_lv0[p.first]->mid_p){
			cout << "***************" << "\n";
			cout << "mid_p exists: " << endl;
			cout << skyline_index_lv0[p.first]->hubid << endl;
			cout << "***************" << "\n";
			break;
		}
	}
	
	cout << "----------------------------------" << "\n";
	cout << "skyline_index_hub building done!" << endl;

	
	


	cout << "----------------------------------" << "\n";
	cout << "Give out one vertex in (delta, delta)-core: " << endl;

	bool ddFlag = false;
	for(const auto &p: skyline_index_lv0){
		if(skyline_index_lv0[p.first]->hubid[0]==delta && skyline_index_lv0[p.first]->hubid[1]==delta){
			// cout << skyline_index_lv0[p.first]->nodeset << "\n";
			ddFlag = true;
		}
	}
	if(!ddFlag) cout << "(d,d)-skyLabel is covered by others." << endl;
	cout << "----------------------------------" << "\n";
	cout << "checking of (d,d)-core done!" << endl;

	

}

void build_skyline_index_lv1(BiGraph& g, 
	unordered_map<vector<int>, vector<string>, VectorHasher>& skyNode, 
	unordered_map<vector<int>, skyline_index_hub*, VectorHasher>& skyline_index_lv0,
	unordered_map<string, vector<skyline_index_hub*>>& skyline_index_v2h,
    unordered_map<string, vector<skyline_index_ccblock*>>& skyline_index_v2ccb, 
	unordered_map<string, skyline_index_ccblock*>& skyline_index_v2par, 
	unordered_map<vector<int>, skyline_index_ccblock*, VectorHasher>& skyline_index_lv1, 
	int& delta){

		
	skyline_index_v2ccb.clear();
	skyline_index_lv1.clear();

	vector<bool> left_pushed; vector<bool> right_pushed;
	vector<bool> left_pushed_glb; vector<bool> right_pushed_glb;

	left_pushed.resize(g.num_v1, false); 
	right_pushed.resize(g.num_v2, false);
	left_pushed_glb.resize(g.num_v1, false); 
	right_pushed_glb.resize(g.num_v2, false);

	vector<vector<int>> build_start_set;
	vector<vector<int>> link_remain_set;

	// build from left side
	for(auto e: skyNode){
		if(skyline_index_lv0[e.first]->left_p == nullptr){
			build_start_set.push_back(e.first);
		}
	}

	for(auto e: build_start_set){

		build_path_left(e, g, skyline_index_lv0, skyline_index_v2h, skyline_index_v2ccb, 
		skyline_index_v2par, skyline_index_lv1);
			
	}


	// link remaining from right side
	for(auto e: skyNode){
		if(skyline_index_lv0[e.first]->right_p == nullptr){
			link_remain_set.push_back(e.first);
		}
	}
	// cout << "link_remain_set: " << link_remain_set << endl;

	
	for(auto e: link_remain_set){

		link_path_right(e, g, skyline_index_lv0, skyline_index_v2h, skyline_index_v2ccb, 
		skyline_index_v2par, skyline_index_lv1);

	}
	

	
	int cnt = 0;
	for(auto e: skyline_index_lv1){
		cnt++;
	}
	cout << "number of ccblocks: " << cnt << endl;

	// int countt = 0;
	// for(const auto &p: skyline_index_v2ccb){
	// 	for(const auto addr: skyline_index_v2ccb[p.first]){
	// 		countt++;
	// 	}
	// }
	// cout << "number of v2ccb: " << countt << endl;

	// countt = 0;
	// for(const auto &p: skyline_index_v2h){
	// 	for(const auto addr: skyline_index_v2h[p.first]){
	// 		countt++;
	// 	}
	// }
	// cout << "number of v2h: " << countt << endl;
	

}

void retrieve_skyline_index_lv0(BiGraph& g, 
	unordered_map<vector<int>, vector<string>, VectorHasher>& skyNode,
	unordered_map<vector<int>, skyline_index_hub*, VectorHasher>& skyline_index_lv0, 
	vector<bool>& left_node, vector<bool>& right_node, int a_q, int b_q) {

	fill_n(left_node.begin(), left_node.size(), false);
	fill_n(right_node.begin(), right_node.size(), false);

	vector<vector<int>> build_start_set;

	for(auto e: skyNode){
		if(skyline_index_lv0[e.first]->left_c == nullptr 
		&& skyline_index_lv0[e.first]->right_c == nullptr){
			if(e.first[0]>=a_q && e.first[1]>=b_q){
				build_start_set.push_back(e.first);
				break;
			}
		}
	}

	if(!build_start_set.size()){
		cout << "NO results! Try another (alpha, beta) or another query vertex." << endl;
		return;
	} else {
		cout << "the number of start set is " << build_start_set.size() << "\n";
		cout << "they are: \n";
		for (auto e: build_start_set){
			cout << e << " ";
		}
		cout << "\n ---end--- \n";
	}

	queue<skyline_index_hub*> Que;
	for(auto e: build_start_set){
		Que.push(skyline_index_lv0[e]);
		skyline_index_hub* cur = nullptr; // store the topest skyNode

		// cout << "Start BFS retrieving... " << e << "\n";
		while(!Que.empty()){
			cur = Que.front();
			Que.pop();

			// mark all qualified vertices
			for(auto vt : cur->nodeset){
				if(vt[0]=='u'){
					vt.erase(vt.begin());
					int vtid = stoi(vt)-1;
					if(!left_node[vtid]) left_node[vtid] = true;
				} else {
					vt.erase(vt.begin());
					int vtid = stoi(vt)-1;
					if(!right_node[vtid]) right_node[vtid] = true;
				}
			}

			if(!cur->visited) cur->visited = true;

			// retrieving upwards for all qualified skyline_index_block
			if(cur->left_p 
			&& cur->left_p->hubid[0]>=a_q 
			&& cur->left_p->hubid[1]>=b_q 
			&& !cur->left_p->visited) {
				Que.push(cur->left_p);
				cur->left_p->visited = true;
			}
			if(cur->right_p 
			&& cur->right_p->hubid[0]>=a_q 
			&& cur->right_p->hubid[1]>=b_q 
			&& !cur->right_p->visited) {
				Que.push(cur->right_p);	
				cur->right_p->visited = true;	
			}


			if(cur->left_c 
			&& cur->left_c->hubid[0]>=a_q 
			&& cur->left_c->hubid[1]>=b_q 
			&& !cur->left_c->visited) {
				Que.push(cur->left_c);	
				cur->left_c->visited = true;	
			}

			if(cur->right_c 
			&& cur->right_c->hubid[0]>=a_q 
			&& cur->right_c->hubid[1]>=b_q 
			&& !cur->right_c->visited) {
				Que.push(cur->right_c);	
				cur->right_c->visited = true;	
			}

						
		}

	}

	cout << "Lv0 BFS Retrieving done! " << endl;
	cout << "----------------------------------" << "\n";

	for(const auto &p: skyline_index_lv0){
		if(skyline_index_lv0[p.first]->visited) skyline_index_lv0[p.first]->visited = false;
	}


	cout << "Query done, the result for " << "(" << a_q << ", " << b_q <<")-core is :" <<endl;

	int ans_u = 0, ans_v = 0;
	cout << "vertices in upper layer: " << endl;
	for(int i=0; i<left_node.size(); i++){
		// if(left_node[i]!=false) cout << "u" << i+1 << " ";
		if(left_node[i]!=false) ans_u++;
	}
	cout << ans_u;
	cout << "\n";

	cout << "vertices in lower layer: " << endl;
	for(int i=0; i<right_node.size(); i++){
		// if(right_node[i]!=false) cout << "v" << i+1 << " ";
		if(right_node[i]!=false) ans_v++;
	}
	cout << ans_v;
	cout << "\n";


}

void retrieve_skyline_index_lv1(BiGraph& g, 
	unordered_map<string, vector<skyline_index_ccblock*>>& skyline_index_v2ccb, 
	unordered_map<vector<int>, skyline_index_ccblock*, VectorHasher>& skyline_index_lv1, 
	vector<bool>& left_node, vector<bool>& right_node, int a_q, int b_q, string v_q) {

	fill_n(left_node.begin(), left_node.size(), false);
	fill_n(right_node.begin(), right_node.size(), false);

	vector<vector<int>> skycand;
	for(const auto &addr: skyline_index_v2ccb[v_q]){
		int a_s = addr->hubid[0];
		int b_s = addr->hubid[1];
		if(a_s >= a_q && b_s >= b_q){
			skycand.push_back({a_s, b_s, addr->ccid});
			break;
		}	
	}

	if(!skycand.size()){
		cout << "NO results! Try another (alpha, beta) or another query vertex." << endl;
		return;
	} else {
		cout << "the number of skycand of " << v_q << " is " << skycand.size() << "\n";
		cout << "they are: \n";
		for (auto e: skycand){
			cout << e << " ";
		}
		cout << "\n ---end--- \n";
	}

	
	queue<skyline_index_ccblock*> Que;

	for(auto e: skycand){
		Que.push(skyline_index_lv1[e]);
		skyline_index_ccblock* cur = nullptr; 
		// cout << "Start BFS retrieving... " << e << "\n";
		while(!Que.empty()){
			cur = Que.front();
			Que.pop();

			// mark all qualified vertices
			for(auto vt : cur->nodeset){
				if(vt[0]=='u'){
					vt.erase(vt.begin());
					int vtid = stoi(vt)-1;
					if(!left_node[vtid]) left_node[vtid] = true;
				} else {
					vt.erase(vt.begin());
					int vtid = stoi(vt)-1;
					if(!right_node[vtid]) right_node[vtid] = true;
				}
			}

			if(!cur->visited) cur->visited = true;

			// retrieving upwards for all qualified skyline_index_block
			if(cur->left_p 
			&& cur->left_p->hubid[0]>=a_q 
			&& cur->left_p->hubid[1]>=b_q 
			&& !cur->left_p->visited) {
				Que.push(cur->left_p);
				cur->left_p->visited = true;
			}
			if(cur->right_p 
			&& cur->right_p->hubid[0]>=a_q 
			&& cur->right_p->hubid[1]>=b_q 
			&& !cur->right_p->visited) {
				Que.push(cur->right_p);	
				cur->right_p->visited = true;	
			}

			if(cur->left_c.size()!=0){
				for(auto blk: cur->left_c){
					if(!blk->visited
					&& blk->hubid[0]>=a_q
					&& blk->hubid[1]>=b_q){
						Que.push(blk);	
						blk->visited = true;
					}
				}
			}

			if(cur->right_c.size()!=0){
				for(auto blk: cur->right_c){
					if(!blk->visited
					&& blk->hubid[0]>=a_q
					&& blk->hubid[1]>=b_q){
						Que.push(blk);	
						blk->visited = true;
					}
				}
			}
			
		}

	}

	cout << "BFS Retrieving done! " << endl;
	cout << "----------------------------------" << "\n";

	cout << "Query done, the result for " << "(" << a_q << ", " << b_q <<")-core of " << v_q << " is :" <<endl;

	int ans_u = 0, ans_v = 0;
	cout << "vertices in upper layer: " << endl;
	for(int i=0; i<left_node.size(); i++){
		// if(left_node[i]!=false) cout << "u" << i+1 << " ";
		if(left_node[i]!=false) ans_u++;
	}
	cout << ans_u;
	cout << "\n";

	cout << "vertices in lower layer: " << endl;
	for(int i=0; i<right_node.size(); i++){
		// if(right_node[i]!=false) cout << "v" << i+1 << " ";
		if(right_node[i]!=false) ans_v++;
	}
	cout << ans_v;
	cout << "\n";

}

void output_skyline_index(
	string path,
	unordered_map<string, vector<skyline_index_ccblock*>>& skyline_index_v2ccb, 
	unordered_map<vector<int>, skyline_index_ccblock*, VectorHasher>& skyline_index_lv1,
	unordered_map<vector<int>, skyline_index_hub*, VectorHasher>& skyline_index_lv0,
	unordered_map<string, vector<skyline_index_hub*>>& skyline_index_v2h
	){
	
	printf("Saving binary...\n");
	FILE* fout = fopen((path + "skyind_lv1.bin").c_str(), "wb");
	for(auto iter=skyline_index_v2ccb.begin(); iter!=skyline_index_v2ccb.end(); iter++){
        fwrite(&(iter->first), sizeof(string), 1, fout);
        fwrite(&(iter->second), sizeof(skyline_index_ccblock*), iter->second.size(), fout);
    }
	for(auto iter=skyline_index_lv1.begin(); iter!=skyline_index_lv1.end(); iter++){
        fwrite(&(iter->first), sizeof(int), iter->first.size(), fout);
        fwrite(&(iter->second), sizeof(skyline_index_ccblock*), 1, fout);
    }
	fclose(fout);


	fout = fopen((path + "skyind_lv0.bin").c_str(), "wb");
	for(auto iter=skyline_index_lv0.begin(); iter!=skyline_index_lv0.end(); iter++){
        fwrite(&(iter->first), sizeof(int), iter->first.size(), fout);
        fwrite(&(iter->second), sizeof(skyline_index_hub*), 1, fout);
    }
	fclose(fout);

	cout << "save index to bin done." << endl;
    
}


void process_mem_usage(double& vm_usage, double& resident_set)
{
    vm_usage     = 0.0;
    resident_set = 0.0;

    // the two fields we want
    unsigned long vsize;
    long rss;
    {
        std::string ignore;
        std::ifstream ifs("/proc/self/stat", std::ios_base::in);
        ifs >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore
                >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore
                >> ignore >> ignore >> vsize >> rss;
    }

    long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
    vm_usage = vsize / 1024.0;
    resident_set = rss * page_size_kb;
}

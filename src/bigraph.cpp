#include "bigraph.h"
#include <string>
#include <algorithm>
#include <iostream>
#include <cstdio>
#include <list>
#include <vector>
#include <unordered_set>
#include <map>
#include <set>
#include <cstdlib>
#include "utility.h"
using namespace std;

#define LINE_LENGTH 1000



BiGraph::BiGraph(string dir)
{
	num_v1 = 0;
	num_v2 = 0;
	num_edges = 0;

	neighbor_v1.clear();
	neighbor_v2.clear();


	degree_v1.clear();
	degree_v2.clear();

	core_v1.clear();
	core_v2.clear();

	//KKCore index left (x,*) right (*,x)
	left_index.clear();
	right_index.clear();
	v1_max_degree = 0;
	v2_max_degree = 0;
	delta = -1;
	this->dir = dir;
	loadGraph(dir);
}

BiGraph::BiGraph() {
	dir = "";
	num_v1 = 0;
	num_v2 = 0;
	num_edges = 0;

	neighbor_v1.clear();
	neighbor_v2.clear();


	degree_v1.clear();
	degree_v2.clear();

	core_v1.clear();
	core_v2.clear();

	//KKCore index left (x,*) right (*,x)
	left_index.clear();
	right_index.clear();
	v1_max_degree = 0;
	v2_max_degree = 0;
	delta = -1;
}

void BiGraph::print()
{
	string bigraphE = dir + "graph.e";
	string bigraphMeta = dir + "graph.meta";

	FILE *graphEF = fopen(bigraphE.c_str(), "w");
	FILE *graphMetaF = fopen(bigraphMeta.c_str(), "w");

	fprintf(graphMetaF, "%d\n%d\n%d\n", num_v1, num_v2, num_edges);
	fclose(graphMetaF);
	for (int i = 0; i < num_v1; ++i)
	{
		for (int j = 0; j < neighbor_v1[i].size(); ++j)
		{
			fprintf(graphEF, "%d %d\n", i, neighbor_v1[i][j]);
		}
	}
	fclose(graphEF);
}

void BiGraph::print(bool hash) {
	string bigraphE = dir + "graph.e";
	string bigraphMeta = dir + "graph.meta";

	FILE *graphEF = fopen(bigraphE.c_str(), "w");
	FILE *graphMetaF = fopen(bigraphMeta.c_str(), "w");

	fprintf(graphMetaF, "%d\n%d\n%d\n", neighborHash_v1.size(), neighborHash_v2.size(), num_edges);
	fclose(graphMetaF);
	for (int i = 0; i < neighborHash_v1.size(); ++i)
	{
		for (auto j = neighborHash_v1[i].begin(); j != neighborHash_v1[i].end(); ++j)
		{
			fprintf(graphEF, "%d %d\n", i, *j);
		}
	}
	fclose(graphEF);
}

void BiGraph::printCout()
{
	cout << "\nBiGraph: " << endl;
	for (int i = 0; i < num_v1; ++i)
	{
		cout << i << ": ";
		if (neighbor_v1[i].size() == 0)
		{
			cout << "compress error" << endl;
			exit(1);
		}
		for (int j = 0; j < neighbor_v1[i].size(); ++j)
		{
			cout << neighbor_v1[i][j] << ", ";
		}
		cout << endl;
	}
	cout << endl;

}

void BiGraph::printSum()
{
	cout << "\nBiGraph Sum: " << endl;
	cout << "num_v1: " << num_v1 << endl;
	cout << "num_v2: " << num_v2 << endl;
	cout << "edge: " << num_edges << endl;

}

void BiGraph::init(unsigned int num1, unsigned int num2)
{
	num_v1 = num1;
	num_v2 = num2;
	num_edges = 0;

	neighbor_v1.resize(num_v1);
	neighbor_v2.resize(num_v2);

	degree_v1.resize(num_v1);
	degree_v2.resize(num_v2);

	fill_n(degree_v1.begin(), num_v1, 0);
	fill_n(degree_v2.begin(), num_v2, 0);

	left_delete.resize(num_v1);
	right_delete.resize(num_v2);
}

void BiGraph::loadGraph(string dir)
{
	unsigned int n1, n2;
	unsigned int edges = 0;
	int u, v, t;
	int r;

	string metaFile = dir + "graph.meta";
	string edgeFile = dir + "graph.e";

	FILE * metaGraph = fopen(metaFile.c_str(), "r");
	FILE * edgeGraph = fopen(edgeFile.c_str(), "r");

	if (fscanf(metaGraph, "%d\n%d", &n1, &n2) != 2)
	{
		fprintf(stderr, "Bad file format: n1 n2 incorrect\n");
		exit(1);
	}

	fprintf(stdout, "n1: %d, n2: %d\n", n1, n2);

	init(n1, n2);

	while ((r = fscanf(edgeGraph, "%d %d %d", &u, &v, &t)) != EOF)
	{
		if (r != 3)
		{
			fprintf(stderr, "Bad file format: u v t incorrect\n");
			exit(1);
		}

		addEdge(u, v, t);
	}

	fclose(metaGraph);
	fclose(edgeGraph);

	for (int i = 0; i < num_v1; ++i)
	{
		neighbor_v1[i].shrink_to_fit();
		sort(neighbor_v1[i].begin(), neighbor_v1[i].end());

	}
	for (int i = 0; i < num_v2; ++i)
	{
		neighbor_v2[i].shrink_to_fit();
		sort(neighbor_v2[i].begin(), neighbor_v2[i].end());
	}


}

void BiGraph::addEdge(vid_t u, vid_t v, vid_t t)
{

	neighbor_v1[u].push_back(v);
	++degree_v1[u];
	if (degree_v1[u] > v1_max_degree) v1_max_degree = degree_v1[u];
	neighbor_v2[v].push_back(u);
	++degree_v2[v];
	if (degree_v2[v] > v2_max_degree) v2_max_degree = degree_v2[v];
	num_edges++;
}

// not change max_degree
void BiGraph::deleteEdge(vid_t u, vid_t v, vid_t t)
{
	for (int i = 0; i < degree_v1[u]; ++i)
	{
		int vv = neighbor_v1[u][i];
		if (vv == v)
		{
			swap(neighbor_v1[u][i], neighbor_v1[u][degree_v1[u] - 1]);
			--degree_v1[u];
			neighbor_v1[u].pop_back();
			num_edges--;//only once!!!
			break;
		}
	}

	if (degree_v1[u] + 1 == v1_max_degree) {
		v1_max_degree = 0;
		for (auto d : degree_v1) {
			v1_max_degree = v1_max_degree < d ? d : v1_max_degree;
		}
	}

	for (int i = 0; i < degree_v2[v]; ++i)
	{
		int uu = neighbor_v2[v][i];
		if (uu == u)
		{
			swap(neighbor_v2[v][i], neighbor_v2[v][degree_v2[v] - 1]);
			--degree_v2[v];
			neighbor_v2[v].pop_back();
			break;
		}
	}

	if (degree_v2[v] + 1 == v2_max_degree) {
		v2_max_degree = 0;
		for (auto d : degree_v2) {
			v2_max_degree = v2_max_degree < d ? d : v2_max_degree;
		}
	}
}

bool BiGraph::isEdge(vid_t u, vid_t v)
{
	/*if (binary_search(neighbor_v1[u].begin(),
		neighbor_v1[u].begin() + degree_v1[u], v))
		return true;
	else
		return false;*/
	/*if (neighborHash_v1[u].find(v) == neighborHash_v1[u].end())
	{
	return false;
	}
	else
	return true;*/
	for (auto it = neighbor_v1[u].begin(); it != neighbor_v1[u].end(); it++) {
		if (*it == v) return true;
	}
	return false;
}

void BiGraph::compressGraph()
{
	vector<unordered_set<vid_t>> n_neighborHash_v1;
	vector<unordered_set<vid_t>> n_neighborHash_v2;

	n_neighborHash_v1.resize(num_v1);
	n_neighborHash_v2.resize(num_v2);

		for (int i = 0; i < num_v1; ++i)
		{
			n_neighborHash_v1[i].reserve(10*neighbor_v1[i].size());
			for (int j = 0; j < neighbor_v1[i].size(); ++j)
			{
				int v = neighbor_v1[i][j];
				n_neighborHash_v1[i].insert(v);
			}
		}

		for (int i = 0; i < num_v2; ++i)
		{
			n_neighborHash_v2[i].reserve(2*neighbor_v2[i].size());
			for (int j = 0; j < neighbor_v2[i].size(); ++j)
			{
				int v = neighbor_v2[i][j];
				n_neighborHash_v2[i].insert(v);
			}
		}

	swap(n_neighborHash_v1, neighborHash_v1);
	swap(n_neighborHash_v2, neighborHash_v2);

	for (int i = 0; i < num_v1; ++i)
	{
	if (neighbor_v1[i].size() != degree_v1[i])
	cout << "degree error" << endl;
	}

	for (int i = 0; i < num_v2; ++i)
	{
	if (neighbor_v2[i].size() != degree_v2[i])
	cout << "degree error" << endl;
	}

	cout<<"degree correct"<<endl;

}

int BiGraph::get_left_index_with_fixed_left_k(vid_t u, int left_k) {
	if (left_index[u].size() > left_k) return left_index[u][left_k];
	else return 0;
}

bool isNodesetEqual(unordered_set<vid_t>& set1, unordered_set<vid_t>& set2) {
	if (set1.size() != set2.size()) {
		return false;
	}
	for (auto it = set1.begin(); it != set1.end(); it++) {
		auto got = set2.find(*it);
		if (got == set2.end()) {
			return false;
		}
	}
	return true;
}

void inv(BiGraph& g) {
	swap(g.degree_v1, g.degree_v2);
	swap(g.left_index, g.right_index);
	swap(g.num_v1, g.num_v2);
	swap(g.neighbor_v1, g.neighbor_v2);
	swap(g.neighborHash_v1, g.neighborHash_v2);
	swap(g.v1_max_degree, g.v2_max_degree);
	swap(g.left_delete, g.right_delete);
}


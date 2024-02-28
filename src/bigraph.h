#pragma once
#ifndef __BIGRAPH_H
#define __BIGRAPH_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <unordered_set>
#include "utility.h"


struct skyline_index_ccblock{
	std::vector<std::string> nodeset;
	std::vector<int> hubid;
	int ccid;
	skyline_index_ccblock* left_p = nullptr;
	skyline_index_ccblock* right_p = nullptr;
	skyline_index_ccblock* mid_p = nullptr;

	std::vector<skyline_index_ccblock*> left_c;
	std::vector<skyline_index_ccblock*> right_c;
	std::vector<skyline_index_ccblock*> mid_c;

	bool visited = false;

};

struct skyline_index_hub{
	std::vector<skyline_index_ccblock*> blockset;
	std::vector<std::string> nodeset;
	std::vector<int> hubid;
	skyline_index_hub* left_p = NULL;
	skyline_index_hub* right_p = NULL;
	skyline_index_hub* left_c = NULL;
	skyline_index_hub* right_c = NULL;
	skyline_index_hub* mid_p = NULL;
	skyline_index_hub* mid_c = nullptr;
	bool visited = false;
	// bool pushed = false;

};

struct VectorHasher {
    int operator()(const std::vector<int>& V) const {
        int hash = V.size();
        for(auto &i : V) {
            hash ^= i + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        }
        return hash;
    }
};


class Edge
{
public:
	Edge(int u_, int v_, int t_) { u = u_; v = v_; t = t_;}
	bool operator<(const Edge &other) const
	{
		if (u == other.u)
			return v < other.v;
		return u < other.u;
	}

	int u;
	int v;
	int t;
};

class DegreeNode
{
public:
	int id;
	int degree;
};

class BiGraph
{

public:

	BiGraph(std::string dir);
	BiGraph();
	~BiGraph() {}

	void addEdge(vid_t u, vid_t v, vid_t t);
	void deleteEdge(vid_t u, vid_t v, vid_t t);
	bool isEdge(vid_t u, vid_t v);
	num_t getV1Num() { return num_v1; }
	num_t getV2Num() { return num_v2; }
	num_t getV1Degree(vid_t u) { return degree_v1[u]; }
	num_t getV2Degree(vid_t u) { return degree_v2[u]; }
	std::vector<vid_t> & getV2Neighbors(vid_t u) { return neighbor_v2[u]; }
	std::vector<vid_t> & getV1Neighbors(vid_t u) { return neighbor_v1[u]; }
	void print();
	void print(bool hash);
	void printSum();
	void printCout();

public:

	void init(unsigned int num_v1, unsigned int num_v2);
	void loadGraph(std::string dir);
	void compressGraph();

	std::string dir;
	num_t num_v1;
	num_t num_v2;
	num_t num_edges;

	std::vector<std::vector<vid_t>> neighbor_v1;
	std::vector<std::vector<vid_t>> neighbor_v2;

	std::vector<std::unordered_set<vid_t>> neighborHash_v1;
	std::vector<std::unordered_set<vid_t>> neighborHash_v2;

	std::vector<int> degree_v1;
	std::vector<int> degree_v2;

	std::vector<num_t> core_v1;
	std::vector<num_t> core_v2;

public:

	//KKCore index left (x,*) right (*,x)
	std::vector<std::vector<int>> left_index;
	std::vector<std::vector<int>> right_index;
	int v1_max_degree;
	int v2_max_degree;
	std::vector<bool> left_delete;
	std::vector<bool> right_delete;
	// for dynamic update
	std::vector<std::vector<int>> left_index_old;
	std::vector<std::vector<int>> right_index_old;
	//BiGraph operator=(const BiGraph& g);
	int delta = -1;

public:
	int get_left_index_with_fixed_left_k(vid_t u, int left_k);
	//BiGraph& operator=(const BiGraph& g_);
};



#endif  /* __BIGRAPH_H */
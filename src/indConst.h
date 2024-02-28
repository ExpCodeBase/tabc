#pragma once
#ifndef INDCONST_H
#define INDCONST_H
#include "bigraph.h"
#include <fstream>

// extern bool DELETEC;
// extern bool APC;
// extern long long DELETECOUNT;
// extern long long APCOUNT;


extern void inv(BiGraph& g);

extern void process_mem_usage(double& vm_usage, double& resident_set);

extern void alphaCopyPeel_for_kcore(int left_k, BiGraph& g);

extern void coreDomComp(BiGraph& g);

extern int maxkcorenumber_test_optimal(BiGraph& g);

extern void domIndexDelta(BiGraph& g, std::unordered_map<std::string, std::vector<std::vector<int>>>& skyLable, int& delta);

extern void build_skyline_index_lv0(BiGraph&g, 
	std::unordered_map<std::string, std::vector<std::vector<int>>>& skyLable, 
	std::unordered_map<std::vector<int>, std::vector<std::string>, VectorHasher>& skyNode,
	std::unordered_map<std::vector<int>, skyline_index_hub*, VectorHasher>& skyline_index_lv0,  
	std::unordered_map<std::string, std::vector<skyline_index_hub*>>& skyline_index_v2h, 
	int& delta);

extern void build_skyline_index_lv1(BiGraph& g, 
	std::unordered_map<std::vector<int>, std::vector<std::string>, VectorHasher>& skyNode, 
	std::unordered_map<std::vector<int>, skyline_index_hub*, VectorHasher>& skyline_index_lv0,
	std::unordered_map<std::string, std::vector<skyline_index_hub*>>& skyline_index_v2h, 
    std::unordered_map<std::string, std::vector<skyline_index_ccblock*>>& skyline_index_v2ccb, 
	std::unordered_map<std::string, skyline_index_ccblock*>& skyline_index_v2par, 
	std::unordered_map<std::vector<int>, skyline_index_ccblock*, VectorHasher>& skyline_index_lv1, 
	int& delta);

extern void output_skyline_index(
	std::string path,
    std::unordered_map<std::string, std::vector<skyline_index_ccblock*>>& skyline_index_v2ccb, 
	std::unordered_map<std::vector<int>, skyline_index_ccblock*, VectorHasher>& skyline_index_lv1,
	std::unordered_map<std::vector<int>, skyline_index_hub*, VectorHasher>& skyline_index_lv0,
	std::unordered_map<std::string, std::vector<skyline_index_hub*>>& skyline_index_v2h);

extern void retrieve_skyline_index_lv0(BiGraph& g,  
	std::unordered_map<std::vector<int>, std::vector<std::string>, VectorHasher>& skyNode, 
	std::unordered_map<std::vector<int>, skyline_index_hub*, VectorHasher>& skyline_index_lv0, 
	std::vector<bool>& left_node, std::vector<bool>& right_node, int a_q, int b_q);

extern void retrieve_skyline_index_lv1(BiGraph& g, 
    std::unordered_map<std::string, std::vector<skyline_index_ccblock*>>& skyline_index_v2ccb, 
	std::unordered_map<std::vector<int>, skyline_index_ccblock*, VectorHasher>& skyline_index_lv1,  
	std::vector<bool>& left_node, std::vector<bool>& right_node, int a_q, int b_q, std::string v_q);

#endif // !INDCONST_H
#include <ctime>
#include "indConst.h"
#include "bigraph.h"
using namespace std;


int main(int argc, char **argv) {
	if (argc == 1) {
		cout << "error in number of arguments" << endl;
	}
	string exec_type = argv[1];
	if (exec_type == "-domIndDelta") {
		cout << "start domIndDelta for " << argv[2] << endl;
		unordered_map<string, vector<vector<int>>> skyLable;
		int delta = 0;
		BiGraph g(argv[2]);
		auto start = chrono::system_clock::now();
		domIndexDelta(g, skyLable, delta);
		auto end = chrono::system_clock::now();
		chrono::duration<double> time = end - start;
		cout << "domIndDelta running time: " << time.count() << endl;
	}
	else if (exec_type == "-domQuery") {
		cout << "start domQuery for " << argv[2] << endl;
		BiGraph g(argv[2]);
		unordered_map<string, vector<vector<int>>> skyLable;
		unordered_map<vector<int>, vector<string>, VectorHasher> skyNode;
		unordered_map<vector<int>, skyline_index_hub*, VectorHasher> skyline_index_lv0;
		unordered_map<vector<int>, skyline_index_ccblock*, VectorHasher> skyline_index_lv1;
		unordered_map<string, vector<skyline_index_ccblock*>> skyline_index_v2ccb;
		unordered_map<string, vector<skyline_index_hub*>> skyline_index_v2h;
		int delta = 0;
		vector<bool> left; vector<bool> right;
		left.resize(g.num_v1, false); right.resize(g.num_v2, false);
		auto start1 = chrono::system_clock::now();
		domIndexDelta(g, skyLable, delta);
		auto end1 = chrono::system_clock::now();
		cout << "delta: " << delta << endl;
		chrono::duration<double> time1 = end1 - start1;
		cout << "----------------------------------" << "\n";
		cout << "lable computing time: " << time1.count() << endl;
		cout << "----------------------------------" << "\n";
		auto start2 = chrono::system_clock::now();
		build_skyline_index_lv0(g, skyLable, skyNode, skyline_index_lv0, skyline_index_v2h, delta);
		auto end2 = chrono::system_clock::now();
		chrono::duration<double> time2 = end2 - start2;
		cout << "----------------------------------" << "\n";
		cout << "index constructing time: " << time2.count() << endl;

		auto start5 = chrono::system_clock::now();
		retrieve_skyline_index_lv0(g, skyNode, skyline_index_lv0, left, right, stoi(argv[3]), stoi(argv[4]));
		auto end5 = chrono::system_clock::now();
		chrono::duration<double> time5 = end5 - start5;
		cout << "----------------------------------" << "\n";
		cout << "query time: " << time5.count() << endl;
		cout << "----------------------------------" << "\n";
		
		auto start6 = chrono::system_clock::now();
		output_skyline_index(argv[2], skyline_index_v2ccb, skyline_index_lv1, skyline_index_lv0, skyline_index_v2h);
		auto end6 = chrono::system_clock::now();
		chrono::duration<double> time6 = end6 - start6;
		cout << "----------------------------------" << "\n";
		cout << "index saving time: " << time6.count() << endl;
		cout << "----------------------------------" << "\n";

	}

	else {
		cout << "illegal arguments" << endl;
	}
	return 0;
}
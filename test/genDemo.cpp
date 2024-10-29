#include <ctime>
#include <cstdlib>
#include <vector>
#include <set>
#include <iostream>
#include "motsptw_fastdom.hpp"


using namespace std;
bool run(vector<vector<double>>& grid) {
	vector<pair<double, double>> tw = {
		{0, 100},
		{0, 100},
		{0, 100},
		{0, 100},
		{0, 100},
	};
	set<long> keys = {1, 3};
	vector<double> st = {0, 0, 0, 0, 0};
	rzq::search_fastdom::MOTSPTWResult res;
	rzq::search_fastdom::RunMOTSPTW(&grid, tw, st, 0, 4, keys, &res, true, false, 300);
	if (res.paths.size()>1) return true;
	return false;
}

void floyd(vector<vector<double>>& g) {
	int n = 5;
	for (int k=0; k<n; k++) {
		for (int i=0; i<n; i++) {
			for (int j=0; j<n; j++) {
				g[i][j] = min(g[i][j], g[i][k] + g[k][j]);
			}
		}
	}
}

void print(vector<vector<double>>& g) {
	int n = 5;
	cout << n << endl;
	for (int i=0; i<n; i++) {
		for (int j=0; j<n; j++) cout << g[i][j] << " ";
		cout << endl;
	}
	for (int i=0; i<n; i++) cout << "0 100" << endl;
	cout << "2\n1\n3" << endl;
}

int main() {
	srand(time(0));	
	long cnt = 0;
	long maxtry = 100000000;
	vector<vector<double>> g = {
		{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0},
		{999, 999, 999, 999, 0},
	};
	const int maxw = 10;
	while (cnt < maxtry) {
		for (int i=0; i<4; i++) {
			for (int j=i+1; j<4; j++) {
				g[i][j] = rand() % maxw + 1;
				g[j][i] = g[i][j];
			}
		}
		for (int i=0; i<4; i++) {
			g[i][4] = g[i][0];
		}
		floyd(g);
		if (run(g)) {
			print(g);
			break;
		}
		printf("%ld\r", cnt);
		cnt++;
	}
	return 0;
}

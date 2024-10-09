#include "search_motsptw.hpp"
#include "data_loader.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <chrono>

const double TIMELIMIT = 300;


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "invalid input" << std::endl;
        return -1;
    }

    rzq::basic::SparseGraph g;
    std::string fn = argv[1];
		// remove suffix ".txt"
		auto spos = fn.find_last_of('/')+1;
		std::string insname = fn.substr(spos, fn.length()-4-spos);

    rzq::basic::DataLoader dl(fn);
    dl.Load();
    dl.CreateGraph(g);

    long vo = 0;
    long vd = dl.GetVd();

    std::set<long> keys = dl.GetKeys();

    rzq::search::MOTSPTWResult res;
    std::vector<std::pair<double, double>> tw = dl.GetTw();
    std::vector<double> st = dl.GetSt();
    auto start = std::chrono::high_resolution_clock::now();
    RunMOTSPTW(&g, tw, st, vo, vd, keys, &res, TIMELIMIT);
    auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

		std::stringstream row;
		std::ofstream fout;
		fout.open("output/res.csv", std::ios_base::app);

		row << insname << ","   
				<< keys.size() << ","
				<< std::setprecision(4) << duration << "," 
				<< res.timeout << "," 
				<< res.paths.size() << "," 
				<< res.num_expd << "," 
				<< res.num_gen << ","
				<< res.max_qsize		<< "," 
				<< res.frontier_pruned << ","
				<< res.sol_pruned		<< "," 
				<< res.fea_pruned << ","
				<< res.post_pruned << std::endl;

		fout << row.str();
		std::cout << row.str();
		fout.close();

		fout.open("output/k" + std::to_string(keys.size()) + "_" + insname  + ".sol", std::ios_base::out);
		for (auto iter: res.paths) {
			long k = iter.first;
			for (auto var: res.costs[k])
				fout << var << " ";
			fout << std::endl;

			for (auto v: res.paths[k])
				fout << v << " ";
			fout << std::endl;
		}
    return 0;
}

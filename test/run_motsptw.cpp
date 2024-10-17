#include <iomanip>
#include <iostream>
#include <ostream>
#include <sstream>
#include <fstream>
#include <string>

#include "data_loader.hpp"
// default planner
#include "search_motsptw.hpp"
// variant: fast dominate
#include "motsptw_fastdom.hpp"
// variant: ddl based heuristic
#include "motsptw_ddl_heur.hpp"
// variant: gap vert based pruning
#include "motsptw_gap_prune.hpp"

const double TIMELIMIT = 300;

void run_default(rzq::basic::DataLoader& dl, rzq::basic::SparseGraph& g, rzq::search::MOTSPTWResult& res)  {
    long vo = 0;
    long vd = dl.GetVd();
		auto grid = g.to_grid(1);
    std::set<long> keys = dl.GetKeys();
    std::vector<std::pair<double, double>> tw = dl.GetTw();
    std::vector<double> st = dl.GetSt();
		rzq::search::RunMOTSPTW(&grid, tw, st, vo, vd, keys, &res, TIMELIMIT);
}

void run_fastdom(rzq::basic::DataLoader& dl, rzq::basic::SparseGraph& g, rzq::search_fastdom::MOTSPTWResult& res,
		bool use_gap_prune=false)  {

    long vo = 0;
    long vd = dl.GetVd();
		auto grid = g.to_grid(1);
    std::set<long> keys = dl.GetKeys();
    std::vector<std::pair<double, double>> tw = dl.GetTw();
    std::vector<double> st = dl.GetSt();
		rzq::search_fastdom::RunMOTSPTW(&grid, tw, st, vo, vd, keys, &res, use_gap_prune, TIMELIMIT);
}


void run_ddl_heur(rzq::basic::DataLoader& dl, rzq::basic::SparseGraph& g, rzq::search_ddl_heur::MOTSPTWResult& res)  {
    long vo = 0;
    long vd = dl.GetVd();
		auto grid = g.to_grid(1);
    std::set<long> keys = dl.GetKeys();
    std::vector<std::pair<double, double>> tw = dl.GetTw();
    std::vector<double> st = dl.GetSt();
		rzq::search_ddl_heur::RunMOTSPTW(&grid, tw, st, vo, vd, keys, &res, TIMELIMIT);
}


void run_gap_prune(rzq::basic::DataLoader& dl, rzq::basic::SparseGraph& g, rzq::search_gap_prune::MOTSPTWResult& res)  {
    long vo = 0;
    long vd = dl.GetVd();
		auto grid = g.to_grid(1);
    std::set<long> keys = dl.GetKeys();
    std::vector<std::pair<double, double>> tw = dl.GetTw();
    std::vector<double> st = dl.GetSt();
		rzq::search_gap_prune::RunMOTSPTW(&grid, tw, st, vo, vd, keys, &res, TIMELIMIT);
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "invalid input" << std::endl;
        return -1;
    }
		std::string solver = "default";
		if (argc > 2) {
			solver = std::string(argv[2]);
		}

    rzq::basic::SparseGraph g;
    std::string fn = argv[1];
		// remove suffix ".txt"
		auto spos = fn.find_last_of('/')+1;
		auto kpos = fn.find_first_of('_')+1;
		std::string kstr = fn.substr(kpos, fn.find('/', kpos)-kpos);
		std::string insname = fn.substr(spos, fn.length()-4-spos);
    rzq::basic::DataLoader dl(fn);
    dl.Load();
    dl.CreateGraph(g);
		rzq::search::MOTSPTWResult res;

		if (solver == "default") {
			run_default(dl, g, res);
		}
		else if (solver == "fastdom") {
			run_fastdom(dl, g, res, false);
		}
		else if (solver == "combine") {
			run_fastdom(dl, g, res, true);
		}
		else if (solver == "ddl_heur") {
			run_ddl_heur(dl, g, res);
		}
		else if (solver == "gap_prune") {
			run_gap_prune(dl, g, res);
		}
		else {
			std::cerr << "Invalid variant: [" << solver << "]" << std::endl;
		}

		std::stringstream row;
		std::ofstream fout;
		fout.open("output/res.csv", std::ios_base::app);

		row << insname << ","   
				<< solver << ","
				<< kstr << ","
				<< std::setprecision(4) << res.runtime << "," 
				<< res.timeout << "," 
				<< res.paths.size() << "," 
				<< res.num_expd << "," 
				<< res.num_gen << ","
				<< res.max_qsize		<< "," 
				<< res.frontier_pruned << ","
				<< res.sol_pruned		<< "," 
				<< res.fea_pruned << ","
				<< res.post_pruned << "," 
				<< res.post2_pruned << std::endl;

		fout << row.str();
		std::cout << row.str();
		fout.close();

		if (res.paths.size() > 0) {
			fout.open("solutions.txt", std::ios_base::out);
			for (auto iter: res.paths) {
				long k = iter.first;
				for (auto var: res.costs[k])
					fout << var << " ";
				fout << std::endl;

				for (auto v: res.paths[k])
					fout << v << " ";
				fout << std::endl;
			}
		}
    return 0;
}

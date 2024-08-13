#include "search_motsptw.hpp"
#include "data_loader.hpp"
#include <iostream>
#include <string>
#include <chrono>


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "invalid input" << std::endl;
        return -1;
    }

    rzq::basic::SparseGraph g;
    std::string fn = argv[1];

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
    RunMOTSPTW(&g, tw, st, vo, vd, keys, &res);
    auto end = std::chrono::high_resolution_clock::now();


    // print paths, times and costs
    std::cout << "---- reprint solutions for more clarity:" << std::endl;
    for (auto iter : res.paths) {
        long k = iter.first; // id of a Pareto-optipmal solution
        // path nodes
        std::cout << " path nodes = ";
        for (auto xx : res.paths[k])
        {
            std::cout << xx << ", ";
        }
        std::cout << std::endl;
        // cost
        std::cout << " cost = " << res.costs[k] << std::endl;
        std::cout << " number of generated labels = " << res.num_generated_labels << std::endl;
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "run time: " << duration << "ms" << std::endl;

    }
    return 0;
}
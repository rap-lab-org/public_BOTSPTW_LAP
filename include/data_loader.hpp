#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>
#include <iostream>

#include "graph.hpp"

namespace rzq{
namespace basic{

class DataLoader{
public:
    DataLoader(std::string fn);
    ~DataLoader();
    void Load();
    void CreateGraph(SparseGraph& g);
    std::vector<std::pair<double, double>> GetTw();
    std::vector<double> GetSt();
    long GetVd();
protected:
    std::string fn;
    int n;
    std::vector<std::vector<double>> cost;
    std::vector<std::vector<double>> time;
    std::vector<std::pair<double, double>> tw;
    std::vector<double> st;
};

}
}
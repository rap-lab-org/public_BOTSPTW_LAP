#include <fstream>

#include "data_loader.hpp"

namespace rzq{
namespace basic{

DataLoader::DataLoader(std::string fn) {
    this->fn = fn;
}

DataLoader::~DataLoader(){}

void DataLoader::Load() {
    std::ifstream file(fn);
    
    file >> n;

    cost.resize(n);
    for (int i = 0; i < n; i++) {
        cost[i].resize(n);
        for (int j = 0; j < n; j++) {
            file >> cost[i][j];
        }
    }

    time.resize(n);
    for (int i = 0; i < n; i++) {
        time[i].resize(n);
        for (int j = 0; j < n; j++) {
            file >> time[i][j];
        }
    }

    tw.resize(n);
    for (int i = 0; i < n; i++) {
        file >> tw[i].first >> tw[i].second;
    }

    st.resize(n);
    for (int i = 0; i < n; i++) {
        file >> st[i];
    }

    file.close();
}

void DataLoader::CreateGraph(SparseGraph& g) {
    std::vector<double> cv;
    cv.resize(2);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j) {
                continue;
            }
            cv[0] = cost[i][j];
            cv[1] = time[i][j];
            g.AddArc(i, j, cv);
        }
    }
}

std::vector<std::pair<double, double>> DataLoader::GetTw() {
    return tw;
}

std::vector<double> DataLoader::GetSt() {
    return st;
}

long DataLoader::GetVd() {
    return (n - 1);
}

}
}

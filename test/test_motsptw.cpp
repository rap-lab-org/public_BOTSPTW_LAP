#include "search_motsptw.hpp"
#include "graph_io.hpp"
#include <iostream>
#include <string>


int Test();

int main(){
  Test();
  return 0;
};

int Test() {
  rzq::basic::SparseGraph g;

  std::vector<double> cv;
  cv.resize(2);
  cv[0] = 1;
  cv[1] = 10;
  g.AddArc(0, 1, cv);

  cv[0] = 1;
  cv[1] = 1;

  g.AddArc(0, 2, cv);

  g.AddArc(1, 2, cv);

  cv[0] = 10;
  cv[1] = 1;

  g.AddArc(1, 3, cv);

  cv[0] = 1;
  cv[1] = 1;

  g.AddArc(2, 3, cv);

  cv[0] = 2;
  cv[1] = 2;
  g.AddArc(2, 1, cv);

  // do some print and verification to make sure the generated graph is correct.

  std::cout << "num_nodes: " << g.NumVertex() << std::endl;
  std::cout << "num_edges: " << g.NumEdge() << std::endl;
  std::cout << "cdims: " << g.CostDim() << std::endl;

  // ######################################### //
  // ####### Test 2 - run planner ######### //
  // ######################################### //

  long vo = 0; // starting node in the graph.
  long vd = 3; // destination node id in the graph.

  rzq::search::MOTSPTWResult res;
  std::vector<std::pair<double, double>> tw;
  tw.resize(g.NumVertex());
  for (int i = 0; i < g.NumVertex(); i++) {
    tw[i] = std::make_pair(0, 100);
  }
  std::vector<double> st;
  st.resize(g.NumVertex());
  for (int i = 0; i < g.NumVertex(); i++) {
    st[i] = 0;
  }
  RunMOTSPTW(&g, tw, st, vo, vd, &res);

  // print paths, times and costs
  std::cout << "---- reprint solutions for more clarity:" << std::endl;
  for (auto iter : res.paths) {
    long k = iter.first; // id of a Pareto-optipmal solution
    // path nodes
    std::cout << " path nodes = ";
    for (auto xx : res.paths[k]) {
      std::cout << xx << ", ";
    }
    std::cout << std::endl;
    // cost
    std::cout << " cost = " << res.costs[k] << std::endl;
  }
  return 1;

};
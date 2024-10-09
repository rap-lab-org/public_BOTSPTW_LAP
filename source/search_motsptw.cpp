#include "search_motsptw.hpp"
#include <set>
#include <memory>
#include <fstream>
#include <algorithm>
#include <map>

namespace rzq{
namespace search{

std::ostream& operator<<(std::ostream& os, Label& l)
{
  std::string s;
  s = "{id:" + std::to_string(l.id) + ",v:" + std::to_string(l.v) + ",b:" + ToString(l.b) + ",g:" 
    + ToString(l.g) + ",f:" + ToString(l.f) + "}";
  os << s;
  return os;
};

Frontier::Frontier() {
  return;
}

Frontier::~Frontier() {
  return;
}

bool dominates(const Label& l1, const Label& l2) {
  if (l1.g[0] > l2.g[0] || l1.g[1] > l2.g[1]) {
    return false;
  }
  for (int i = 0; i < l1.b.size(); i++) {
    if (!l1.b[i] && l2.b[i]) {
      return false;
    }
  }
  return true;
}


bool Frontier::Check(const Label& l) const {
  if (labels.empty()) {
    return false;
  }
  // int flag = 0;
  // for (auto it = labels.begin(); it != labels.end(); it++) {
  //   flag = 0;
  //   if (it->g[0] > l.g[0] || it->g[1] >= l.g[1]) {
  //     continue;
  //   }
  //   for (int i = 0; i < l.b.size(); i++) {
  //     if (l.b[i] && !it->b[i]) {
  //       flag = 1;
  //       break;
  //     }
  //   }
  //   if (flag == 1) {
  //     continue;
  //   }
  //   return true;
  // }
  // return false;
  for (auto it = labels.begin(); it != labels.end(); it++) {
    if (dominates(*it, l)) {
      return true;
    }
  }
  return false;
}

void Frontier::Update(Label l) {
  if (labels.empty()) {
    labels.push_back(l);
    return;
  }
  int flag = 0;
  for (auto it = labels.begin(); it != labels.end(); ) {
    if (it->g[0] < l.g[0] || it->g[1] <= l.g[1]) {
      it++;
      continue;
    }
    for (int i = 0; i < l.b.size(); i++) {
      if (!l.b[i] && it->b[i]) {
        flag = 1;
        break;
      }
    }
    if (flag == 0) {
      it = labels.erase(it);
    }
    else {
      it++;
    }
  }
  labels.push_back(l);
  return;
}

MOTSPTW::MOTSPTW() {};
MOTSPTW::~MOTSPTW() {
    for (int idx = 0; idx < _alpha.size(); idx++){
        delete _alpha[idx];
    }
    delete solu;
    return;
}

void MOTSPTW::SetGraphPtr(basic::PlannerGraph* g) {
  _graph = g;
};

void MOTSPTW::SetTimeWindow(TimeWindowVec tw) {
  _tw = tw;
};

void MOTSPTW::SetServiceTime(std::vector<double> st) {
  _service_time = st;
};

// void MOTSPTW::InitHeu(long vd) {
//   // SimpleTimer timer;
//   // timer.Start();

//   // _dijks.resize(_graph->CostDim());
//   // for (size_t i = 0; i<_graph->CostDim(); i++) {
//   //   _dijks[i].SetGraphPtr(_graph);
//   //   _dijks[i].ExhaustiveBackwards(vd, std::numeric_limits<double>::infinity(), i) ;
//   // }
//   // _res.rt_initHeu = timer.GetDurationSecond();

//   // G_DOM_CHECK_COUNT = 0; // reset dom check counter.
//   return ;
// };

CostVec MOTSPTW::_Heuristic(long v, const BinaryServiceVec& b) {
  auto out = CostVec(_graph->CostDim(), 0);
  double time_h = 0;

  // std::vector<bool> inMST(_graph->NumVertex(), false);
  // std::vector<double> key(_graph->NumVertex(), std::numeric_limits<double>::infinity());
  // std::vector<long> targets;

  // for (long i = 0; i < _graph->NumVertex(); i++) {
  //   if (b[i] == true) {
  //     continue;
  //   }
  //   targets.push_back(i);
  // }

  // std::priority_queue<PLD, std::vector<PLD>, ComparePLD> pq;
  // pq.push(std::make_pair(v, 0));
  // key[v] = 0;
  // while(!pq.empty()) {
  //   long u = pq.top().first;
  //   pq.pop();
  //   if (inMST[u] == true) {
  //     continue;
  //   }
  //   inMST[u] = true;
  //   time_h += key[u];
  //   for (auto i : targets) {
  //     if (inMST[i] == false && key[i] > _graph->GetCost(u, i)[1]) {
  //       key[i] = _graph->GetCost(u, i)[1];
  //       pq.push(std::make_pair(i, key[i]));
  //     }
  //   }
  // }
  
  out[1] = time_h;

  // std::cout << "here" << std::endl;

  return out;
};

MOTSPTWResult MOTSPTW::GetResult() const {
  return _res;
};

long MOTSPTW::_GenLabelId() {

  long out = _label_id_gen++;

  if (_label_id_gen > __vec_alloc_total){
    __vec_alloc_total += __vec_alloc_batch;
    _label.resize(__vec_alloc_total);
    _parent.resize(__vec_alloc_total);
    if (__vec_alloc_batch < __vec_alloc_batch_max){
      __vec_alloc_batch += __vec_alloc_batch;
    }
  }

  return out;
};

bool MOTSPTW::_FrontierCheck(const Label& l) const {
  auto res = _alpha[l.v]->Check(l);
  return res;
};

bool MOTSPTW::_SolutionCheck(const Label& l) const {
  auto temp = solu->Check(l);
  return temp;
};

bool MOTSPTW::_FeaCheck(const Label& l) const {
  if (l.g[1] + _service_time[l.v] > _tw[l.v].second) {
    return true;
  }
  if (l.b[l.v] != true) {
    return true;
  }
  return false;
};

bool MOTSPTW::_PostCheck_1(const Label& l) const {
  for (int i = 0; i < l.b.size(); i++) {
    if (l.b[i] == true) {
      continue;
    }
    // For all nodes that have not been visited, check if the time window is violated.
    if (l.g[1] + _graph->GetCost(l.v, i)[1] + _service_time[i] > _tw[i].second) {
      return true;
    }
  }
  return false;
};

bool MOTSPTW::_PostCheck_2(const Label& l) const {
  std::priority_queue<double, std::vector<double>, std::greater<double>> sub_travel_time;
  for (int i = 0; i < l.b.size(); i++) {
    if (l.b[i] == true && i != l.v) {
      continue;
    }
    for (int j = 0; j < l.b.size(); j++) {
      if (l.b[j] == true) {
        continue;
      }
      if (i == j) {
        continue;
      }
      sub_travel_time.push(_graph->GetCost(i, j)[1]);
    }
  }

  if (sub_travel_time.empty()) {
    return false;
  }

  std::priority_queue<double, std::vector<double>, std::greater<double>> sub_service_time;
  for (int i = 0; i < l.b.size(); i++) {
    if (l.b[i] == true) {
      continue;
    }
    sub_service_time.push(_service_time[i]);
  }

  std::vector<double> ddl;
  for (int i = 0; i < l.b.size(); i++) {
    if (l.b[i] == true) {
      continue;
    }
    ddl.push_back(_tw[i].second);
  }

  std::sort(ddl.begin(), ddl.end());

  double t = l.g[1];

  for (int i = 0; i < ddl.size(); i++) {
    t += sub_travel_time.top() + sub_service_time.top();
    if (t > ddl[i]) {
      return true;
    }
    sub_travel_time.pop();
    sub_service_time.pop();
  }
  return false;
};

std::vector<long> MOTSPTW::_BuildPath(long lid) {
  std::vector<long> out, out2;
  while( lid >= 0 ) {
    out.push_back(_label[lid].v);
    lid = _parent[lid];
  }
  for (size_t i = 0; i < out.size(); i++) {
    out2.push_back(out[out.size()-1-i]);
  }
  return out2;
};

void MOTSPTW::_PostProcRes() {
    for (auto lb : solu->labels) {
      long lid = lb.id;
      _res.paths[lid] = _BuildPath(lid);
      _res.costs[lid] = _label[lid].g;
    }
  return ;
};

void MOTSPTW::_UpdateFrontier(Label l) {
  _alpha[l.v]->Update(l);
};

void MOTSPTW::_InitFrontiers() {
  _alpha.resize(_graph->NumVertex()); // this requires the graph vertex are numbered from 0 to |V|.
  for (int idx = 0; idx < _alpha.size(); idx++){
    _alpha[idx] = new Frontier;
  }
  solu = new Frontier;
  return;
};

bool MOTSPTW::_IsDone(const Label& l) const {
  for (int i = 0; i < l.b.size(); i++) {
    if (l.b[i] == false) {
      return false;
    }
  }
  return true;
};

int MOTSPTW::Search(long vo, long vd) {
    _InitFrontiers();
    _vo = vo;
    _vd = vd;
    _res.reset();
    auto zero_vec = InitVecType(_graph->CostDim(), 0.0);
    BinaryServiceVec bo = InitVecType(_graph->NumVertex(), false);
    bo[vo] = true;
    Label lo(_GenLabelId(), vo, zero_vec, _Heuristic(_vo, bo), bo);
    _res.num_expd++;
    // std::cout << "num of labels: " << _res.num_generated_labels << std::endl;
    _label[lo.id] = lo;
    _parent[lo.id] = -1; 

    _open.push(lo);

    auto tstart = std::chrono::high_resolution_clock::now();
    while (!_open.empty()) {
				if (_open.size() > _res.max_qsize) _res.max_qsize = _open.size();

				auto tcur = std::chrono::high_resolution_clock::now();
				auto dur = std::chrono::duration<double>(tcur - tstart).count();
				if (dur > _tlimit) {
					_res.timeout = true;
					break;
				}
        Label l = _open.top();
        _open.pop();
				if (_FrontierCheck(l)) {
					_res.frontier_pruned++;
					continue;
				}
				if (_SolutionCheck(l)) {
					_res.sol_pruned++;
					continue;
				}
        _UpdateFrontier(l);
        if (_IsDone(l) && l.v == _vd) {
            // std::cout << "solution found: " << l << std::endl;
            solu->Update(l);
        } else {
          auto succs = _graph->GetSuccs(l.v);
          auto cvecs = _graph->GetSuccCosts(l.v);
          _res.num_expd++;
          for (int idx = 0; idx < succs.size(); idx++)
          {
            auto u = succs[idx];
            CostVec gu = l.g + cvecs[idx];
            gu[1] += std::max(_tw[l.v].first - l.g[1], 0.0) + _service_time[l.v];
            if (_key_nodes.find(l.v) != _key_nodes.end()) {
							// update 2nd objective
							// if the u is a key node, then apply penalty on 'later finish'
              gu[0] += gu[1] - _tw[l.v].first;
            }
            // gu[1] += _service_time[l.v];
            BinaryServiceVec bu = l.b;
            bu[u] = !bu[u];
            Label l2(_GenLabelId(), u, gu, gu + _Heuristic(u, bu), bu);
            // std::cout << "l2: " << l2 << " parent: " << l.id << std::endl;
            _label[l2.id] = l2;
            _parent[l2.id] = l.id;
						_res.num_gen++;
            if (_FeaCheck(l2))
            {
							_res.fea_pruned++;
              continue;
            }
            if (_PostCheck_1(l2)) {
							_res.post_pruned++;
              continue;
            }
						if (_FrontierCheck(l2)) {
							_res.frontier_pruned++;
							continue;
						}
            if (_SolutionCheck(l2))
            {
							_res.sol_pruned++;
              continue;
            }
            if (_PostCheck_2(l2)) {
							_res.post2_pruned++;
              continue;
            }
            _open.push(l2);
            // std::cout << "l2: " << l2 << std::endl;
          }
          // std::cout << "size of open: " << _open.size() << std::endl;
        }     
    }
    _PostProcRes();
    return 1;
}

int RunMOTSPTW(rzq::basic::PlannerGraph* g, TimeWindowVec tw, std::vector<double> st, long vo, long vd, std::set<long> keys, MOTSPTWResult* res, double tlimit) {
    int ret_flag = 0;
    auto planner = MOTSPTW();
    planner.SetGraphPtr(g);
		planner.SetTimeLimit(tlimit);
    planner.SetTimeWindow(tw);
    planner.SetServiceTime(st);
    planner._key_nodes = keys;
    // planner.InitHeu(vd);
    ret_flag = planner.Search(vo, vd);
    *res = planner.GetResult();
    return ret_flag;
}

}
}

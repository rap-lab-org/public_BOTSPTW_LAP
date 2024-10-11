#include "search_motsptw.hpp"
#include <algorithm>
#include <cassert>
#include <set>
#include <chrono>

namespace rzq {
namespace search {

std::vector<int> _taskids;

std::ostream &operator<<(std::ostream &os, Label &l) {
  std::string s;
  s = "{id:" + std::to_string(l.id) + ",v:" + std::to_string(l.v) +
      ",b:" + l.b.to_str() + ",g:" + ToString(l.g) + ",f:" + ToString(l.f) +
      "}";
  os << s;
  return os;
};

Frontier::Frontier() { return; }

Frontier::~Frontier() { return; }

// is l1 dominate l2
bool dominates(const Label &l1, const Label &l2) {
  if (l1.g[0] > l2.g[0] || l1.g[1] > l2.g[1]) {
    return false;
  }
	if (l2.b.is_subset(l1.b)) return true;
  return false;
}

bool Frontier::Check(const Label &l) const {
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
  for (auto it = labels.begin(); it != labels.end();) {
		if (dominates(l, *it)) {
			it = labels.erase(it);
		}
		else {
			it++;
		}
  }
  labels.push_back(l);
  return;
}

MOTSPTW::MOTSPTW(){};
MOTSPTW::~MOTSPTW() {
  for (int idx = 0; idx < _alpha.size(); idx++) {
    delete _alpha[idx];
  }
  delete solu;
  return;
}

void MOTSPTW::SetGraphPtr(basic::PlannerGraph *g) { _graph = g; };

void MOTSPTW::SetTimeWindow(TimeWindowVec tw) { _tw = tw; };

void MOTSPTW::SetServiceTime(std::vector<double> st) { _service_time = st; };

// void MOTSPTW::InitHeu(long vd) {
//   // SimpleTimer timer;
//   // timer.Start();

//   // _dijks.resize(_graph->CostDim());
//   // for (size_t i = 0; i<_graph->CostDim(); i++) {
//   //   _dijks[i].SetGraphPtr(_graph);
//   //   _dijks[i].ExhaustiveBackwards(vd,
//   std::numeric_limits<double>::infinity(), i) ;
//   // }
//   // _res.rt_initHeu = timer.GetDurationSecond();

//   // G_DOM_CHECK_COUNT = 0; // reset dom check counter.
//   return ;
// };

CostVec MOTSPTW::_Heuristic(long v, const BinaryServiceVec &b) {
  auto out = CostVec(_graph->CostDim(), 0);
  double time_h = 0;

  // std::vector<bool> inMST(_graph->NumVertex(), false);
  // std::vector<double> key(_graph->NumVertex(),
  // std::numeric_limits<double>::infinity()); std::vector<long> targets;

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

MOTSPTWResult MOTSPTW::GetResult() const { return _res; };

long MOTSPTW::_GenLabelId() {

  long out = _label_id_gen++;

  if (_label_id_gen > __vec_alloc_total) {
    __vec_alloc_total += __vec_alloc_batch;
    _label.resize(__vec_alloc_total);
    _parent.resize(__vec_alloc_total);
    if (__vec_alloc_batch < __vec_alloc_batch_max) {
      __vec_alloc_batch += __vec_alloc_batch;
    }
  }

  return out;
};

bool MOTSPTW::_FrontierCheck(const Label &l) const {
  auto res = _alpha[l.v]->Check(l);
  return res;
};

bool MOTSPTW::_SolutionCheck(const Label &l) const {
  auto temp = solu->Check(l);
  return temp;
};

bool MOTSPTW::_FeaCheck(const Label &l) const {
  if (l.g[1] + _service_time[l.v] > _tw[l.v].second) {
    return true;
  }
  return false;
};

bool MOTSPTW::_PostCheck_1(const Label &l) const {
	l.b.all_notexists(_taskids);
  for (const auto& i: _taskids) {
    // For all nodes that have not been visited, check if the time window is
    // violated.
    if (l.g[1] + _graph->GetCost(l.v, i)[1] + _service_time[i] >
        _tw[i].second) {
      return true;
    }
  }
  return false;
};


void MOTSPTW::dbg_postcheck_N(const Label &l, const std::vector<int> &todo,
                              const std::vector<double> &ddl,
                              const std::vector<double> &travel,
                              const std::vector<double> &service) const {
  std::vector<int> idx;
  for (int i = 0; i < todo.size(); i++)
    idx.push_back(i);
  bool all_infeasible = true;
  std::vector<int> seq;
  do {
    seq.clear();
    for (int i = 0; i < todo.size(); i++)
      seq.push_back(todo[idx[i]]);
    double t = l.g[1];
    int cur = l.v;
    bool infeasible = false;
    // printf("t@cur: %f\n", l.g[1]);
    for (auto v : seq) {
      // printf("from %d to %d: %f\n", cur, v, _graph->GetCost(cur, v)[1]);
      t = std::max(t + _graph->GetCost(cur, v)[1], _tw[v].first) +
          _service_time[v];
      // printf("cur: %d, next: %d t@next: %f, tw:[%f, %f]\n", cur, v, t,
             // _tw[v].first, _tw[v].second);
      if (t > _tw[v].second) {
        infeasible = true;
        break;
      }
      cur = v;
    }
    if (infeasible == false) {
      // find a feasible case
      all_infeasible = false;
      break;
    }
  } while (std::next_permutation(idx.begin(), idx.end()));

  // given set `todo`, there should be no feasible tsptw
  if (all_infeasible == false) {
    printf("[label] v:%ld t:%f\n", l.v, l.g[1]);
    for (auto v : seq)
      printf("%d ", v);
    printf("\n");
    printf("sorted ddl:\n");
		for (auto v: ddl) printf("%f ", v); printf("\n");
		for (auto e: travel) printf("%f ", e); printf("\n");

		auto t = l.g[1];
		for (int i=0; i<ddl.size(); i++) {
			printf("cur t: %f\n", t);
			t += travel[i] + service[i];
			if (t > ddl[i]) {
				printf("here infeasible\n");
			}
		}
  }
  assert(all_infeasible == true);
}

bool MOTSPTW::_PostCheck_N(const Label &l, int n = 2) const {
  std::vector<int> todo;
  std::vector<double> ddl, sub_travel_time, sub_service_time;
  for (int i = 0; i < l.b.size() - 1; i++)
    if (l.b.get(i) == false) {
      todo.push_back(i);
    }
  if (n > todo.size())
    n = todo.size();
  std::random_shuffle(todo.begin(), todo.end());
  todo.erase(todo.begin() + n, todo.end());

  assert(todo.size() == n);

  for (int i = 0; i < n; i++) {
    auto u = todo[i];
    sub_travel_time.push_back(_graph->GetCost(l.v, u)[1]);
    for (int j = i + 1; j < n; j++) {
      auto v = todo[j];
      sub_travel_time.push_back(_graph->GetCost(u, v)[1]);
      sub_travel_time.push_back(_graph->GetCost(v, u)[1]);
    }
  }
  if (sub_travel_time.empty())
    return false;

  for (auto v : todo) {
    sub_service_time.push_back(_service_time[v]);
  }

  for (auto v : todo) {
    ddl.push_back(_tw[v].second);
  }
  std::sort(ddl.begin(), ddl.end());
  std::sort(sub_service_time.begin(), sub_service_time.end());
  std::sort(sub_travel_time.begin(), sub_travel_time.end());

  double t = l.g[1];
  bool res = false;
  for (int i = 0; i < ddl.size(); i++) {
    t += sub_travel_time[i] + sub_service_time[i];
    if (t > ddl[i]) {
      res = true;
      break;
    };
  }
  // if (res)
  //   dbg_postcheck_N(l, todo, ddl, sub_travel_time, sub_service_time);
  return res;
};

std::vector<long> MOTSPTW::_BuildPath(long lid) {
  std::vector<long> out, out2;
  while (lid >= 0) {
    out.push_back(_label[lid].v);
    lid = _parent[lid];
  }
  for (size_t i = 0; i < out.size(); i++) {
    out2.push_back(out[out.size() - 1 - i]);
  }
  return out2;
};

void MOTSPTW::_PostProcRes() {
  for (auto lb : solu->labels) {
    long lid = lb.id;
    _res.paths[lid] = _BuildPath(lid);
    _res.costs[lid] = _label[lid].g;
  }
  return;
};

void MOTSPTW::_UpdateFrontier(Label l) { _alpha[l.v]->Update(l); };

void MOTSPTW::_InitFrontiers() {
  _alpha.resize(_graph->NumVertex()); // this requires the graph vertex are
                                      // numbered from 0 to |V|.
  for (int idx = 0; idx < _alpha.size(); idx++) {
    _alpha[idx] = new Frontier;
  }
  solu = new Frontier;
  return;
};

bool MOTSPTW::_IsDone(const Label &l) const {
	return l.b.is_all(true);
};

int MOTSPTW::Search(long vo, long vd) {
  _InitFrontiers();
  _vo = vo;
  _vd = vd;
  _res.reset();
	_taskids.reserve(_graph->NumVertex());
	_taskids.clear();
  auto zero_vec = InitVecType(_graph->CostDim(), 0.0);
  BinaryServiceVec bo(_graph->NumVertex());
  bo.set(vo, true);
  Label lo(_GenLabelId(), vo, zero_vec, _Heuristic(_vo, bo), bo);
  _res.num_expd++;
  // std::cout << "num of labels: " << _res.num_generated_labels << std::endl;
  _label[lo.id] = lo;
  _parent[lo.id] = -1;

  _open.push(lo);

  auto tstart = std::chrono::high_resolution_clock::now();
  while (!_open.empty()) {
    if (_open.size() > _res.max_qsize)
      _res.max_qsize = _open.size();

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
			// iterate all unfinished tasks
      for (int idx = 0; idx < succs.size(); idx++) {
        auto u = succs[idx];
				if (l.b.get(u)) continue;
        CostVec gu = l.g + cvecs[idx];
        gu[1] += std::max(_tw[l.v].first - l.g[1], 0.0) + _service_time[l.v];
        if (_key_nodes.find(l.v) != _key_nodes.end()) {
          // update 2nd objective
          // if the u is a key node, then apply penalty on 'later finish'
          gu[0] += gu[1] - _tw[l.v].first;
        }
        // gu[1] += _service_time[l.v];
        BinaryServiceVec bu(l.b);
        bu.set(u, true);
        Label l2(_GenLabelId(), u, gu, gu + _Heuristic(u, bu), bu);
        // std::cout << "l2: " << l2 << " parent: " << l.id << std::endl;
        _label[l2.id] = l2;
        _parent[l2.id] = l.id;
        _res.num_gen++;
        if (_FeaCheck(l2)) {
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
        if (_SolutionCheck(l2)) {
          _res.sol_pruned++;
          continue;
        }
        if (_PostCheck_N(l2, 2)) {
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

int RunMOTSPTW(rzq::basic::PlannerGraph *g, TimeWindowVec tw,
               std::vector<double> st, long vo, long vd, std::set<long> keys,
               MOTSPTWResult *res, double tlimit) {
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

} // namespace search
} // namespace rzq

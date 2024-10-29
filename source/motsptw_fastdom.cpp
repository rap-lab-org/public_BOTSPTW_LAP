#include "motsptw_fastdom.hpp"
#include <random>
#include <cassert>
#include <fstream>
namespace rzq {
namespace search_fastdom {

std::vector<int> _taskids;
const bool DEBUG = false;

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

FastFrontierVec::FastFrontierVec() { return; }

FastFrontierVec::~FastFrontierVec() { return; }

FastFrontierMap::FastFrontierMap() { return; }

FastFrontierMap::~FastFrontierMap() { return; }

// is l1 dominate l2
bool Frontier::dominates(const Label &l1, const Label &l2) const {
  if (l1.g[0] > l2.g[0]) {
    return false;
  }
	if (l2.b.is_subset(l1.b)) {
		if (DEBUG) {
			std::cout << "  DOM: [" << l1.id << " " << l1.b.to_str() << " g:" << l1.g 
				<< "] dominates [" 
				<< l2.id << " " << l2.b.to_str() << " g:" << l2.g  << "]" << std::endl;
		}
		return true;
	}
  return false;
}

// is l1 dominate l2
bool FastFrontierVec::dominates(const Label &l1, const Label &l2) const {
	// precondition:
	// l1.g[0] <= l2.g[0]
	if (l2.b.is_subset(l1.b)) {
		if (DEBUG) {
			std::cout << "  DOM:[" << l1.id << " " << l1.b.to_str() << " g:" << l1.g 
				<< "] dominates [" 
				<< l2.id << " " << l2.b.to_str() << " g:" << l2.g  << "]" << std::endl;
		}
		return true;
	}
  return false;
}

bool FastFrontierMap::dominates(const Label &l1, const Label &l2) const {
	// precondition:
	// l1.g[0] <= l2.g[0]
	if (l2.b.is_subset(l1.b)) {
		if (DEBUG) {
			std::cout << "  DOM:[" << l1.id << " " << l1.b.to_str() << " g:" << l1.g 
				<< "] dominates [" 
				<< l2.id << " " << l2.b.to_str() << " g:" << l2.g  << "]" << std::endl;
		}
		return true;
	}
  return false;
}

bool Frontier::Check(const Label &l) const {
  if (labels.empty()) {
    return false;
  }
	for (const auto& it: labels) {
		if (dominates(it, l)) {
			if (DEBUG) {
				std::cout << "  prune in check: " << l.id << " is dominated by " << it.id << std::endl;
			}
			return true;
		}
		if (key(it) > key(l)) break;
	}
  return false;
}

bool FastFrontierVec::Check(const Label& l) const {
	// is l dominated by any existing labels?
	for (const auto& it: NDs) {
		if (key(it) <= key(l) && dominates(it, l)) {
			if (DEBUG) {
				std::cout << "  prune in check: " << l.id << " is dominated by " << it.id << std::endl;
			}
			return true;
		}
	}
	return false;
}

bool FastFrontierMap::Check(const Label& l) const {
	for (const auto& [k, lid]: NDs) {
		if (k > key(l)) break;
		const Label& it = _all_labels->at(lid);
		if (dominates(it, l)) {
			if (DEBUG) {
				std::cout << "  prune in check: " << l.id << " is dominated by " << it.id << std::endl;
			}
			return true;
		}
	}
	return false;
}

void Frontier::Update(const Label& l) {
  if (labels.empty()) {
    labels.push_back(l);
    return;
  }
	int rmcnt = 0;
	auto it = labels.rbegin();
	auto tail = labels.rbegin();
	while (it != labels.rend() && key(*it) == key(l)) {
		if (dominates(l, *it)) {
			// move the item to tail, later will be removed from the tail
			if (it != tail)
				std::swap(*it, *tail);
			++tail;
			++rmcnt;
		}
		++it;
	}
	while (rmcnt>0) {
		if (DEBUG) {
			auto l2 = labels.back();
			std::cout << "  erase in update: " << l2.id << " is dominated by " << l.id << std::endl;
		}
		labels.pop_back();
		--rmcnt;
	}
  labels.push_back(l);
  return;
}

void FastFrontierVec::Update(const Label& newl) {
	// l cannot dominate any item before 'it' 
	int rmcnt = 0;
	auto it = NDs.rbegin();
	auto tail = NDs.rbegin();
	while (it != NDs.rend()) {
		if (key(*it) >= key(newl) && dominates(newl, *it)) {
			if (it != tail) 
				std::swap(*it, *tail);
			++tail;
			++rmcnt;
		}
		++it;
	}
	while (rmcnt>0) {
		if (DEBUG) {
			auto l2 = NDs.back();
			std::cout << "  erase in update: " << l2.id << " is dominated by " << newl.id << std::endl;
		}
		NDs.pop_back();
		--rmcnt;
	}
	labels.push_back(newl);
	NDs.push_back(newl);
}

void FastFrontierMap::Update(const Label& newl) {
	auto it = NDs.lower_bound(key(newl));
	while (it != NDs.end()) {
		const auto& [k, idx] = *it;
		const Label& l = _all_labels->at(idx);
		assert (key(newl) <= key(l));
		if (dominates(newl, l)) {
			if (DEBUG) {
				std::cout << "  erase in update: " << l.id << " is dominated by " << newl.id << std::endl;
			}
			it = NDs.erase(it);
		} else {
			it++;
		}
	}
	NDs.insert(it, {key(newl), newl.id});
	lids.push_back(newl.id);
}

MOTSPTW::MOTSPTW(){};
MOTSPTW::~MOTSPTW() {
  for (int idx = 0; idx < _alpha.size(); idx++) {
    delete _alpha[idx];
  }
  delete solu;
  return;
}

void MOTSPTW::SetGraphPtr(Grid *g) { _graph = g; };

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

double MOTSPTW::_HeurArrivalTime(const Label& l) const {
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
	return 0;
}

double MOTSPTW::_HeurPenalty(const Label& l) const {
	double res = 0;
	std::vector<std::pair<double, int>> st;
	for (const auto& k: _key_nodes) {
		if (l.b.get(k) == 0) {
			st.push_back({_tw[k].first, k});
		}
	}
	std::sort (st.begin(), st.end());
	double curt = l.g[1];
	int curv = l.v;
	for (const auto& it: st) {
		const auto& id = it.second;
		curt += _graph->at(curv).at(id);
		curt = std::max(curt, _tw[id].first);
		res += curt;
		curv = id;
	}
	return res;
}

CostVec MOTSPTW::_Heuristic(const Label& l) const {
  auto out = CostVec(DIM, 0);
	if (use_heur) out[0] = _HeurPenalty(l);
  // out[1] = _HeurArrivalTime(l);
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
    if (l.g[1] + _graph->at(l.v).at(i) + _service_time[i] >
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
      t = std::max(t + _graph->at(cur).at(v), _tw[v].first) +
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
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(todo.begin(), todo.end(), g);
  todo.erase(todo.begin() + n, todo.end());

  assert(todo.size() == n);

  for (int i = 0; i < n; i++) {
    auto u = todo[i];
    sub_travel_time.push_back(_graph->at(l.v).at(u));
    for (int j = i + 1; j < n; j++) {
      auto v = todo[j];
      sub_travel_time.push_back(_graph->at(u).at(v));
      sub_travel_time.push_back(_graph->at(v).at(u));
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
	for (const auto& lb: solu->get_labels()) {
  // for (const auto& [k, lb] : solu->labels) {
    long lid = lb.id;
    _res.paths[lid] = _BuildPath(lid);
    _res.costs[lid] = _label[lid].g;
  }
	std::ofstream fout("./frontiers.csv");
	fout << "v,NDs,labels" << std::endl;
	for (int v=0; v<_graph->size(); v++) {
		fout << v << "," << _alpha[v]->get_NDs() << ","  << _alpha[v]->labels.size() << std::endl;
	}
  return;
};

void MOTSPTW::_UpdateFrontier(Label l) { _alpha[l.v]->Update(l); };

void MOTSPTW::_InitFrontiers() {
  _alpha.resize(_graph->size()); // this requires the graph vertex are
                                      // numbered from 0 to |V|.
  for (int idx = 0; idx < _alpha.size(); idx++) {
    _alpha[idx] = new SearchFrontier;
		_alpha[idx]->_all_labels = &_label;
  }
  solu = new SearchFrontier;
	solu->_all_labels = &_label;
  return;
};

bool MOTSPTW::_IsDone(const Label &l) const {
	return l.b.is_all(true);
};

int MOTSPTW::Search(long vo, long vd) {
  _InitFrontiers();
	_InitGapVerts();
  _vo = vo;
  _vd = vd;
  _res.reset();
	_taskids.reserve(_graph->size());
	_taskids.clear();
  auto zero_vec = InitVecType(DIM, 0.0);
  BinaryServiceSet bo(_graph->size());
  bo.set(vo, true);
  Label lo(_GenLabelId(), vo, zero_vec, zero_vec, bo);
	lo.f = lo.g + _Heuristic(lo);
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
      solu->Update(l);
    } else {
			int n = _graph->size();
      _res.num_expd++;
			if (DEBUG) std::cout << "expd: " << l << std::endl;
			// iterate all unfinished tasks
			for (int u = 0; u < n; u++) {
				if (l.b.get(u)) continue;
				if (use_gap_prune && _GapVertCheck(l, u)) {
					_res.post2_pruned++;
					continue;
				}
				auto dvu = _graph->at(l.v).at(u);
        CostVec gu = {l.g[0], l.g[1]};
				gu[1] = std::max(l.g[1]+dvu, _tw[u].first);
        if (_key_nodes.find(u) != _key_nodes.end()) {
          // update 2nd objective
          // if the u is a key node, then apply penalty on 'later finish'
          gu[0] += gu[1] - _tw[u].first + 1;
        }
        // gu[1] += _service_time[l.v];
        BinaryServiceSet bu(l.b);
        bu.set(u, true);
        Label l2(_GenLabelId(), u, gu, gu, bu);
				l2.f = l2.g + _Heuristic(l2);
        if (DEBUG) std::cout << "  gen: " << l2 << " parent: " << l.id << std::endl;
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
				l2.g[1] += _service_time[l2.v];
        // if (_FrontierCheck(l2)) {
        //   _res.frontier_pruned++;
        //   continue;
        // }
        // if (_SolutionCheck(l2)) {
        //   _res.sol_pruned++;
        //   continue;
        // }
        // if (_PostCheck_N(l2, 2)) {
        //   _res.post2_pruned++;
        //   continue;
        // }
        _open.push(l2);
      }
    }
  }
	auto tcur = std::chrono::high_resolution_clock::now();
	_res.runtime = std::chrono::duration<double>(tcur - tstart).count();
  _PostProcRes();
  return 1;
}


bool MOTSPTW::isValid(const std::vector<long>& path,
		const CostVec& cost) {
	std::set<long> vis;
	double curt = 0;
	long curv = _vo;
	vis.insert(curv);
	auto it = path.begin();
	// path must start with _vo
	if ((*it) != _vo) {
		if (DEBUG) {
			std::cout << "start is " << *it << " expect: " << _vo << std::endl;
		}
		return false;
	}
	double panelty = 0;
	while ((++it) != path.end()) {
		const auto& nxtv = *it;
		curt += _graph->at(curv).at(nxtv);
		// reach the next vert at time `curt`
		curt = std::max(curt, _tw[nxtv].first) + _service_time[nxtv];
		curv = nxtv;
		// this vert has been visited before
		if (vis.count(curv)) {
			if (DEBUG) std::cout << "v:" << curv << " has been visited multiple times" << std::endl;
			return false;
		}
		if (curt > _tw[curv].second) {
			if (DEBUG) {
				std::cout << "v:" << curv << " t: " << curt 
				<< " over due: " << _tw[curv].second << std::endl;
			}
			return false;
		}
		if (_key_nodes.count(curv)) {
			panelty += curt - _tw[curv].first + 1;
		}
		vis.insert(curv);
	}
	// there must be some unvisited vertexes
	if (vis.size() != path.size()) {
		return false;
	}
	const double EPS = 1e-3;
	if (fabs(cost[0]-panelty) > EPS) {
		if (DEBUG) {
			std::cout << "Inconsistent panelty: " << panelty << " expected: " << cost[0]  << std::endl;
		}
		return false;
	}
	if (fabs(curt-cost[1]) > EPS) {
		if (DEBUG) {
			std::cout << "Inconsistent arrival time: " << curt << " expected: " << cost[1]  << std::endl;
		}
		return false;
	}
	return true;
}

int RunMOTSPTW(Grid* g, TimeWindowVec tw,
               std::vector<double> st, long vo, long vd, std::set<long> keys,
               MOTSPTWResult *res, bool use_gap_prune, bool use_heur, double tlimit) {
  int ret_flag = 0;
  auto planner = MOTSPTW();
  planner.SetGraphPtr(g);
  planner.SetTimeLimit(tlimit);
  planner.SetTimeWindow(tw);
  planner.SetServiceTime(st);
  planner._key_nodes = std::unordered_set<long>(keys.begin(), keys.end());
  // planner.InitHeu(vd);
	planner.use_gap_prune = use_gap_prune;
	planner.use_heur = use_heur;
  ret_flag = planner.Search(vo, vd);
  *res = planner.GetResult();
	if (!planner.isValidAll()) {
		std::cout << "invalid plan" << std::endl;
		exit(1);
	}
  return ret_flag;
}

} // namespace search
} // namespace rzq

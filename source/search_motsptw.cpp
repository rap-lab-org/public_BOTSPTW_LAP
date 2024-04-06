#include "search_motsptw.hpp"
#include <set>
#include <memory>
#include <fstream>

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


bool Frontier::Check(const Label& l) const {
  if (labels.empty()) {
    return false;
  }
  for (auto it = labels.begin(); it != labels.end(); it++) {
    if (it->g[0] > l.g[0] || it->g[1] > l.g[1]) {
      return false;
    }
    for (int i = 0; i < l.b.size(); i++) {
      if (l.b[i] && !it->b[i]) {
        return false;
      }
    }
    std::cout << "here" << std::endl;
    return true;
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
    if (it->g[0] < l.g[0] || it->g[1] < l.g[1]) {
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

void MOTSPTW::InitHeu(long vd) {
  // SimpleTimer timer;
  // timer.Start();

  // _dijks.resize(_graph->CostDim());
  // for (size_t i = 0; i<_graph->CostDim(); i++) {
  //   _dijks[i].SetGraphPtr(_graph);
  //   _dijks[i].ExhaustiveBackwards(vd, std::numeric_limits<double>::infinity(), i) ;
  // }
  // _res.rt_initHeu = timer.GetDurationSecond();

  // G_DOM_CHECK_COUNT = 0; // reset dom check counter.
  return ;
};

CostVec MOTSPTW::_Heuristic(long v) {
  auto out = CostVec(_graph->CostDim(), 0);

  // for (size_t cdim = 0; cdim < out.size(); cdim++) {
  //   out[cdim] = _dijks[cdim].GetDistValue(v);
  //   // out[cdim] = 0;
  //   if (out[cdim] < 0) {
  //     throw std::runtime_error( "[ERROR], unavailable heuristic !?" );
  //   }
  // }
  // std::cout << " h(" << v << ") = " << out << std::endl;
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
  if (l.b[l.v] == true) {
    return true;
  }
  return false;
};

bool MOTSPTW::_PostCheck(const Label& l) const {
  for (int i = 0; i < l.b.size(); i++) {
    // For all nodes that have not been visited, check if the time window is violated.
    if (l.g[1] + _graph->GetCost(l.v, i)[1] + _service_time[i] > _tw[i].second) {
      return true;
    }
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
  for (auto it : l.b) {
    if (!it) {
      return false;
    }
  }
  return true;
};

int MOTSPTW::Search(long vo, long vd) {
    _InitFrontiers();
    _vo = vo;
    _vd = vd;
    auto zero_vec = InitVecType(_graph->CostDim(), 0.0);
    BinaryServiceVec bo = InitVecType(_graph->NumVertex(), false);
    Label lo(_GenLabelId(), vo, zero_vec, _Heuristic(_vo), bo);
    _label[lo.id] = lo;
    _parent[lo.id] = -1; 

    _open.push_back(lo);

    while (!_open.empty()) {
        Label l = *_open.begin();
        _open.erase(_open.begin());
        if ( _FrontierCheck(l) || _SolutionCheck(l) ) {
          // std::cout << "l: " << l << std::endl;
          continue;
        }
        _UpdateFrontier(l);
        if (l.v == vd) {
            l.b[vd] = true;
            if (_IsDone(l)) {
              solu->Update(l);
            }
        } else {
          auto succs = _graph->GetSuccs(l.v);
          auto cvecs = _graph->GetSuccCosts(l.v);
          for (int idx = 0; idx < succs.size(); idx++)
          {
            auto u = succs[idx];
            CostVec gu = l.g + cvecs[idx];
            gu[1] += std::max(_tw[l.v].first - l.g[1], 0.0) + _service_time[l.v];
            BinaryServiceVec bu = l.b;
            bu[l.v] = true;
            Label l2(_GenLabelId(), u, gu, gu + _Heuristic(u), bu);
            _label[l2.id] = l2;
            _parent[l2.id] = l.id;
            if (_FrontierCheck(l2) || _SolutionCheck(l2))
            {
              // std::cout << "l2: " << l2 << std::endl;
              continue;
            }
            if (_FeaCheck(l2))
            {
              // std::cout << "l2: " << l2 << std::endl;
              continue;
            }
            if (_PostCheck(l2)) {
              // std::cout << "l2: " << l2 << std::endl;
              continue;
            }
            _open.push_back(l2);
          }
          // std::cout << "size of open: " << _open.size() << std::endl;
        }     
    }
    _PostProcRes();
    return 1;
}

int RunMOTSPTW(rzq::basic::PlannerGraph* g, TimeWindowVec tw, std::vector<double> st, long vo, long vd, MOTSPTWResult* res) {
    int ret_flag = 0;
    auto planner = MOTSPTW();
    planner.SetGraphPtr(g);
    planner.SetTimeWindow(tw);
    planner.SetServiceTime(st);
    planner.InitHeu(vd);
    ret_flag = planner.Search(vo, vd);
    *res = planner.GetResult();
    return ret_flag;
}

}
}
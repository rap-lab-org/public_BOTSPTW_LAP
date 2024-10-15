#pragma once
// #include "search.hpp"
// #include "search_dijkstra.hpp"
#include "graph.hpp"
#include "taskset.hpp"
#include <map>
#include <unordered_map>
#include <set>
#include <vector>
#include <queue>

namespace rzq {
namespace search {

using namespace rzq::basic;

typedef std::vector<double> CostVec;
// typedef ServiceVec BinaryServiceSet;
typedef ServiceBits BinaryServiceSet;
typedef std::vector<std::pair<double, double>> TimeWindowVec;

struct Label {
	Label() { };
  Label(long id0, long v0, const CostVec& g0, const CostVec& f0, const BinaryServiceSet& b0): id(id0), v(v0), g(g0), f(f0), b(b0) { };
  long id; // label's id, make it easy to look up.
  long v;
  BinaryServiceSet b;
  CostVec g;
  CostVec f;
};

class CompareLabel {
public:
  bool operator()(const Label& l1, const Label& l2) {
		if (l1.f[1] == l2.f[1]) return l1.f[0] > l2.f[0];
    return l1.f[1] > l2.f[1];
  }
};

typedef std::pair<long, double> PLD;

class ComparePLD {
public:
  bool operator()(const PLD& p1, const PLD& p2) {
    return p1.second > p2.second;
  }
};

struct MOTSPTWResult {
  std::unordered_map< long, std::vector<long> > paths;
  std::unordered_map< long, CostVec > costs;
	bool timeout;
	long max_qsize;
  long num_expd, num_gen;
	long frontier_pruned, sol_pruned, fea_pruned, post_pruned;
	long post2_pruned;

	void reset() {
		timeout = false;
		num_gen = num_expd = frontier_pruned = sol_pruned = fea_pruned = post_pruned = 0;
		max_qsize = post2_pruned = 0;
	}
};

std::ostream& operator<<(std::ostream& os, Label& l) ;

class Frontier {
public:
  Frontier();
  virtual ~Frontier();
  virtual bool Check(const Label& l) const;
  virtual void Update(Label l);
  std::vector<Label> labels;

	 bool dominates(const Label& l1, const Label& l2) const; 

		inline std::vector<Label> get_labels() { return labels; }
		//  key field is non-decreasing in labels;
		inline long key(const Label& l) const { return l.g[1]; }

		inline int get_NDs() { return labels.size(); }
};

class FastFrontier {
public:
	using LID = int; // label id
	FastFrontier();
	~FastFrontier();
	bool Check(const Label& l) const;
	void Update(Label l);
	// std::multimap<long, LID> NDs;
	std::vector<Label> NDs;
	std::vector<Label> labels;

	// g0: penalty, g1: arrival time, 
	// use g0 as key in map, g1 is ignored due to dimension reduction
	inline long key(const Label& l) const { return l.g[0]; }
	bool dominates(const Label& l1, const Label& l2) const; 

	inline std::vector<Label> get_labels() { 
		return labels;
	}

	inline int get_NDs() { return NDs.size(); }
};

class MOTSPTW {

typedef Frontier SearchFrontier;
// typedef FastFrontier SearchFrontier;

public:
    MOTSPTW(); //
    virtual ~MOTSPTW(); //
    virtual void SetGraphPtr(basic::PlannerGraph* g); //
    virtual void SetTimeWindow(TimeWindowVec tw); //
    virtual void SetServiceTime(std::vector<double> st); //
    // virtual void InitHeu(long vd); //
    virtual int Search(long vo, long vd) ;
    virtual MOTSPTWResult GetResult() const ; //
		inline void SetTimeLimit(double t) {
			_tlimit = t;
		}
    std::set<long> _key_nodes;
protected:
    virtual CostVec _Heuristic(long v, const BinaryServiceSet& b) ; //

    // this method needs to new frontiers, which depend on the specific #obj.
    virtual void _UpdateFrontier(Label l) ; //

    virtual long _GenLabelId() ; //

    virtual bool _FrontierCheck(const Label& l) const; //
  
    virtual bool _SolutionCheck(const Label& l) const; //

    virtual bool _FeaCheck(const Label& l) const; //

    virtual bool _PostCheck_1(const Label& l) const;

    virtual bool _PostCheck_N(const Label& l, int n) const;

    virtual void _PostProcRes(); //

    virtual std::vector<long> _BuildPath(long lid) ; //

    virtual void _InitFrontiers() ; //

    virtual bool _IsDone(const Label& l) const; //

		void dbg_postcheck_N(const Label& l, const std::vector<int>& todo,
				const std::vector<double>& ddl,
				const std::vector<double>& travel,
				const std::vector<double>& service) const;

    basic::PlannerGraph* _graph;
    std::vector< SearchFrontier* > _alpha;
    SearchFrontier* solu;
    TimeWindowVec _tw;
    std::vector<double> _service_time;
		double _tlimit = 300; // time limit in seconds

    size_t __vec_alloc_total = 0;
    size_t __vec_alloc_batch = 1024;
    size_t __vec_alloc_batch_max = 1024*4;

    long _label_id_gen = 0;
    long _vo = -1, _vd = -1;
    std::priority_queue<Label, std::vector<Label>, CompareLabel> _open;

    // std::unordered_map<long, Label> _label;
    std::vector<Label> _label; 

    // std::unordered_map<long, long> _parent;
    std::vector<long> _parent;

    MOTSPTWResult _res;
    // std::vector<rzq::search::Dijkstra> _dijks;

};

int RunMOTSPTW(basic::PlannerGraph* g, TimeWindowVec tw, std::vector<double> st, long vo, long vd, std::set<long> keys, MOTSPTWResult* res, double tlimit=300);


}
};

#include "search.hpp"
#include "search_dijkstra.hpp"

#include <unordered_map>
#include <unordered_set>
#include <set>
#include <vector>
#include <queue>
#include <functional>  

namespace rzq {
namespace search {

using namespace rzq::basic;

typedef std::vector<double> CostVec;
typedef std::vector<bool> BinaryServiceVec;
typedef std::vector<std::pair<double, double>> TimeWindowVec;

struct Label {
  Label() {};
  Label(long id0, long v0, const CostVec& g0, const CostVec& f0, const BinaryServiceVec& b0) {
    id = id0; v = v0; g = g0; f = f0; b = b0;
  };
  long id; // label's id, make it easy to look up.
  long v;
  BinaryServiceVec b;
  CostVec g;
  CostVec f;
};

class CompareLabel {
public:
  bool operator()(const Label& l1, const Label& l2) {
    return l1.f[1] > l2.f[1];
  }
};

struct MOTSPTWResult {
  std::unordered_map< long, std::vector<long> > paths;
  std::unordered_map< long, CostVec > costs;
  long num_generated_labels;
};

std::ostream& operator<<(std::ostream& os, Label& l) ;

class Frontier {
public:
  Frontier();
  virtual ~Frontier();
  virtual bool Check(const Label& l) const;
  virtual void Update(Label l);
  std::vector<Label> labels;
};

class MOTSPTW {
public:
    MOTSPTW(); //
    virtual ~MOTSPTW(); //
    virtual void SetGraphPtr(basic::PlannerGraph* g); //
    virtual void SetTimeWindow(TimeWindowVec tw); //
    virtual void SetServiceTime(std::vector<double> st); //
    virtual void InitHeu(long vd); //
    virtual int Search(long vo, long vd) ;
    virtual MOTSPTWResult GetResult() const ; //
protected:
    virtual CostVec _Heuristic(long v) ; //

    // this method needs to new frontiers, which depend on the specific #obj.
    virtual void _UpdateFrontier(Label l) ; //

    virtual long _GenLabelId() ; //

    virtual bool _FrontierCheck(const Label& l) const; //
  
    virtual bool _SolutionCheck(const Label& l) const; //

    virtual bool _FeaCheck(const Label& l) const; //

    virtual bool _PostCheck_1(const Label& l) const;

    virtual bool _PostCheck_2(const Label& l) const;

    virtual void _PostProcRes(); //

    virtual std::vector<long> _BuildPath(long lid) ; //

    virtual void _InitFrontiers() ; //

    virtual bool _IsDone(const Label& l) const; //

    basic::PlannerGraph* _graph;
    std::vector< Frontier* > _alpha;
    Frontier* solu;
    TimeWindowVec _tw;
    std::vector<double> _service_time;

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
    std::vector<rzq::search::Dijkstra> _dijks;

};

int RunMOTSPTW(basic::PlannerGraph* g, TimeWindowVec tw, std::vector<double> st, long vo, long vd, MOTSPTWResult* res);


}
}

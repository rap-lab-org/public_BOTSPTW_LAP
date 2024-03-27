#include "search.hpp"
#include "search_dijkstra.hpp"

#include <unordered_map>
#include <unordered_set>
#include <set>
#include <vector>

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

struct MOTSPTWResult {
  std::unordered_map< long, std::vector<long> > paths;
  std::unordered_map< long, CostVec > costs;
};

std::ostream& operator<<(std::ostream& os, Label& l) ;

class Frontier {
public:
  Frontier();
  virtual ~Frontier();
  virtual bool Check(Label l);
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

    virtual bool _FrontierCheck(Label l) ; //
  
    virtual bool _SolutionCheck(Label l) ; //

    virtual bool _FeaCheck(Label l) ; //

    virtual void _PostProcRes(); //

    virtual std::vector<long> _BuildPath(long lid) ; //

    virtual void _InitFrontiers() ; //

    virtual bool _IsDone(Label l) ; //

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
    std::vector<Label> _open;

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

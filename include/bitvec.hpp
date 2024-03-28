#pragma once
#include <cstddef>
#include <vector>
#include <bitset>

// represent a set with 64 element
// i-th bit is 0 means not in set
// i-th bit is 1 means in set
using BitSet = std::bitset<64>;

class BitVec : public std::vector<BitSet> {
public:
  using std::vector<BitSet>::vector;
  // max number of element in set 
  size_t num;

  inline BitVec init(int n) {
    // return an empty BitVec (all bits are 0) for n element
    size_t size = (n >> 6) + 1; // ceil(n / 64);
    BitVec res(size, BitSet{0});
    res.num = n;
    return res;
  };

  // add `id` to set
  inline void addItem(int id) {
    int i = id >> 6;        // eqiv id / 64
    int j = id & (id << 6); // eqiv id % 64
    this->at(i).set(j, true);
  };

  // remove `id` from set
  inline void rmItem(int id) {
    int i = id >> 6;        // eqiv id / 64
    int j = id & (id << 6); // eqiv id % 64
    this->at(i).set(j, false);
  }

  inline bool isSubset(const BitVec& rhs) const {
    // TODO:
    return false;
  }
};

#pragma once
#include "vec_type.hpp"
#include <bitset>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>

namespace rzq {

namespace search {

class ServiceVec {
public:
  std::vector<bool> data;
  ServiceVec(){};
  ServiceVec(int n) : data(std::vector<bool>(n, false)){};
  ServiceVec(const ServiceVec &other) : data(other.data){};

  inline int size() const { return data.size(); }
  inline bool get(int idx) const { return data.at(idx); }

  inline void set(int idx, bool v) { data[idx] = v; };

  inline bool is_subset(const ServiceVec &other) const {
    // check if `this` is subset of `other`
    for (int i = 0; i < data.size(); i++) {
      if (data.at(i) && (!other.get(i)))
        return false;
    }
    return true;
  }

  inline bool is_all(bool v) const {
    for (const auto &i : data)
      if (i != v)
        return false;
    return true;
  }

  inline void all_exisits(std::vector<int>& idx) const {
  	idx.clear();
  	for (int i=0; i<data.size(); i++) if (data.at(i) == true) {
  		idx.push_back(i);
  	}
  }

	inline void all_notexists(std::vector<int>& idx) const {
		idx.clear();
  	for (int i=0; i<data.size(); i++) if (data.at(i) == false) {
  		idx.push_back(i);
  	}
	}

  std::string to_str() const { return std::ToString(data); }
};

class ServiceBits {
public:
  // support max 256 bits
  // data[0]: 0 - 63
  // data[1]: 64 - 127
  // data[2]: 128 - 191
  // data[3]: 192 - 255
  std::bitset<64> data[4];
  ServiceBits() { _size = -1; };
  ServiceBits(int n) { // init n bits to represent todo tasks
    // cannot larger than 256
    if (n > 256)
      exit(-1);
    _size = n;

    // 1. all tasks are marked as done
    data[0].set();
    data[1].set();
    data[2].set();
    data[3].set();

    // 2. mark [0, n) tasks as todo (false)
    int bid = 0;
    while (n > 0) {
      if (n > 64) {
				// set all bits to 0
        data[bid].reset();
        n -= 64;
        bid++;
      } else {
				// set [0, n) bits to 0
        data[bid] ^= (1 << n) - 1;
        break;
      }
    }
  };

  ServiceBits(const ServiceBits &other) {
    data[0] = other.data[0];
    data[1] = other.data[1];
    data[2] = other.data[2];
    data[3] = other.data[3];
    _size = other.size();
  };

  inline bool is_subset(const ServiceBits &other) const {
    return _is_subset(0, other.data[0]) && _is_subset(1, other.data[1]) &&
           _is_subset(2, other.data[2]) && _is_subset(3, other.data[3]);
  }

  inline bool is_all(bool v) const {
    // for (int i=0; i<_size; i++) if (get(i) != v) return false;
    // return true;
    if (v)
      return data[0].all() && data[1].all() && data[2].all() && data[3].all();
    else {
      int n = _size;
      int bid = 0;
      while (n > 0) {
        if (n > 64) {
          // all bits must be 0
          if (data[bid].any())
            return false;
          n -= 64;
          bid++;
        } else {
          // 0..n-1 bits must be 0
          if ((~data[bid]) != ((1 << n) - 1))
            return false;
        }
      }
    }
    return true;
  }

  inline int size() const { return _size; }

  inline void set(int pos, bool v) {
    auto bid = pos >> 6;
    auto idx = pos & ((1 << 6) - 1);
    data[bid][idx] = v;
  };

  inline bool get(int pos) const {
    auto bid = pos >> 6;
    auto idx = pos & ((1 << 6) - 1);
    return data[bid][idx];
  }

  std::string to_str() const {
    std::string res = "";
    res += data[3].to_string();
    res += data[2].to_string();
    res += data[1].to_string();
    res += data[0].to_string();
    return res;
  }

  inline bool _is_subset(int bid, const std::bitset<64> &other) const {
    // `this` is subset of `other`
    return (other & data[bid]) == data[bid];
  }

  inline void all_exisits(std::vector<int>& idx) const {
  	idx.clear();
  	for (int i=0; i<_size; i++) if (get(i) == true) {
  		idx.push_back(i);
  	}
  }

	inline void all_notexists(std::vector<int>& idx) const {
		idx.clear();
  	for (int i=0; i<_size; i++) if (get(i) == false) {
  		idx.push_back(i);
  	}
	}

private:
  int _size;
};

} // namespace search
} // namespace rzq

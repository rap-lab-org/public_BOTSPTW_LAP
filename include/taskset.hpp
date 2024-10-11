#pragma once
#include <string>
#include <vector>

namespace rzq {

namespace search {


class ServiceVec { 
public:
	std::vector<bool> data;	
	ServiceVec() { }
	ServiceVec(int n, bool v): data(std::vector<bool>(n, v)) { };
	ServiceVec(const ServiceVec& other): data(other.data) { };

	inline int size() const { return data.size(); }
	inline bool get(int idx) const { return data.at(idx); }

	inline void set(int idx, bool v) { data[idx] = v; };

	inline bool is_subset(const ServiceVec& other) const {
		// check if `this` is subset of `other`
		for (int i=0; i<data.size(); i++) {
			if (data.at(i) && (!other.get(i))) return false;
		}
		return true;
	}

	inline bool is_all(bool v) const {
		for (const auto& i: data) if (i != v)	return false;
		return true;
	}

	inline const std::vector<int>& elements(bool var) {
		_idxes.clear();
		_idxes.reserve(data.size());
		for (int i=0; i<data.size(); i++) if (data.at(i) == var) {
			_idxes.push_back(i);
		}
		return _idxes;
	}
private:
	std::vector<int> _idxes;
};

}
}

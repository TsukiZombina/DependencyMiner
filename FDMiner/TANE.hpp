#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <iterator>
#include "reader.hpp"
#include "hset.hpp"

class TANE {
public:
  using Partition = std::unordered_map<int, std::vector<int>>;

  std::vector<std::vector<int>> data;
  std::vector<int> T;
  std::unordered_map<int, std::vector<int>> C;
  std::vector<HSet> level;

  int nrow;
  int ncol;

  TANE () = default;

  void read_data(std::string& path) {
    auto r = Reader(path);
    data = std::move(r.data);
    nrow = r.nrow;
    ncol = r.ncol;
    T.resize(nrow);
  }

  void generate_next_level() {
    std::vector<HSet> new_level;
    std::unordered_set<int> visited;
    for (int i = 0; i < level.size(); ++i) {
      for (int j = i + 1; j < level.size(); ++j) {
        auto& s1 = level[i];
        auto& s2 = level[j];
        // compute how many bits are different
        auto diff = count_ones(s1.code ^ s2.code);
        if (diff == 2) {
          std::vector<int> buffer;
          std::set_union(s1.data.begin(), s1.data.end(),
                         s2.data.begin(), s2.data.end(),
                         std::back_inserter(buffer));
          auto hset = HSet(buffer);
          if (visited.find(hset.code) == visited.end()) {
            visited.insert(hset.code);
            new_level.emplace_back(hset);
          }
        }
      }
    }
    level = std::move(new_level);
  }

  // compute partition with respoect to a single column
  Partition compute_partitions(int col) {
    Partition ret;
    for (int ridx = 0; ridx < data.size(); ++ridx) {
      auto& row = data[ridx];
      auto p = ret.find(row[col]);
      if (p == ret.end()) {
        ret[row[col]] = std::vector<int>{ridx};
      } else {
        (p->second).push_back(ridx);
      }
    }
    strip_partition(ret);
    return ret;
  }

  // strip single-value partition
  void strip_partition(Partition& partitions) {
    for (auto iter = partitions.begin(); iter != partitions.end(); ) {
      if (iter->second.size() == 1) {
        iter = partitions.erase(iter);
      } else {
        ++iter;
      }
    }
  }

  Partition multiply_partitions(Partition& lhs, Partition& rhs) {
    Partition ret;
    Partition S;
    init_T();
    for (auto p: lhs) {
      for (auto ridx: p.second) {
        T[ridx] = p.first;
      }
      S[p.first] = std::vector<int>();
    }
    for (auto p: rhs) {
      for (auto ridx: p.second) {
        if (T[ridx] != -1) {
          S[T[ridx]].push_back(ridx);
        }
      }
      for (auto ridx: p.second) {
        if (S[T[ridx]].size() > 1) {
          ret[ret.size()] = std::move(S[T[ridx]]);
        }
        S[T[ridx]].clear();
      }
    }
    return ret;
  }

  std::vector<std::vector<int>> mine_fd() {
    C.clear();

    // L_1 := {{A} | A \in R}
    std::vector<HSet> level;
    for (int i = 0; i < ncol; ++i) {
      level.emplace_back(i);
    }

    // C(\empty) := R
    std::vector<int> R;
    for (int i = 0; i < ncol; ++i) {
      R.push_back(i);
    }
    C[0] = std::move(R);

    while (!level.empty()) {
      // line 6
    }

  }

  void init_T() {
    for (auto iter = T.begin(); iter != T.end(); ++iter) {
      (*iter) = -1;
    }
  }

  int count_ones(int code) {
    int ret = 0;
    while (code) {
      code = code & (code - 1);
      ++ret;
    }
    return ret;
  }
};

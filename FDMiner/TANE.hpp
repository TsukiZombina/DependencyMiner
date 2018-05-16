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
  std::vector<std::vector<int>> data;

  int nrow;
  int ncol;

  TANE () = default;

  void read_data(std::string& path) {
    auto r = Reader(path);
    data = std::move(r.data);
    nrow = r.nrow;
    ncol = r.ncol;
  }

  std::vector<HSet> generate_next_level(std::vector<HSet>& old_level) {
    std::vector<HSet> new_level;
    std::unordered_set<int> visited;
    for (int i = 0; i < old_level.size(); ++i) {
      for (int j = i + 1; j < old_level.size(); ++j) {
        auto& s1 = old_level[i];
        auto& s2 = old_level[j];
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
    return new_level;
  }

  // compute partition with respoect to a single column
  std::unordered_map<int, std::vector<int>> compute_partitions(int col) {
    std::unordered_map<int, std::vector<int>> ret;
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
  void strip_partition(std::unordered_map<int, std::vector<int>>& partitions) {
    for (auto iter = partitions.begin(); iter != partitions.end(); ) {
      if (iter->second.size() == 1) {
        iter = partitions.erase(iter);
      } else {
        ++iter;
      }
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

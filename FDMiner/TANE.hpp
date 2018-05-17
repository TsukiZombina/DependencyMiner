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
  std::vector<HSet> L;
  // partitions with respect to a column set
  std::unordered_map<int, Partition> set_part_map;
  // last item is rhs
  // note rhs is not excluded
  std::vector<std::vector<int>> FD;

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
    for (int i = 0; i < L.size(); ++i) {
      for (int j = i + 1; j < L.size(); ++j) {
        auto& s1 = L[i];
        auto& s2 = L[j];
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
            auto new_p = multiply_partitions(set_part_map[s1.code], set_part_map[s2.code]);
            set_part_map[hset.code] = std::move(new_p);
          }
        }
      }
    }
    L = std::move(new_level);
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

  void mine_fd() {
    // L_1 := {{A} | A \in R}
    L.clear();
    for (int i = 0; i < ncol; ++i) {
      L.emplace_back(i);
    }

    // init partitions
    set_part_map.clear();
    for (auto hset: L) {
      set_part_map[hset.code] = std::move(compute_partitions(hset.data[0]));
    }

    // C(\empty) := R
    std::vector<int> R;
    for (int i = 0; i < ncol; ++i) {
      R.push_back(i);
    }
    C.clear();
    C[0] = std::move(R);

    int depth = 0;
    while (!L.empty()) {
      std::cout << depth << std::endl;
      compute_dependencies();
      prune();
      generate_next_level();
      ++depth;
    }
  }

  void compute_dependencies() {
    for (auto X: L) {
      C[X.code] = std::move(full_set());
      std::vector<int> buffer;
      for (auto item: X.data) {
        buffer.clear();
        int X_without_A = X.code - std::pow(2, item);
        std::set_intersection(C[X.code].begin(), C[X.code].end(),
                              C[X_without_A].begin(), C[X_without_A].end(),
                              std::back_inserter(buffer));
        C[X.code] = std::move(buffer);
      }
    }
    for (auto X: L) {
      std::vector<int> R_minus_X;
      std::vector<int> candidates;
      std::set_intersection(C[X.code].begin(), C[X.code].end(),
                            X.data.begin(), X.data.end(),
                            std::back_inserter(candidates));
      for (auto A: candidates) {
        if (isValid(X, A)) {
          // from line 6
          auto buffer = X.data;
          buffer.push_back(A);
          FD.emplace_back(std::move(buffer));
          C[X.code].erase(std::find(C[X.code].begin(), C[X.code].end(), A));
          // remove all B in R \ X from C
          for (auto iter = C[X.code].begin(); iter != C[X.code].end(); ) {
            if (!X.contains(*iter)) { // then *iter \in R\X
              iter = C[X.code].erase(iter);
            } else {
              ++iter;
            }
          }
        }
      }
    }
  }

  bool isValid(HSet& bigX, int A) {
    auto X = bigX.remove(A);
    if (X.code == 0) {
      return false;
    }
    if (compute_eX(X) == compute_eX(bigX)) {
      return true;
    }
    return false;
  }

  int compute_eX(HSet& X) {
    auto P = set_part_map[X.code];
    int eX = 0;
    for (auto e_class: P) {
      eX += e_class.second.size();
    }
    eX -= P.size();
    return eX;
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

  std::vector<int> full_set() {
    std::vector<int> ret;
    for (int i = 0; i < ncol; ++i) {
      ret.push_back(i);
    }
    return ret;
  }

  void prune() {
    for (auto iter = L.begin(); iter != L.end(); ) {
      auto& X = *iter;
      if (C[X.code].empty()) {
        iter = L.erase(iter);
      } else if (set_part_map[X.code].empty()) { // superKey
        for (auto A: C[X.code]) {
          auto intersect = full_set();
          for (auto B: X.data) {
            std::vector<int> tmp;
            auto constructed = X.merge(A).remove(B);
            std::set_intersection(intersect.begin(), intersect.end(),
                                  constructed.data.begin(), constructed.data.end(),
                                  std::back_inserter(tmp));
            intersect = std::move(tmp);
          }
          auto tmpset = HSet(intersect);
          if (tmpset.contains(A)) {
            auto dependency = X.data;
            dependency.push_back(A);
            FD.emplace_back(dependency);
          }
        }
        iter = L.erase(iter);
      } else {
        ++iter;
      }
    }
  }
};

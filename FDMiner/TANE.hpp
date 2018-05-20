#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <iterator>
#include <utility>
#include <omp.h>
#include "reader.hpp"

inline int encode(int item) {
  return std::pow(2, item);
}

inline int encode(std::vector<int>& items) {
  int code = 0;
  for (int i = 0; i < items.size(); ++i) {
    code += std::pow(2, items[i]);
  }
  return code;
}

inline std::vector<int> decode_to_vector(int code) {
  std::vector<int> items;
  for (int i = 0; code != 0; ++i) {
    if (code % 2 == 1) {
      items.push_back(i);
    }
    code /= 2;
  }
  return items;
}

inline std::unordered_set<int> decode_to_set(int code) {
  std::unordered_set<int> items;
  for (int i = 0; code != 0; ++i) {
    if (code % 2 == 1) {
      items.insert(i);
    }
    code /= 2;
  }
  return items;
}

inline bool contains(int code, int item) {
  int s = encode(item);
  return (s & code);
}

inline int exclude_set(int code1, int code2) {
  return (code1 ^ code2);
}

inline int exclude_item(int code, int item) {
  if (contains(code, item)) {
    return (code - encode(item));
  }
  return code;
}

inline int merge_set(int code1, int code2) {
  return (code1 | code2);
}

inline int merge_item(int code, int item) {
  if (contains(code, item)) {
    return code;
  }
  return (code + std::pow(2, item));
}

inline int intersect(int code1, int code2) {
  return (code1 & code2);
}

// how many bits are different
inline int difference(int code1, int code2) {
  int ret = 0;
  int code = code1 ^ code2;
  while (code) {
    code = code & (code - 1);
    ++ret;
  }
  return ret;
}

class TANE {
public:
  using Partition = std::vector<std::vector<int>>;

  std::vector<std::vector<int>> data;
  std::vector<int> T;
  std::unordered_map<int, int> C;
  std::vector<int> L;
  std::unordered_map<int, Partition> set_part_map;
  std::vector<std::pair<int, int>> FD;

  std::unordered_map<int, std::pair<int, int>> parents;
  std::unordered_map<int, int> eX;

  Partition S;

  int nrow;
  int ncol;
  int full_set;

  TANE () = default;

  inline void read_data(std::string& path) {
    auto r = Reader(path);
    data = std::move(r.data);
    nrow = r.nrow;
    ncol = r.ncol;

    T.resize(nrow);

    full_set = 0;
    for (int i = 0; i < ncol; ++i) {
      full_set = (full_set << 1) + 1;
    }

    set_part_map.reserve(1500);
  }

  inline void generate_next_level() {
    std::vector<int> new_level;
    new_level.reserve(L.size() * L.size() / 2);
    std::unordered_set<int> visited;
    for (int i = 0; i < L.size(); ++i) {
      for (int j = i + 1; j < L.size(); ++j) {
        auto s1 = L[i];
        auto s2 = L[j];
        if (difference(s1, s2) == 2) {
          int merged = merge_set(s1, s2);
          if (visited.find(merged) == visited.end()) {
            visited.insert(merged);
            new_level.push_back(merged);
            parents[merged] = std::make_pair(s1, s2);
          }
        }
      }
    }
    L = std::move(new_level);
  }

  inline Partition one_column_partition(int col) {
    Partition tmp;
    for (int ridx = 0; ridx < data.size(); ++ridx) {
      auto cat = data[ridx][col];
      if (cat >= tmp.size()) {
        tmp.emplace_back();
      }
      tmp[cat].push_back(ridx);
    }
    Partition ret;
    for (int i = 0; i < tmp.size(); ++i) {
      if (tmp[i].size() > 1) {
        ret.emplace_back(tmp[i]);
      }
    }
    return ret;
  }

  inline void multiply_partitions(Partition& lhs, Partition& rhs, Partition& buf) {
    init_T();
    for (int cidx = 0; cidx < lhs.size(); ++cidx) {
      auto& p = lhs[cidx];
      for (int i = 0; i < p.size(); ++i) {
        T[p[i]] = cidx;
      }
    }
    while (S.size() < lhs.size()) {
      S.emplace_back();
    }
    for (int cidx = 0; cidx < rhs.size(); ++cidx) {
      auto& p = rhs[cidx];
      for (int i = 0; i < p.size(); ++i) {
        auto ridx = p[i];
        if (T[ridx] != -1) {
          S[T[ridx]].push_back(ridx);
        }
      }
      for (int i = 0; i < p.size(); ++i) {
        auto ridx = p[i];
        if (T[ridx] != -1) {
          if (S[T[ridx]].size() > 1) {
            buf.emplace_back();
            buf[buf.size() - 1] = S[T[ridx]];
          }
          S[T[ridx]].clear();
        }
      }
    }
  }

  inline void run() {
    L.clear();
    for (int i = 0; i < ncol; ++i) {
      L.emplace_back(encode(i));
    }

    set_part_map.clear();
    for (int i = 0; i < ncol; ++i) {
      set_part_map[L[i]] = std::move(one_column_partition(i));
    }

    C[0] = full_set;

    while (!L.empty()) {
      compute_dependencies();
      prune();
      generate_next_level();
    }
  }

  inline void compute_dependencies() {
    for (int i = 0; i < L.size(); ++i) {
      auto& X = L[i];
      C[X] = full_set;
      auto Xset = decode_to_vector(X);
      for (int j = 0; j < Xset.size(); ++j) {
        auto A = Xset[j];
        int X_without_A = exclude_item(X, A);
        C[X] = intersect(C[X], C[X_without_A]);
      }
    }
    for (int i = 0; i < L.size(); ++i) {
      auto& X = L[i];
      auto candidates = decode_to_vector(intersect(X, C[X]));
      for (int j = 0; j < candidates.size(); ++j) {
        auto A = candidates[j];
        if (isValid(X, A)) {
          FD.emplace_back(exclude_item(X, A), A);
          C[X] = exclude_item(C[X], A);
          auto CX_set = decode_to_vector(C[X]);
          for (int k = 0; k < CX_set.size(); ++k) {
            auto B = CX_set[k];
            if (!contains(X, B)) {
              C[X] = exclude_item(C[X], B);
            }
          }
        }
      }
    }
  }

  inline bool isValid(int bigX, int A) {
    auto X = exclude_item(bigX, A);
    if (!X) {
      return false;
    }
    if (get_eX(X) == get_eX(bigX)) {
      return true;
    }
    return false;
  }

  inline int get_eX(int X) {
    if (eX.find(X) != eX.end()) {
      return eX[X];
    }
    int tmp = compute_eX(X);
    eX[X] = tmp;
    return tmp;
  }

  inline int compute_eX(int X) {
    compute_partition_on_demand(X);
    auto P = set_part_map[X];
    int eX = 0;
    for (int i = 0; i < P.size(); ++i) {
      eX += P[i].size();
    }
    eX -= P.size();
    return eX;
  }

  inline void init_T() {
    for (auto iter = T.begin(); iter != T.end(); ++iter) {
      (*iter) = -1;
    }
  }

  inline void prune() {
    for (auto iter = L.begin(); iter != L.end(); ) {
      auto X = *iter;
      if (!C[X]) {
        iter = L.erase(iter);
      } else {
        ++iter;
      }
    }
  }

  inline void compute_partition_on_demand(int X) {
    if (set_part_map.find(X) != set_part_map.end()) {
      return;
    }
    auto& source = parents[X];

    #pragma omp sections
    {
      #pragma omp section
      {
        compute_partition_on_demand(source.first);
      }
      #pragma omp section
      {
        compute_partition_on_demand(source.second);
      }
    }

    auto& lhs = set_part_map[source.first];
    auto& rhs = set_part_map[source.second];
    if (lhs.size() < rhs.size()) {
      multiply_partitions(lhs, rhs, set_part_map[X]);
    } else {
      multiply_partitions(rhs, lhs, set_part_map[X]);
    }
  }

  void output(std::ostream& ofs) {
    for (int i = 0; i < FD.size(); ++i) {
      auto& item =FD[i];
      auto lhs = decode_to_vector(item.first);
      auto rhs = item.second;
      for (auto col: lhs) {
          if (col != rhs) {
              ofs << col + 1 << " ";
          }
      }
      ofs << "-> " << rhs + 1 << std::endl;
    }
  }
};

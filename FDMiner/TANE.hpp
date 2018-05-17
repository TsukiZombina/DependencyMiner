#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <iterator>
#include <utility>
#include "reader.hpp"

inline int encode(int item) {
  return std::pow(2, item);
}

inline int encode(std::vector<int>& items) {
  int code = 0;
  for (auto item: items) {
    code += std::pow(2, item);
  }
  return code;
}

inline int encode(std::unordered_set<int>& items) {
  int code = 0;
  for (auto item: items) {
    code += std::pow(2, item);
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
  // TODO: change this to vector of vector
  using Partition = std::unordered_map<int, std::vector<int>>;

  std::vector<std::vector<int>> data;
  std::vector<int> T;
  std::unordered_map<int, int> C;
  std::vector<int> L;
  std::unordered_map<int, Partition> set_part_map;
  std::vector<std::pair<int, int>> FD;

  int nrow;
  int ncol;
  int full_set;

  TANE () = default;

  void read_data(std::string& path) {
    auto r = Reader(path);
    data = std::move(r.data);
    nrow = r.nrow;
    ncol = r.ncol;

    T.resize(nrow);

    full_set = 0;
    // memset(&full_set, 1, sizeof(int));
    for (int i = 0; i < ncol; ++i) {
      full_set = (full_set << 1) + 1;
    }
  }

  void generate_next_level() {
    std::vector<int> new_level;
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
            auto new_p = multiply_partitions(set_part_map[s1], set_part_map[s2]);
            set_part_map[merged] = std::move(new_p);
          }
        }
      }
    }
    L = std::move(new_level);
  }

  // compute partition with respoect to a single column
  Partition one_column_partition(int col) {
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
    for (auto iter = ret.begin(); iter != ret.end(); ) {
      if (iter->second.size() == 1) {
        iter = ret.erase(iter);
      } else {
        ++iter;
      }
    }
    return ret;
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

  void run() {
    L.clear();
    for (int i = 0; i < ncol; ++i) {
      L.emplace_back(encode(i));
    }

    set_part_map.clear();
    for (int i = 0; i < ncol; ++i) {
      set_part_map[L[i]] = std::move(one_column_partition(i));
    }

    C[0] = full_set;

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
      C[X] = full_set;
      auto Xset = decode_to_vector(X);
      for (auto A: Xset) {
        int X_without_A = exclude_item(X, A);
        C[X] = intersect(C[X], C[X_without_A]);
      }
    }
    for (auto X: L) {
      auto candidates = decode_to_vector(intersect(X, C[X]));
      for (auto A: candidates) {
        if (isValid(X, A)) {
          FD.emplace_back(exclude_item(X, A), A);
          C[X] = exclude_item(C[X], A);
          auto CX_set = decode_to_vector(C[X]);
          for (auto B: CX_set) {
            if (!contains(X, B)) { // B \in R\X
              C[X] = exclude_item(C[X], B);
            }
          }
        }
      }
    }
  }

  bool isValid(int bigX, int A) {
    auto X = exclude_item(bigX, A);
    if (!X) {
      return false;
    }
    if (compute_eX(X) == compute_eX(bigX)) {
      return true;
    }
    return false;
  }

  int compute_eX(int X) {
    auto P = set_part_map[X];
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

  void prune() {
    for (auto iter = L.begin(); iter != L.end(); ) {
      auto X = *iter;
      if (!C[X]) {
        iter = L.erase(iter);
      } else {
        ++iter;
      }
    }
  }
};

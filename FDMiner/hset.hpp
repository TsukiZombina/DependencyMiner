#pragma once
#include <vector>
#include <unordered_set>

// HSet for "hashable set"
class HSet {
public:
  std::unordered_set<int> members;
  int code;
  HSet (std::unordered_set<int> init, int base=16) : members(std::move(init)) {
    code = 0;
    for (int i = 0; i < base; ++i) {
      if (members.find(i) != members.end()) {
        code += 1;
      }
      code <<= 1;
    }
  }
};

#pragma once
#include <vector>
#include <cmath>
#include <iostream>

// HSet for "hashable set"
class HSet {
public:
  // by using vector instead of unordered_set
  // we can make union faster
  // the data have to be ordered
  std::vector<int> data;

  int code;

  HSet (std::vector<int>& init) : data(std::move(init)) {
    update_code();
  }

  HSet (int init) {
    data.push_back(init);
    update_code();
  }

  void pretty_print() {
    for (auto item: data) {
      std::cout << item << " ";
    }
    std::cout << "| code: " << code << std::endl;
  }

  void update_code() {
    code = 0;
    for (auto item: data) {
      code += std::pow(2, item);
    }
  }

  bool contains(int idx) {
    return (std::find(data.begin(), data.end(), idx) != data.end());
  }

  HSet remove(int idx) {
    auto buf = data;
    buf.erase(std::find(buf.begin(), buf.end(), idx));
    return HSet(buf);
  }
};

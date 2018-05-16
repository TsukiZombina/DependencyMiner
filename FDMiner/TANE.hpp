#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include "reader.hpp"
#include "hset.hpp"

class TANE {
public:
  std::vector<std::vector<int>> data;

  int nrow;
  int ncol;

  TANE () = default;

  void read_data(std::string path) {
    auto r = Reader(path);
    data = std::move(r.data);
    nrow = r.nrow;
    ncol = r.ncol;
  }
};

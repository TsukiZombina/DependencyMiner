#ifndef READER_H
#define READER_H

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

class Reader {
public:
  std::vector<std::vector<int>> data;
  // value_map[k] maps values of the kth column to int
  std::vector<std::unordered_map<std::string, int>> value_map;
  // vector containing all the column names
  std::vector<std::string> attributes;

  int nrow;
  int ncol;

  Reader(std::string path) {
    // std::cout << "Reading data: " << path << std::endl;
    data.reserve(100000);
    read_data(path);
  }

  void read_data(std::string path) {
    std::ifstream in(path);

    // handle column names
    std::string line;
    std::getline(in, line);

    std::stringstream columns(line);
    std::string attribute;

    while(std::getline(columns, attribute, ','))
    {
        attributes.push_back(attribute);
        std::cout << attribute << std::endl;
    }

    for (std::string line; std::getline(in, line); ) {
      // std::cout << "Processing line: " << line << std::endl;
      data.emplace_back();
      if (data.size() >= 1) {
        data[data.size() - 1].reserve(data[0].size());
      }
      parse_line(line, data[data.size() - 1]);
    }
    nrow = data.size();
    ncol = value_map.size();
  }

  void parse_line(std::string& line, std::vector<int>& row) {
    int left = 0;
    int right = 0;
    int col_idx = 0;
    while (right < line.length()) {
      if (line[right] == ',' && line[right + 1] != ' ') {
        // a new column
        auto value = line.substr(left, right - left);
        auto code = hash(value, col_idx);
        row.push_back(code);
        // std::cout << "Processed column " << value << " col_idx=" << col_idx << " hashId=" << value_map[col_idx][value] << std::endl;
        ++col_idx;
        left = right + 1;
      }
      ++right;
    }
    auto value = line.substr(left, right - left);
    auto code = hash(value, col_idx);
    row.push_back(code);
  }

  int hash(std::string& value, int col_idx) {
    if (col_idx >= value_map.size()) {
      value_map.emplace_back(std::unordered_map<std::string, int>());
      value_map[value_map.size() - 1].reserve(100000);
    }
    auto iter = value_map[col_idx].find(value);
    if (iter != value_map[col_idx].end()) {
      return iter->second;
    } else {
      int code = value_map[col_idx].size();
      value_map[col_idx][value] = code;
      return code;
    }
  }

  bool check_integrity() {
    bool flag = true;
    for (int i = 0; i < nrow; ++i) {
      if (data[i].size() != ncol) {
        std::cout << "Error!\n"
                  << "But row " << i << " has " << data[i].size() << " columns.\n";
        flag = false;
      }
    }
    std::cout << "Total Rows: " << nrow << "\n"
              << "Total Cols: " << ncol << "\n";
    return flag;
  }
};

#endif // READER_H

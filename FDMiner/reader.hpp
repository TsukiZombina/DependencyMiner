#ifndef READER_H
#define READER_H

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <regex>
#include <cfloat>
#include "similarity_metrics.hpp"

class Reader {
public:
  std::vector<std::vector<int>> data;
  // value_map[k] maps values of the kth column to int
  std::vector<std::unordered_map<std::string, int>> value_map;
  // vector containing all the column names
  std::vector<std::string> attributes;
  // unordered_map holding centers for clustering
  std::unordered_map<int, std::vector<std::string>> centersMap;

  unsigned int nrow;
  unsigned int ncol;

  bool _useSimilarity = { false };
  std::unordered_map<int, std::pair<std::string, std::string>> _indices;

  Reader(std::string& path) {
    // std::cout << "Reading data: " << path << std::endl;
    data.reserve(100000);
    read_data(path);
  }

  Reader(std::string& path, bool useSimilarity,
         const std::unordered_map<int, std::pair<std::string, std::string>>& indices) :
      _useSimilarity(useSimilarity),
      _indices(indices)
  {
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

    std::ofstream ecStream("eq_classes.txt");

    for(const auto& attribute: attributes)
    {
        ecStream << attribute << " ";
    }

    ecStream << std::endl;

    for(const auto& row: data)
    {
        for(const auto& ec: row)
        {
            ecStream << ec << "\t";
        }

        ecStream << std::endl;
    }

    ecStream.close();
  }

  void parse_line(std::string& line, std::vector<int>& row) {
    unsigned int left = 0;
    unsigned int right = 0;
    unsigned int col_idx = 0;
    while (right < line.length()) {
      if (line[right] == ',' && line[right + 1] != ' ') {
        // a new column
        auto value = line.substr(left, right - left);

        int code;

        if(_useSimilarity)
        {
            code = sim_hash(value, col_idx);
        }
        else
        {
            code = hash(value, col_idx);
        }

        row.push_back(code);
        // std::cout << "Processed column " << value << " col_idx=" << col_idx << " hashId=" << value_map[col_idx][value] << std::endl;
        ++col_idx;
        left = right + 1;
      }
      ++right;
    }
    auto value = line.substr(left, right - left);

    int code;

    if(_useSimilarity)
    {
        code = sim_hash(value, col_idx);
    }
    else
    {
        code = hash(value, col_idx);
    }

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

  int sim_hash(std::string& value, int col_idx)
  {
      if (col_idx >= value_map.size()) {
          value_map.emplace_back(std::unordered_map<std::string, int>());
          value_map[value_map.size() - 1].reserve(100000);
      }

      if(_indices.count(col_idx) > 0)
      {
          if(centersMap.count(col_idx) == 0)
          {
              centersMap[col_idx] = std::vector<std::string>();
              centersMap[col_idx].push_back(value);
              return 0;
          }
          else
          {
              std::string metric = _indices[col_idx].first;

              int eqClass = -1;
              float minDistance = FLT_MAX;

              for(int j = 0; j < centersMap[col_idx].size(); j++)
              {
                  if(metric == "w")
                  {
                      // Parse strings into numeric vector
                      std::regex re("[ ]+");
                      std::sregex_token_iterator it1(centersMap[col_idx][j].begin(), centersMap[col_idx][j].end(), re, -1);
                      std::sregex_token_iterator it2(value.begin(), value.end(), re, -1);
                      std::sregex_token_iterator reg_end;

                      std::vector<float> x;
                      std::vector<float> y;

                      for(; it1 != reg_end && it2 != reg_end; ++it1, ++it2)
                      {
                          x.push_back(std::stof(it1->str()));
                          y.push_back(std::stof(it2->str()));
                      }

                      float d = getDifference(x, y);
                      float delta = std::stof(_indices[col_idx].second);

                      if(d <= delta)
                      {
                          if(d < minDistance)
                          {
                              minDistance = d;
                              eqClass = j;
                          }
                      }
                  }
                  else if(metric == "d")
                  {
                      // Parse date strings
                      std::regex re("[/]+");
                      std::sregex_token_iterator it1(centersMap[col_idx][j].begin(), centersMap[col_idx][j].end(), re, -1);
                      std::sregex_token_iterator it2(value.begin(), value.end(), re, -1);
                      std::sregex_token_iterator reg_end;

                      int d1[3] = { 0, 0, 0 };
                      int d2[3] = { 0, 0, 0 };

                      for(int i = 0; it1 != reg_end && it2 != reg_end; ++it1, ++it2, ++i)
                      {
                          d1[i] = std::stoi(it1->str());
                          d2[i] = std::stoi(it2->str());
                      }

                      Date date1 = { d1[0], d1[1], d1[2] };
                      Date date2 = { d2[0], d2[1], d2[2] };

                      int d = getDifference(date1, date2);
                      long delta = std::stol(_indices[col_idx].second);

                      if(d <= delta)
                      {
                          if(d < minDistance)
                          {
                              minDistance = d;
                              eqClass = j;
                          }
                      }
                  }
                  else if(metric == "t")
                  {
                      // Parse time strings
                      std::regex re("[:]");
                      std::sregex_token_iterator it1(centersMap[col_idx][j].begin(), centersMap[col_idx][j].end(), re, -1);
                      std::sregex_token_iterator it2(value.begin(), value.end(), re, -1);
                      std::sregex_token_iterator reg_end;

                      unsigned t1[2] = { 0, 0 };
                      unsigned t2[2] = { 0, 0 };

                      for(int i = 0; it1 != reg_end && it2 != reg_end; ++it1, ++it2, ++i)
                      {
                          t1[i] = std::stoi(it1->str());
                          t2[i] = std::stoi(it2->str());
                      }

                      Time time1 = { t1[0], t1[1] };
                      Time time2 = { t2[0], t2[1] };

                      long d = getDifference(time1, time2);
                      long threshold = std::stol(_indices[col_idx].second);

                      if(d <= threshold)
                      {
                          if(d < minDistance)
                          {
                              minDistance = d;
                              eqClass = j;
                          }
                      }
                  }
                  else if(metric == "l")
                  {
                      size_t d = getDifference(centersMap[col_idx][j], value);
                      float threshold = std::stoi(_indices[col_idx].second);

                      if(d <= threshold)
                      {
                          if(d < minDistance)
                          {
                              minDistance = d;
                              eqClass = j;
                          }
                      }
                  }
              }

              if(eqClass == -1)
              {
                  // Add new center
                  int code = centersMap[col_idx].size();
                  centersMap[col_idx].push_back(value);
                  return code;
              }

              return eqClass;
          }
      }

      auto iter = value_map[col_idx].find(value);

      if (iter != value_map[col_idx].end())
      {
          return iter->second;
      }
      else
      {
          int code = value_map[col_idx].size();
          value_map[col_idx][value] = code;
          return code;
      }
  }

  bool check_integrity() {
    bool flag = true;
    for (unsigned int i = 0; i < nrow; ++i) {
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

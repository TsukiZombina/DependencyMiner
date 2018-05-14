#pragma once
#include <string>
#include <vector>
#include <unordered_set>
#include <fstream>
#include <iostream>
#include <algorithm>
class DFD {
public:
    DFD(std::string path, int size = -1, int ncol = -1) : size(size), ncol(ncol) {
        if (size == -1) {
            //calculate
        }
        if (ncol == -1) {
            //calculate
        }
        data = new std::vector<std::string>[ncol];
        for (int i = 0; i < ncol; ++i) {
            data[i].reserve(size);
        }
        std::ifstream in(path);
        std::string buffer;
        if (!in.is_open()) {
            std::cout << "read file fail. Please check the path and retry/n";
        }
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < ncol - 1; ++j) {
                std::getline(in, buffer, ',');
                data[j].push_back(buffer);
            }
            std::getline(in, buffer);
            data[ncol - 1].push_back(buffer);
        }
    }
    void ouput() {
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < ncol; ++j) {
                std::cout << data[j][i] << ", ";
            }
            std::cout << std::endl;
        }
    }
    void extraction() {
        for (int i = 0; i < ncol; ++i) {
            if (std::unordered_set<std::string>(data[i].begin(), data[i].end()).size() == data[i].size()) {
                unique_cols.push_back(i);
                for (int j = 0; j < ncol; ++j) {
                    if (j != i) {
                        FD.push_back(std::vector<int>({ i, j }));
                    }
                }
            }
        }
        // foreach RHS\in R do FD.push_back({K->RHS''|k\in findLHSs(RHS,r)})
    }
private:
    std::vector<std::string>* data;
    int size;
    int ncol;
    std::vector<std::vector<int>> FD;
    std::vector<int> unique_cols;
};

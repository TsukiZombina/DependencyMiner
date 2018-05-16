#pragma once
#include <string>
#include <vector>
#include <unordered_set>
#include <map>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <set>

void subset(int n, std::set<int>& S, int depth = -1) {
    if (depth == -1) {
        depth = 32;
    }
    if (depth == 0 || n == 0)
        return;
    int nn = n;
    while (nn != 0) {
        int firstOne = nn & ~(nn - 1);
        nn = nn & ~firstOne;
        S.insert(n & ~firstOne);
        subset(n & ~firstOne, S, depth - 1);
    }
}

void superset(int n, std::set<int>& S, int tabu, int depth = -1, int mask = -1) {
    if (depth == -1) {
        depth = 32;
    }
    if (mask == -1) {
        mask = 1;
        while (mask < n) mask <<= 1;
        mask -= 1;
    }
    if (depth == 0 || (n | tabu) == mask) return;
    int nn = mask & ~n;
    while (nn != 0) {
        int firstOne = nn & ~(nn - 1);
        nn = nn & ~firstOne;
        if (firstOne != tabu) {
            S.insert(n | firstOne);
            superset(n | firstOne, S, tabu, mask, depth - 1);
        }
    }
}

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
                for (int j = 0; j < ncol; ++j) {
                    if (j != i) {
                        FD.push_back(std::vector<int>({ i, j }));
                    }
                }
            } else {
                non_unique_cols.push_back(i);
            }
        }
        for (int i : non_unique_cols) {
            findLHSs(i);
        }
        // foreach RHS\in R do FD.push_back({K->RHS''|k\in findLHSs(RHS,r)})
    }
    void findLHSs(int i) {
        std::vector<int> seeds;
        seeds.push_back(non_unique_cols.at(0) == i ? non_unique_cols.at(1) : non_unique_cols.at(0)); // should random?
        while (!seeds.empty) {
            auto node = pickSeed();
            while (node) {
                if (visited(node)) {
                    if (isCandidate(node) {
                        if (isDependency(node)) {
                            if (isMinimal(node)) {
                                //FD.push()
                            }
                        } else {
                            if (isMaximal(node)) {
                                //maxNonDeps.push_back(node)
                            }
                        }
                        updateDependencyType(node);
                    }
                } else {
                    inferCategory(node);
                    if (category(node) == null) {
                        computePartitions(node, i);
                    }
                }
                node = pickNextNode(node);
            }
            seeds = generateNextSeeds();
        }
    }

    Node pickNextNode(Node node) {
        if (node.isCandidateMinimalDep) {
            auto S = uncheckedSubsets(node);
            auto P = prunedSets(node);
            S = S - P;
            if (S.empty()) {
                //FD.push_back()
            } else {
                nextNode = S.at(0); // should random
                trace.push(node);
                return nextNode;
            }
        } else if (node.isCandidateMaximalNonDep) {
            auto S = uncheckedSupersets(node);
            auto P = prunedSuperSets(node);
            S = S - P;
            if (S.empty()) {
                //maxNonDeps.push_back(node)
            } else {
                nextNode = S.at(0); // should random
                trace.push(node);
                return nextNode;
            }
        } else {
            return trace.nextNode;
        }
    }

    Seeds generateNextSeeds() {
        auto seeds;
        auto newSeeds;
        for (auto maxNonDep : maxNonDeps) {
            complement = maxNonDeps - maxNonDep;
            if (seeds.empty()) {
                auto emptyColumns = Bitset(| maxNonDep | );
                for (auto setBitIndex : complement) {
                    seeds.push_back(emptyColumns.setCopy(setBitIndex);
                }
            } else {
                for (dep : seeds) {
                    for (setBitIndex : complement) {
                        newSeeds.push_back(dep.setCopy(setBitIndex));
                    }
                }
                minimizedNewDeps = minimize newSeeds;
                seeds = {};
                for (newSeed : minimizedNewDeps) {
                    seeds.push_back(newSeed);
                }
                newSeeds = {};
            }
        }
        seeds = seeds - minDeps;
        return seeds;
    }

    bool checkFD(std::vector<int>& lhs, int rhs) {
        std::map<std::string, std::string> dict;
        for (int i = 0; i < size; ++i) {
            std::string l;
            for (auto j : lhs) {
                l += data[j].at(i);
            }
            auto iter = dict.find(l);
            if (iter == dict.end()) {
                dict.insert(std::pair<std::string, std::string>(l, data[rhs].at(i)));
            } else {
                if (data[rhs].at(i) != iter->second)
                    return false;
            }
        }
        return true;
    }
private:
    std::vector<std::string>* data;
    int size;
    int ncol;
    std::vector<std::vector<int>> FD;
    std::vector<int> non_unique_cols;

    struct Node {
        std::vector<int> LHS;
        bool isVisited;
        bool isCandidate;
        bool isDependency;
        bool isMinimal;
        bool isMaximal;
        bool isCandidateMinimalDep;
        bool isCandidateMaximalNonDep;
    };
};

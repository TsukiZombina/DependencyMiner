#pragma once
#include <string>
#include <vector>
#include <stack>
#include <unordered_map>
#include <algorithm>
#include <set>
#include <cassert>
#include "reader.hpp"

#define PROPOGATE
#define PROPOGATELAYER 3
//#define RANDOM

typedef int NodeIndex;
typedef int ColIndex;
const std::vector<ColIndex> BITMAP({ 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768 });

struct Node {
    bool isVisited = false;
    bool isDep = false;
    bool isMinDep = false;
    bool isCandidateMinDep = true;
    bool isNonDep = false;
    bool isMaxNonDep = false;
    bool isCandidateMaxNonDep = true;
};

class DFD {
public:
    DFD(std::string path) {
        auto r = Reader(path);
        data = std::move(r.data);
        nrow = r.nrow;
        ncol = r.ncol;
        T.resize(nrow);
        tabu_for_unique_cols = 0;
        NodeSet.resize(BITMAP.at(ncol));
        set_part_map.reserve(1500);
    }

    void output(std::ostream& out) {
        for (auto& fd : FD) {
            std::string result;
            for (auto x : fd) {
                result += std::to_string(x);
                result += " ";
            }
            result.replace(result.rfind(' '), 1, "");
            result.replace(result.rfind(' '), 1, " -> ");
            out << result << std::endl;
        }
    }

    void extraction() {
        for (int i = 0; i < ncol; ++i) {
            set_part_map[BITMAP.at(i)] = std::move(one_column_partition(i));
            if (set_part_map[BITMAP.at(i)].empty()) {
                for (int j = 0; j < ncol; ++j) {
                    if (j != i) {
                        FD.emplace_back(std::vector<int>({ i + 1, j + 1 }));
                    }
                }
                tabu_for_unique_cols |= BITMAP.at(i);
            } else {
                non_unique_cols.push_back(i);
            }
        }
        //for (int i = 0; i < non_unique_cols.size(); ++i) {
        //    for (int j = i + 1; j < non_unique_cols.size(); ++j) {
        //        multiply_partitions(set_part_map[BITMAP.at(i)], set_part_map[BITMAP.at(j)], set_part_map[BITMAP.at(i) | BITMAP.at(j)]);
        //    }
        //}
        for (int i = 0; i < ncol; ++i) {
            current_rhs = i;
            findLHSs();
            for (NodeIndex j : minDeps) {
                auto fd = getColIndexVector(j);
                for (auto& x : fd) x += 1;
                fd.push_back(i + 1);
                FD.emplace_back(fd);
            }
            //std::fill(NodeSet.begin(), NodeSet.end(), Node());
            for (auto& n : NodeSet) {
                if (n.isVisited) {
                    n.isVisited = false;
                    n.isCandidateMinDep = true;
                    n.isCandidateMaxNonDep = true;
                    n.isDep = false;
                    n.isNonDep = false;
                    n.isMinDep = false;
                    n.isMaxNonDep = false;
                }
            }
            minDeps.clear();
            maxNonDeps.clear();
        }
        std::sort(FD.begin(), FD.end(), [](const std::vector<int>& lhs, const std::vector<int>& rhs) -> bool {
            auto iter1 = lhs.begin();
            auto iter2 = rhs.begin();
            while (iter1 != lhs.end() && iter2 != rhs.end()) {
                if (*iter1 < *iter2) return true;
                if (*iter1 > *iter2) return false;
                ++iter1;
                ++iter2;
            }
            return(iter2 != rhs.end());
        });
    }

    std::vector<std::vector<int>>& getFD() {
        return FD;
    }

private:
    std::vector<std::vector<int>> data;
    int nrow;
    int ncol;
    int tabu_for_unique_cols;
    ColIndex current_rhs;
    std::vector<std::vector<ColIndex>> FD;
    std::vector<ColIndex> non_unique_cols;
    std::vector<Node> NodeSet;
    std::vector<NodeIndex> minDeps;
    std::vector<NodeIndex> maxNonDeps;
    std::stack<NodeIndex> trace;

    using Partition = std::vector<std::vector<int>>;
    std::vector<int> T;
    std::unordered_map<int, int> C;
    std::vector<int> L;
    std::unordered_map<int, Partition> set_part_map;
    Partition S;

    int getRandom(std::set<int>& S) {
        int idx = rand() % S.size(); // not equal prob but acceptable
        auto iter = S.begin();
        while (idx--) {
            ++iter;
        }
        return *iter;
    }

    void subset(int n, std::set<int>& S, int depth = -1) {
        if (depth == -1) {
            depth = 32;
        }
        if (depth == 0 || n == 0)
            return;
        int nn = n;
        while (nn != 0) {
            int firstOne = nn & ~(nn - 1);
            if ((n & ~firstOne) == 0) break;
            nn = nn & ~firstOne;
            S.insert(n & ~firstOne);
            subset(n & ~firstOne, S, depth - 1);
        }
    }

    void superset(int n, std::set<int>& S, int tabu, int mask, int depth = -1) { // optimization: just use 1 layer propogate now
        if (depth == -1) {
            depth = 32;
        }
        if (depth == 0 || (n | tabu) == mask) return;
        int nn = mask & ~n;
        while (nn != 0) {
            int firstOne = nn & ~(nn - 1);
            nn = nn & ~firstOne;
            if ((firstOne & tabu) == 0) {
                S.insert(n | firstOne);
                superset(n | firstOne, S, tabu, mask, depth - 1);
            }
        }
    }

    std::vector<ColIndex> getColIndexVector(NodeIndex nodeID) {
        std::vector<ColIndex> V;
        for (ColIndex i = 0; i < BITMAP.size(); ++i) {
            if (nodeID & BITMAP.at(i)) {
                V.push_back(i);
            }
        }
        return V;
    }

    void minimize(std::set<NodeIndex>& newSeeds, std::set<NodeIndex>& seeds) {
        seeds.clear();
        while (!newSeeds.empty()) {
            auto x = *newSeeds.begin();
            newSeeds.erase(newSeeds.begin());
            bool reserve_it = true;
            for (auto iter = newSeeds.begin(); iter != newSeeds.end();) {
                if ((*iter & x) == x) {
                    iter = newSeeds.erase(iter);
                } else if ((*iter & x) == *iter) {
                    reserve_it = false;
                    break;
                } else {
                    ++iter;
                }
            }
            if (reserve_it) {
                seeds.insert(x);
            }
        }
        newSeeds.clear();
    }

    void findLHSs() {
        if (non_unique_cols.size() < 2) return;
        std::set<NodeIndex> seeds;
        for (auto col : non_unique_cols) {
            if (col == current_rhs) continue;
            seeds.insert(BITMAP.at(col));
            while (!seeds.empty()) {
#ifdef RANDOM
                int nodeID = getRandom(seeds);
#else
                int nodeID = *seeds.begin();
#endif
                while (nodeID != -1) {
                    auto& node = NodeSet.at(nodeID);
                    if (node.isVisited) {
                        if (node.isCandidateMinDep || node.isCandidateMaxNonDep) {
                            if (node.isDep) {
                                if (node.isMinDep) {
                                    node.isCandidateMinDep = false;
                                    minDeps.push_back(nodeID);
                                }
                            } else {
                                if (node.isMaxNonDep) {
                                    node.isCandidateMaxNonDep = false;
                                    maxNonDeps.push_back(nodeID);
                                }
                            }
                        }
                    } else {
                        if (!inferCategory(nodeID)) {
                            computePartition(nodeID);
                        }
                    }
                    nodeID = pickNextNode(nodeID);
                }
                seeds = generateNextSeeds();
            }
        }
    }

    NodeIndex pickNextNode(NodeIndex nodeID) {
        auto& node = NodeSet.at(nodeID);
        if (node.isCandidateMinDep) {
            std::set<NodeIndex> S;
            subset(nodeID, S, 1);
            for (auto iter = S.begin(); iter != S.end();) {
                if (NodeSet.at(*iter).isVisited) {
                    if (NodeSet.at(*iter).isNonDep) {
                        iter = S.erase(iter);
                    } else {
                        node.isCandidateMinDep = false;
                        S.clear();
                        break;
                    }
                } else {
                    ++iter;
                }
            }
            if (S.empty()) {
                if (node.isCandidateMinDep) {
                    node.isMinDep = true;
                    node.isCandidateMinDep = false;
                    minDeps.push_back(nodeID);
                }
            } else {
#ifdef RANDOM
                NodeIndex nextNode = getRandom(S);
#else
                NodeIndex nextNode = *S.begin(); // should random
#endif
                trace.push(nodeID);
                return nextNode;
            }
        } else if (node.isCandidateMaxNonDep) {
            std::set<NodeIndex> S;
            superset(nodeID, S, tabu_for_unique_cols | BITMAP.at(current_rhs), (1 << ncol) - 1, 1); // Not sure whether I understand it correctly
            for (auto iter = S.begin(); iter != S.end();) {
                if (NodeSet.at(*iter).isVisited) {
                    if (NodeSet.at(*iter).isDep) {
                        iter = S.erase(iter);
                    } else {
                        node.isCandidateMaxNonDep = false;
                        break;
                        S.clear();
                    }
                } else {
                    ++iter;
                }
            }
            if (S.empty()) {
                if (node.isCandidateMaxNonDep) {
                    node.isMaxNonDep = true;
                    node.isCandidateMaxNonDep = false;
                    maxNonDeps.push_back(nodeID);
                }
            } else {
#ifdef RANDOM
                NodeIndex nextNode = getRandom(S);
#else
                NodeIndex nextNode = *S.begin();
#endif
                trace.push(nodeID);
                return nextNode;
            }
        }
        NodeIndex idx;
        while (!trace.empty()) {
            idx = trace.top();
            if (NodeSet.at(idx).isVisited && !NodeSet.at(idx).isCandidateMaxNonDep && !NodeSet.at(idx).isCandidateMinDep) {
                trace.pop();
            } else {
                break;
            }
        }
        if (trace.empty())
            return -1;
        trace.pop();
        return idx;
    }

    std::set<NodeIndex> generateNextSeeds() {
        std::set<NodeIndex> seeds;
        std::set<NodeIndex> newSeeds;
        for (auto maxNonDep : maxNonDeps) {
            NodeIndex complement = (BITMAP.at(ncol)- 1) & (~maxNonDep) & ~(tabu_for_unique_cols | BITMAP.at(current_rhs));
            if (seeds.empty()) {
                for (ColIndex i = 0; i < ncol; ++i) {
                    if (complement & BITMAP.at(i)) {
                        seeds.insert(BITMAP.at(i));
                    }
                }
            } else {
                for (auto dep : seeds) {
                    for (ColIndex i = 0; i < ncol; ++i) {
                        if (complement & BITMAP.at(i)) {
                            newSeeds.insert(dep | BITMAP.at(i));
                        }
                    }
                }
                minimize(newSeeds, seeds);
            }
        }
        for (auto dep : minDeps)
            seeds.erase(dep);
        return seeds;
    }

    bool checkFD(NodeIndex nodeID) {
        if (set_part_map.find(nodeID) == set_part_map.end()) {
            std::set<NodeIndex> S;
            subset(nodeID, S, 1);
            int childNodeID = -1;
            for (auto s = S.begin(); s != S.end(); ++s) {
                if (set_part_map.find(*s) != set_part_map.end()) {
                    childNodeID = *s;
                    break;
                }
            }
            if (childNodeID != -1)
                multiply_partitions(set_part_map[childNodeID], set_part_map[~childNodeID & nodeID], set_part_map[nodeID]);
            else {
                auto lhs = getColIndexVector(nodeID);
                auto e = BITMAP.at(lhs.back());
                lhs.erase(lhs.end() - 1);
                for (auto l : lhs) {
                    auto x = BITMAP.at(l);
                    if (set_part_map.find(e | x) == set_part_map.end()) {
                        multiply_partitions(set_part_map[e], set_part_map[x], set_part_map[e | x]);
                    }
                    e = e | x;
                }
            }
        }
        if (set_part_map.find(nodeID | BITMAP.at(current_rhs)) == set_part_map.end()) {
             multiply_partitions(set_part_map.at(nodeID), set_part_map.at(BITMAP.at(current_rhs)), set_part_map[nodeID | BITMAP.at(current_rhs)]);
        }
        bool isFD = set_part_map[nodeID].size() == set_part_map[nodeID | BITMAP.at(current_rhs)].size();
        
#ifdef PROPOGATE
        if (isFD) { // could do more?
            std::set<NodeIndex> S;
            superset(nodeID, S, BITMAP.at(current_rhs) | tabu_for_unique_cols, (1 << ncol) - 1, PROPOGATELAYER);
            for (int s : S) {
                auto& node = NodeSet.at(s);
                node.isVisited = true;
                node.isDep = true;
                node.isCandidateMinDep = false;
                node.isCandidateMaxNonDep = false;
            }
        } else {
            std::set<NodeIndex> S;
            subset(nodeID, S, PROPOGATELAYER);
            for (int s : S) {
                auto& node = NodeSet.at(s);
                node.isVisited = true;
                node.isNonDep = true;
                node.isCandidateMinDep = false;
                node.isCandidateMaxNonDep = false;
            }
        }
#endif
        return isFD;
    }

    bool inferCategory(NodeIndex nodeID) {
        std::set<NodeIndex> S;
        auto& node = NodeSet.at(nodeID);
        subset(nodeID, S, 1);
        for (auto s : S) {
            if (NodeSet.at(s).isVisited && NodeSet.at(s).isDep) {
                node.isVisited = true;
                node.isDep = true;
                node.isCandidateMinDep = false;
                node.isCandidateMaxNonDep = false;
                return true;
            }
        }
        S.clear();
        superset(nodeID, S, tabu_for_unique_cols | BITMAP.at(current_rhs), (1 << ncol) - 1, 1);
        for (auto s : S) {
            if (NodeSet.at(s).isVisited && NodeSet.at(s).isNonDep) {
                node.isVisited = true;
                node.isNonDep = true;
                node.isCandidateMaxNonDep = false;
                node.isCandidateMinDep = false;
                return true;
            }
        }
        return false;
    }

    void computePartition(NodeIndex nodeID) {
        auto& node = NodeSet.at(nodeID);
        node.isVisited = true;
        bool check_result = checkFD(nodeID);
        node.isDep = check_result;
        node.isCandidateMinDep = check_result;
        node.isNonDep = !check_result;
        node.isCandidateMaxNonDep = !check_result;
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
        buf.reserve(150);
        std::fill(T.begin(), T.end(), -1);
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
};

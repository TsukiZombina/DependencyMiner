#include "TANE.hpp"
#include "hset.hpp"
#include "reader.hpp"
#include <string>
#include <unordered_set>
#include <iostream>
#include <vector>

void test_generate_next_level() {
    TANE t;
    std::vector<HSet> tmp;
    for (int i = 0; i < 4; ++i) {
        tmp.emplace_back(i);
    }
    for (auto h: tmp) {
        h.pretty_print();
    }
    std::cout << "============= next level =================\n";
    while (!tmp.empty()) {
        tmp = t.generate_next_level(tmp);
        for (auto h: tmp) {
            h.pretty_print();
        }
        std::cout << "============= next level =================\n";
    }
}

void test_read_data() {
    std::string path = "./data/data.txt";
    Reader r(path);
    r.check_integrity();
}

void test_partition_trivial() {
    std::string path = "./data/test_data.txt";
    TANE t;
    t.read_data(path);
    for (int i = 0; i < t.ncol; ++i) {
        std::cout << "*********************** Col " << i << " *************************\n";
        auto p = t.compute_partitions(i);
        for (auto e_class: p) {
            std::cout << e_class.first << ": ";
            for (auto ridx: e_class.second) {
                std::cout << ridx << " ";
            }
            std::cout << "\n";
        }
    }
}

void test_Hset() {
    std::vector<int> s1 = {1, 4, 9};
    std::vector<int> s2 = {1, 4, 9};
    std::vector<int> s3 = {1, 4, 10};
    auto h1 = HSet(s1);
    auto h2 = HSet(s2);
    auto h3 = HSet(s3);
    std::cout << h1.code << " " << h2.code << " " << h3.code << std::endl;
}

int main() {
    test_partition_trivial();
    system("pause");
    return 0;
}
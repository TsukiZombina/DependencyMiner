#include "TANE.hpp"
#include "hset.hpp"
#include "reader.hpp"
#include <string>
#include <unordered_set>
#include <iostream>
#include <vector>
#include <fstream>

void print_partition(TANE::Partition& p) {
    for (auto e_class: p) {
        std::cout << e_class.first << ": ";
        for (auto ridx: e_class.second) {
            std::cout << ridx << " ";
        }
        std::cout << "\n";
    }
}

void test_generate_next_level() {
    TANE t;
    std::vector<HSet> tmp;
    for (int i = 0; i < 4; ++i) {
        tmp.emplace_back(i);
    }
    for (auto h: tmp) {
        h.pretty_print();
    }
    t.L = std::move(tmp);
    while (!t.L.empty()) {
        std::cout << "==========================================\n";
        t.generate_next_level();
        for (auto h: t.L) {
            h.pretty_print();
        }
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
        print_partition(p);
    }
}

void test_partition_product() {
    std::string path = "./data/test_data.txt";
    TANE t;
    t.read_data(path);
    auto p1 = t.compute_partitions(1);
    auto p2 = t.compute_partitions(2);
    auto p12 = t.multiply_partitions(p1, p2);
    std::cout << "************************* partition using 1&2 **************************\n";
    print_partition(p12);

    auto p3 = t.compute_partitions(3);
    std::cout << "************************* partition using 3 **************************\n";
    print_partition(p3);
    auto p4 = t.compute_partitions(4);
    auto p34 = t.multiply_partitions(p3, p4);
    std::cout << "************************* partition using 3&4 **************************\n";
    print_partition(p34);

    auto p1234 = t.multiply_partitions(p12, p34);
    std::cout << "************************* partition using 1&2&3&4 **************************\n";
    print_partition(p1234);
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

void test_mine_FD() {
    std::string path = "./data/test_data.txt";
    TANE t;
    t.read_data(path);
    t.mine_fd();

    std::ofstream ofs("./data/output.txt");

    for (auto item: t.FD) {
        auto rhs = *(item.end() - 1);
        for (auto col: item) {
            if (col != rhs) {
                ofs << col + 1 << " ";
            }
        }
        ofs << "-> " << rhs + 1 << std::endl;
    }
}

int main() {
    test_mine_FD();
    system("pause");
    return 0;
}
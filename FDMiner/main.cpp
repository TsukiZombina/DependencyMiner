#include "TANE.hpp"
#include "hset.hpp"
#include <string>
#include <unordered_set>
#include <iostream>
#include <vector>

void test_TANE() {
    // std::string path = "./data/test_data.txt";
    std::string path = "./data/data.txt";
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
    // test_Hset();
    test_TANE();
    system("pause");
    return 0;
}
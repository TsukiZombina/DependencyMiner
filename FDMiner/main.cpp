#include "TANE.hpp"
#include "hset.hpp"
#include <string>
#include <unordered_set>
#include <iostream>

void test_TANE() {
    // std::string path = "./data/test_data.txt";
    std::string path = "./data/data.txt";
    TANE t = TANE();
    t.read_data(path);
    system("pause");
}

void test_Hset() {
    std::unordered_set<int> s1 = {1, 4, 9};
    std::unordered_set<int> s2 = {1, 4, 9};
    std::unordered_set<int> s3 = {1, 4, 10};
    auto h1 = HSet(s1);
    auto h2 = HSet(s2);
    auto h3 = HSet(s3);
    std::cout << h1.code << " " << h2.code << " " << h3.code << std::endl;
}

int main() {
    test_Hset();
    system("pause");
    return 0;
}
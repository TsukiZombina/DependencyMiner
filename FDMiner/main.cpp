#include "DFD.hpp"
#include "TANE.hpp"
#include <ctime>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>

double run_TANE() {
    std::string path = "./data.txt";
    TANE t;

    clock_t start = clock();

    t.read_data(path);
    t.run();
    std::ofstream ofs("./TANE_out.txt");
    t.output(ofs);
    ofs.close();

    clock_t end = clock();

    return ((double)(end - start) / CLOCKS_PER_SEC);
}

void test_DFD(bool test = false) {
    srand(time(0));
    if (test) {
        DFD dfd("./test_data.txt");
        dfd.extraction();
        assert(dfd.getFD().size() == 109);
        assert(dfd.getFD().at(48).at(0) == 5);
        assert(dfd.getFD().at(48).at(1) == 11);
        assert(dfd.getFD().at(48).at(2) == 12);
    } else {
        DFD dfd("./data.txt");
        dfd.extraction();
        std::ofstream os("./result.txt");
        dfd.output(os);
    }
}

void test_speed() {
    std::vector<int> T(1000000000);
    clock_t start, end;

    start = clock();
    for (int i = 0; i < T.size(); ++i) {
        T[i] = 1;
    }
    end = clock();
    std::cout << "Time elapsed: " << (double)(end - start) / CLOCKS_PER_SEC << std::endl;

    start = clock();
    for (auto i = T.begin(); i != T.end(); ++i) {
        *i = -1;
    }
    end = clock();
    std::cout << "Time elapsed: " << (double)(end - start) / CLOCKS_PER_SEC << std::endl;

}

void test_TANE() {
    double total = 0;
    int loop = 10;

    run_TANE(); // warm up
    for (int i = 0; i < loop; ++i) {
        double elapse = run_TANE();
        std::cout << "Time elapsed: " << elapse << std::endl;
        total += elapse;
    }

    total /= loop;
    std::cout << "Average: " << total << std::endl;
    system("pause");
}

int main() {
    test_TANE();
    // run_TANE();
    return 0;
}

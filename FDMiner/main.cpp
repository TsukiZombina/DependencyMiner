#include "DFD.hpp"
#include <ctime>
#include <cassert>
#include "TANE.hpp"
#include <string>
#include <fstream>

void test_TANE() {
    std::string path = "./data.txt";
    TANE t;
    t.read_data(path);
    t.run();

    std::ofstream ofs("./TANE_out.txt");

    for (auto item: t.FD) {
        auto lhs = decode_to_vector(item.first);
        auto rhs = item.second;
        for (auto col: lhs) {
            if (col != rhs) {
                ofs << col + 1 << " ";
            }
        }
        ofs << "-> " << rhs + 1 << std::endl;
    }
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

int main() {
    test_TANE();
    //test_DFD(false);
    return 0;
}

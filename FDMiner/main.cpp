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

int main() {
    test_TANE();
    return 0;
}
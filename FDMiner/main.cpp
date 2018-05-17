#include "TANE.hpp"
#include "bitmap_set.hpp"
#include "reader.hpp"
#include <string>
#include <fstream>

void test_mine_FD() {
    // std::string path = "./data/test_data.txt";
    std::string path = "./data.txt";
    TANE t;
    t.read_data(path);
    t.run();

    std::ofstream ofs("./result.txt");

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
    test_mine_FD();
    system("pause");
    return 0;
}
#include "TANE.hpp"
#include <string>

int main() {
    // std::string path = "./data/test_data.txt";
    std::string path = "./data/data.txt";
    TANE t = TANE();
    t.read_data(path);
    system("pause");
    return 0;
}
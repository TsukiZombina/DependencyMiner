#include "DFD.hpp"
#include <ctime>
#include <cassert>

int main() {
    srand(time(0));
    DFD dfd("C:\\Users\\Vica\\Desktop\\test_data.txt", 100, 12);
    //DFD dfd("C:\\Users\\Vica\\Desktop\\trivia.txt", 8, 4);
    dfd.extraction();
    //dfd.output();
    assert(dfd.getFD().size() == 109);
    assert(dfd.getFD().at(48).at(0) == 5);
    assert(dfd.getFD().at(48).at(1) == 11);
    assert(dfd.getFD().at(48).at(2) == 12);
    return 0;
}

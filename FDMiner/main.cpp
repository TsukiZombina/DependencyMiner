#include "DFD.hpp"

int main() {
    //DFD dfd("C:\\Users\\Vica\\Desktop\\test_data.txt", 100, 12);
    DFD dfd("C:\\Users\\Vica\\Desktop\\trivia.txt", 8, 4);
    dfd.extraction();
    dfd.output();

    return 0;
}
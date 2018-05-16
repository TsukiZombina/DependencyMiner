#include "DFD.hpp"
#include <ctime>

int main() {
    srand(time(0));
    DFD dfd("C:\\Users\\Vica\\Desktop\\test_data.txt", 100, 12);
    //DFD dfd("C:\\Users\\Vica\\Desktop\\trivia.txt", 8, 4);
    dfd.extraction();
    dfd.output();

    return 0;
}

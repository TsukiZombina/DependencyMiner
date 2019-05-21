#include "DFD.hpp"
#include "TANE.hpp"
#include "cxxopts.hpp"
#include <ctime>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <utility>
#include <vector>
#include <unordered_map>

double run_TANE(std::string& path) {
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

double run_TANE(std::string& path, bool useSimilarity,
                const std::unordered_map<int, std::string>& indices) {
    TANE t;

    clock_t start = clock();

    t.read_data(path, useSimilarity, indices);
    t.run();
    std::ofstream ofs("./TANE_out.txt");
    t.output(ofs);
    ofs.close();

    clock_t end = clock();

    return ((double)(end - start) / CLOCKS_PER_SEC);
}

void run_DFD(bool test = false) {
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
        std::ofstream os("./DFD_out.txt");
        dfd.output(os);
    }
}

void test_TANE(std::string& path) {
    double total = 0;
    int loop = 10;

    run_TANE(path); // warm up
    for (int i = 0; i < loop; ++i) {
        double elapse = run_TANE(path);
        std::cout << "Time elapsed: " << elapse << std::endl;
        total += elapse;
    }

    total /= loop;
    std::cout << "Average: " << total << std::endl;
    system("pause");
}

void test_DFD() {
    double total = 0;
    int loop = 10;

    for (int i = 0; i < loop; ++i) {
        clock_t start, end;
        start = clock();
        run_DFD(false);
        end = clock();
        double diff = (double)(end - start) / CLOCKS_PER_SEC;
        std::cout << "Time elapsed: " << diff << std::endl;
        total += diff;
    }
    total /= loop;
    std::cout << "Average: " << total << std::endl;
    system("pause");
}

std::vector<std::string> split(const std::string& s, char delimiter)
{
   std::vector<std::string> tokens;
   std::string token;
   std::istringstream tokenStream(s);
   while (std::getline(tokenStream, token, delimiter))
   {
      tokens.push_back(token);
   }
   return tokens;
}

int main(int argc, char** argv) {
    cxxopts::Options options("FDMiner", "An implementation of the TANE algorithm");
    options.add_options()
        ("f, filename",    "Input file",             cxxopts::value<std::string>())
        ("s, similarity",  "Use similarity metrics", cxxopts::value<bool>()->default_value("false"))
        ("a, attributes",  "Attributes indices",     cxxopts::value<std::string>()->default_value(""))
        ("m, metrics",     "Similarity metrics",     cxxopts::value<std::string>()->default_value(""));

    auto args = options.parse(argc, argv);

    std::string filename = args["filename"].as<std::string>();
    bool useSimilarity   = args["similarity"].as<bool>();
    std::string attArg   = args["attributes"].as<std::string>();
    std::string metArg   = args["metrics"].as<std::string>();

    if(useSimilarity)
    {
        std::vector<std::string> attributes = split(attArg, ',');
        std::vector<std::string> metrics    = split(metArg, ',');

        if(attributes.size() == metrics.size())
        {
            // Make an unordered_map of <index, metric> pairs
            std::unordered_map<int, std::string> indices;

            for(int i = 0; i < attributes.size(); i++)
            {
                indices[std::stoi(attributes[i])] = metrics[i];
            }

            run_TANE(filename, useSimilarity, indices);
        }
    }
    else
    {
        // test_TANE();
        run_TANE(filename);
    }

    return 0;
}

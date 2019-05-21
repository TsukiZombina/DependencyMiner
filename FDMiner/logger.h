#ifndef LOGGER_H
#define LOGGER_H

#include <vector>
#include <unordered_map>
#include <fstream>

class Logger
{
public:
    Logger(const std::string& logfile);
    ~Logger();

    void logMessage(const std::string& message);
    void logL(const std::vector<int>& L);
    void logSetPartMap(const std::unordered_map<int, std::vector<std::vector<int>>>& set_part_map);
    void logC(const std::unordered_map<int, int>& C);
    void logXset(const std::vector<int>& Xset);

private:
    std::ofstream mStream;
};

#endif // LOGGER_H

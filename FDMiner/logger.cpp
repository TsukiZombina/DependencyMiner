#include "logger.h"

Logger::Logger(const std::string& filename)
{
    mStream.open(filename);
}

Logger::~Logger()
{
    mStream.close();
}

void Logger::logL(const std::vector<int>& L)
{
    mStream << "L (vector<vector<int>>)\n";

    for(const auto& value : L)
    {
        mStream << value << " ";
    }

    mStream << "\n\n";
}

void Logger::logSetPartMap(const std::unordered_map<int, std::vector<std::vector<int>>>& set_part_map)
{
    mStream << "set_part_map (unordered_map<int, vector<vector<int>>>):\n";

    for(const auto& keyValue: set_part_map)
    {
        mStream << keyValue.first << ":\n";

        for(int row = 0; row < keyValue.second.size(); row++)
        {
            mStream << "{";

            for(int col = 0; col < keyValue.second[row].size(); col++)
            {
                mStream << keyValue.second[row][col] << ", ";
            }

            mStream << "}\n";
        }
    }

    mStream << "\n";
}

void Logger::logC(const std::unordered_map<int, int>& C)
{
    mStream << "C (unordered_map<int, int>):\n";

    for(const auto& keyValue: C)
    {
        mStream << "C[" << keyValue.first << "] = " << keyValue.second << "\n";
    }

    mStream << "\n\n";
}

void Logger::logMessage(const std::string& message)
{
    mStream << message << "\n\n";
}

void Logger::logXset(const std::vector<int>& Xset)
{
    mStream << "Xset (vector<int>):\n";

    for(const auto& value : Xset)
    {
        mStream << value << " ";
    }

    mStream << "\n\n";
}


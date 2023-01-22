#pragma once

#include <string>

using namespace std;

class LogLevels
{
    public:
        enum LogLevel{ Debug = 0, Info = 1, Warning = 2, Error = 3};
        string LogLevelToString(LogLevel logLevel);
        LogLevel LogLevelFromString(string logLevel);
};
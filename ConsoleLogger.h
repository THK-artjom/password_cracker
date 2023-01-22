#pragma once

#include "LogLevel.h"
#include <string>
#include <map>
#include <fstream>
#include <iomanip>

using namespace std;

class ConsoleLogger
{
    private:
        int _processId;
        LogLevels::LogLevel _minLogLevel;
        map<int, ofstream*> _fileStreams;
        string GetTime();
        LogLevels _logLevels;
        void WriteLog(char* formatedStr, LogLevels::LogLevel logLevel);

    public: 
        ConsoleLogger(LogLevels::LogLevel minLogLevel, int processId) { _processId = processId; _minLogLevel = minLogLevel; _logLevels = LogLevels(); };
        ~ConsoleLogger();
        void Info(const char* format, ...);
        void Error(const char* format, ...);
        void Debug(const char* format, ...);
        void Warning(const char* format, ...);
};
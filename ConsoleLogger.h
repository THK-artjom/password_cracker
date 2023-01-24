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
        static ConsoleLogger* _instance;
        static string _fileName;
        
        int _processId;
        LogLevels::LogLevel _minLogLevel;
        string GetTime();
        LogLevels _logLevels;
        void WriteLog(char* formatedStr, LogLevels::LogLevel logLevel);
        ConsoleLogger(LogLevels::LogLevel minLogLevel, int processId) { _processId = processId; _minLogLevel = minLogLevel; _logLevels = LogLevels(); };

        void SetProcessId(int processId);
        void SetLogLevel(LogLevels::LogLevel logLevel);
    public:         
        ~ConsoleLogger();
        static ConsoleLogger* Instance(LogLevels::LogLevel minLogLevel, int processId);
        void Info(const char* format, ...);
        void Error(const char* format, ...);
        void Debug(const char* format, ...);
        void Warning(const char* format, ...);
};
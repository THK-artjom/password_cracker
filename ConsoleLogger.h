#pragma once

#include "LogLevel.h"
#include <string>
#include <map>
#include <fstream>
#include <iomanip>
//#include <mutex>

using namespace std;

class ConsoleLogger
{
    private:
        //mutex _fileWriteMutex;
        static ConsoleLogger* _instance;
        int _processId;
        LogLevels::LogLevel _minLogLevel;
        //map<int, ofstream*> _fileStreams;
        //map<int, mutex> _streamMutex;
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
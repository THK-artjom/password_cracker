#include "ConsoleLogger.h"
#include "LogLevel.h"
#include <stdio.h>
#include <iostream>
#include <chrono>
#include <ctime>
#include <string>
#include <string.h>
#include <cstdarg>
#include <iomanip>
#include <sstream>
#include <fstream>

using namespace std;
using namespace chrono;

void ConsoleLogger::Debug(const char* format, ...)
{
    va_list vl;
    va_start(vl, format);
    char formatedInfo[255] = {};
    vsnprintf(formatedInfo, 255 - 1, format, vl);
    va_end(vl);

    WriteLog(formatedInfo, LogLevels::LogLevel::Debug); 
}

void ConsoleLogger::Info(const char* format, ...)
{
    va_list vl;
    va_start(vl, format);
    char formatedInfo[255] = {};
    vsnprintf(formatedInfo, 255 - 1, format, vl);
    va_end(vl);

    WriteLog(formatedInfo, LogLevels::LogLevel::Info);
}

void ConsoleLogger::Warning(const char* format, ...)
{
    va_list vl;
    va_start(vl, format);
    char formatedInfo[255] = {};
    vsnprintf(formatedInfo, 255 - 1, format, vl);
    va_end(vl);

    WriteLog(formatedInfo, LogLevels::LogLevel::Warning);
}

void ConsoleLogger::Error(const char* format, ...)
{
    va_list vl;
    va_start(vl, format);
    char formatedInfo[255] = {};
    vsnprintf(formatedInfo, 255 - 1, format, vl);
    va_end(vl);

    WriteLog(formatedInfo, LogLevels::LogLevel::Error);
}

string ConsoleLogger::GetTime()
{
    // get current time
    auto now = system_clock::now();

    // get number of milliseconds for the current second
    // (remainder after division into seconds)
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

    // convert to std::time_t in order to convert to std::tm (broken time)
    auto timer = system_clock::to_time_t(now);

    // convert to broken time
    std::tm bt = *std::localtime(&timer);

    std::ostringstream oss;

    oss << std::put_time(&bt, "%H:%M:%S"); // HH:MM:SS
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count();

    return oss.str();
}

string GetFileNameForLogger(int processId)
{
    char fileNameFormat[] = "Process_%d.log";
    char fileName[strlen(fileNameFormat) + 10] = { };
    sprintf(fileName, fileNameFormat, processId);
    return fileName;
}

void ConsoleLogger::WriteLog(char* formatedStr, LogLevels::LogLevel logLevel)
{
    if(logLevel < _minLogLevel)
        return;

    string logLevelStr = _logLevels.LogLevelToString(logLevel);
    string time = GetTime();

    string fileName = GetFileNameForLogger(_processId);
    ofstream myfile;
    myfile.open(fileName, std::ios::out | std::ios::app);

    cout << "[" << time << "]:\t [" << logLevelStr.c_str() << "]\t [" << _processId << "]:\t" << formatedStr << '\n';
    myfile << "[" << time << "]:\t [" << logLevelStr.c_str() << "]\t [" << _processId << "]:\t" << formatedStr << '\n';

    myfile.close();
}

ConsoleLogger::~ConsoleLogger()
{
    _instance = nullptr;
}

void ConsoleLogger::SetLogLevel(LogLevels::LogLevel logLevel)
{
    _minLogLevel = logLevel;
}

void ConsoleLogger::SetProcessId(int processId)
{
    _processId = processId;
}

ConsoleLogger* ConsoleLogger::_instance = nullptr;

ConsoleLogger* ConsoleLogger::Instance(LogLevels::LogLevel minLogLevel, int processId)
{
    if(ConsoleLogger::_instance == nullptr)
        ConsoleLogger::_instance = new ConsoleLogger(minLogLevel, processId);
    
    ConsoleLogger::_instance->SetLogLevel(minLogLevel);
    ConsoleLogger::_instance->SetProcessId(processId);

    return ConsoleLogger::_instance;
}
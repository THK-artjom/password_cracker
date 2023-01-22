#include "LogLevel.h"
#include <string>

using namespace std;

string LogLevels::LogLevelToString(LogLevels::LogLevel logLevel)
{
    switch (logLevel)
    {
        case LogLevel::Info:
            return "Info";
        case LogLevel::Warning:
            return "Warning";
        case LogLevel::Error:
            return "Error";
        case LogLevel::Debug:
        default: 
            return "Debug";
    }
}

LogLevels::LogLevel LogLevels::LogLevelFromString(string logLevel)
{
    if(logLevel.compare("Info") == 0)
        return LogLevel::Info;
    else if(logLevel.compare("Warning") == 0)
        return LogLevel::Warning;
    else if(logLevel.compare("Error") == 0)
        return LogLevel::Error;
    else
        return LogLevel::Debug;           
}
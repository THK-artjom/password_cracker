#pragma once

#include <string>
#include <stdio.h>
#include "ConsoleLogger.h"

using namespace std;

class PasswordChecker
{
    private:
        string _password;
        ConsoleLogger* _logger;
    public:
        PasswordChecker(string password, ConsoleLogger* logger);
        bool IsPasswordValid(string password);
};

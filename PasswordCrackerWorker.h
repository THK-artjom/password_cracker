#pragma once

#include <string>
#include "PasswordChecker.h"
#include "ConsoleLogger.h"

class PasswordCrackerWorker
{
    private:
        ConsoleLogger& _logger;
        PasswordChecker& _passwordChecker;
        const string _characters;
        const int _charactersCount;
        const int _maxLength;
    public:
        PasswordCrackerWorker(string characters
                            , int maxLength
                            , ConsoleLogger& logger
                            , PasswordChecker& passwordChecker);

        ~PasswordCrackerWorker();
        void CrackPassword(int splitRangeLength);
};
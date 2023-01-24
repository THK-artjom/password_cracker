#pragma once

#include <string>
#include "PasswordChecker.h"
#include "ConsoleLogger.h"

class PasswordCrackWorker
{
    private:
        ConsoleLogger& _logger;
        PasswordChecker& _passwordChecker;
        const string _characters;
        const int _charactersCount;
        const int _maxLength;
    public:
        PasswordCrackWorker(string characters
                            , int maxLength
                            , ConsoleLogger& logger
                            , PasswordChecker& passwordChecker);

        ~PasswordCrackWorker();
        void CrackPassword(int splitRangeLength);
};
#pragma once

#include <string>
#include "PasswordChecker.h"
#include "ConsoleLogger.h"

class PasswordCrackerMainWorker
{
    private:
        bool IsPasswordOnlyOneLetter();
        PasswordChecker& _passwordChecker;
        ConsoleLogger& _logger;
        const string _characters;
        const int _charactersCount;
        const int _maxPasswordLength;
        const int _numProcs;
    public:
        PasswordCrackerMainWorker(string characters
                                , int maxPasswordLength
                                , int numProcs
                                , ConsoleLogger& logger
                                , PasswordChecker& passwordChecker);

        ~PasswordCrackerMainWorker();
        void StartCracking(int splitRangeLength);
};
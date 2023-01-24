#include <stdio.h>
#include <string>
#include <string.h>
#include <mpi.h>
#include <math.h>
//#include <vector>
#include <bits/stdc++.h>
#include "time.h"
#include <map>

#include "ConsoleLogger.h"
#include "MPI_Definitions.h"
#include "PasswordChecker.h"
#include "PasswordCrackerWorker.h"
#include "PasswordCrackerMainWorker.h"

using namespace std;

#define MAX_PASSWORD_LENGTH 6

bool IsPasswordSetupValid(string passwordToFind, string characters, int maxPasswordLength, ConsoleLogger* logger)
{
    logger->Debug("Available characters: %s for password to find %s", characters.c_str(), passwordToFind.c_str());

    if(passwordToFind.length() > maxPasswordLength)
    { 
        logger->Error("Password %s is to long for password finder max length %d.", passwordToFind.c_str(), maxPasswordLength);
        return false;
    }
    
    for (size_t i = 0; i < passwordToFind.length(); i++)
    {
        if(characters.find(passwordToFind[i]) == std::string::npos)
        { 
            logger->Error("Password %s contains character %c not in character set %s", passwordToFind.c_str(), passwordToFind[i], characters.c_str());
            return false;
        }
    }

    return true;
}

int AbortExecution()
{
    MPI_Abort(MPI_COMM_WORLD, MPI_ERR_ARG);
    return 0;
}

int StopExecution()
{
    MPI_Finalize();
    return 0;
}

int main(int argumentsCnt, char** argumentsPtr)
{    
    string passwordToFind ="hak"; //only if no argument is provided
    string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"; //only if no argument is provided
    LogLevels::LogLevel logLevel = LogLevels::LogLevel::Debug;
    ConsoleLogger* logger = ConsoleLogger::Instance(logLevel, -1);

    int numprocs = 0;
    int maxPasswordLength = MAX_PASSWORD_LENGTH; //only if no argument is provided
    // read arguments
    for (size_t i = 1; i < argumentsCnt - 1; i+=2)
    {
        string type(argumentsPtr[i]);
        if(argumentsCnt < i + 1)
        {
            
        }
        string value(argumentsPtr[i+1]);

        if(type.compare("-pw") == 0)
            passwordToFind = argumentsPtr[i+1];
        else if(type.compare("-characterSet") == 0)
            characters = argumentsPtr[i+1];
        else if(type.compare("-np") == 0)
        {
            int intVal = stoi(value);
        }
        else if(type.compare("-logLevel") == 0)
        {
            LogLevels logLevels;
            logLevel = logLevels.LogLevelFromString(value);     
        }
        else if(type.compare("-maxPasswordLength") == 0)
        {
            maxPasswordLength = stoi(value);
            if(maxPasswordLength <= 0)
                maxPasswordLength = 1;
        }
        else
        {
            logger->Error("Unknown Parameter %s. Available options: -pw, -maxPasswordLength, -logLevel, -characterSet, -np", type.c_str());
            return 1;
        }
    }
    
    if(IsPasswordSetupValid(passwordToFind, characters, maxPasswordLength, logger) == false)
        return 1;

    int myid;
    MPI_Init(&argumentsCnt,&argumentsPtr);    // Start des MPI-Systems
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);  // Kommunikationsraum
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);      // Id ermitteln

    if(numprocs < 2)
    {
        logger->Error("The cracker needs at least two processes in config.");
        return AbortExecution();
    }

    logger = ConsoleLogger::Instance(logLevel, myid);
    PasswordChecker passwordChecker(passwordToFind, logger);

    const int charactersCount = characters.length();
    int splitRangeLength = ceil(1.00 * charactersCount / (numprocs - 1));

    if(myid == MASTER_PROCESS_ID)
    {  
        logger->Info("We have %d processors for cracking password [%s] having character set {%s} and max length %d", numprocs - 1, passwordToFind.c_str(), characters.c_str(), maxPasswordLength); 
        PasswordCrackerMainWorker mainWorker(characters, maxPasswordLength, numprocs, *logger, passwordChecker);
        mainWorker.StartCracking(splitRangeLength);
    }
    else // myid != 0 => Worker
    {
        PasswordCrackerWorker passwordCracker(characters, maxPasswordLength, *logger, passwordChecker);
        passwordCracker.CrackPassword(splitRangeLength);
    }

    logger->Info("Finished password search.");
    return StopExecution();
}
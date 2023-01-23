#include <stdio.h>
#include "PasswordChecker.h"
#include "PasswordGenerator.h"
#include <string>
#include <string.h>
#include <mpi.h>
#include <math.h>
#include <vector>
#include <bits/stdc++.h>
#include "MemoryObserver.h"
#include "ConsoleLogger.h"
#include "time.h"
#include <map>

using namespace std;

#define PASSWORDRANGE_TAG 0
#define PASSWORDFOUND_TAG PASSWORDRANGE_TAG + 1
#define ABORT_PROCESS_TAG PASSWORDFOUND_TAG + 1
#define MASTER_PROCESS_ID 0
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

bool IsPasswordOnlyOneLetter(int charactersCount, string characters, PasswordChecker passwordChecker, ConsoleLogger logger)
{
    logger.Debug("Master will check all one letter passwords"); 
    for (size_t i = 0; i < charactersCount; i++)
    {
        string charStr(1, characters[i]);
        logger.Debug("Checking password: %s", charStr.c_str()); 
        bool isValid = passwordChecker.IsPasswordValid(charStr);
        if(isValid)
        {
            logger.Info("found one letter password! %s", charStr.c_str()); 
            return true;
        }
    }

    return false;
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

    int numprocs;
    // read arguments
    for (size_t i = 1; i < argumentsCnt -1; i+=2)
    {
        string type(argumentsPtr[i]);
        string value(argumentsPtr[i+1]);

        if(type.compare("-pw") == 0)
            passwordToFind = argumentsPtr[i+1];
        else if(type.compare("-characterSet") == 0)
            characters = argumentsPtr[i+1];
        else if(type.compare("-np") == 0)
        {
            int intVal = stoi(value);
            if(intVal <= 1)
            {    
                logger->Error("The cracker needs at least two processes in config.");
                numprocs = 2;
            }
        }
        else if(type.compare("-logLevel") == 0)
        {
            LogLevels logLevels;
            logLevel = logLevels.LogLevelFromString(value);     
        }
    }
    
    const int charactersCount = characters.length();
    int myid;
    MPI_Init(&argumentsCnt,&argumentsPtr);    // Start des MPI-Systems
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);  // Kommunikationsraum
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);      // Id ermitteln
    MPI_Status stat; //sync variable

    logger = ConsoleLogger::Instance(logLevel, myid);
    PasswordChecker passwordChecker(passwordToFind, logger);

    if(IsPasswordSetupValid(passwordToFind, characters, MAX_PASSWORD_LENGTH, logger) == false)
        return 1;

    int splitRangeLength = 1;
    if(numprocs > 1)
        splitRangeLength = ceil(1.00 * charactersCount / (numprocs - 1));
        

    if(myid == MASTER_PROCESS_ID)
    {   
        if(IsPasswordOnlyOneLetter(charactersCount, characters, passwordChecker, *logger))
            return 0;
        
        logger->Debug("We have %d processors for cracking sending start characters to each one", numprocs - 1); 
        
        char receivedPassword[MAX_PASSWORD_LENGTH + 1] = "";
        receivedPassword[MAX_PASSWORD_LENGTH] = '\0';

        map<int, MPI_Request> waitForPasswordRequests; //sync variable //TODO: add arrays for each process
    
        for (int processId = 1; processId < numprocs; processId++)
        {
            if(processId > charactersCount)
            {   
                logger->Warning("There are more processors available than splitting possible.");
                break;
            }

            int startId = (processId - 1) * splitRangeLength;
            string startCharacters = characters.substr(startId, splitRangeLength);
            const char *c_startcharacters = startCharacters.c_str();
            
            MPI_Request sendPasswordRangeRequest;
            MPI_Isend(c_startcharacters, splitRangeLength, MPI_CHAR, processId, PASSWORDRANGE_TAG, MPI_COMM_WORLD, &sendPasswordRangeRequest);  // Senden eines Puffers mit Zeichenkette an Prozess i 
            
            MPI_Request waitForPasswordRequest;
            waitForPasswordRequests[processId] = waitForPasswordRequest;
            MPI_Irecv(receivedPassword, MAX_PASSWORD_LENGTH, MPI_CHAR, processId, PASSWORDFOUND_TAG, MPI_COMM_WORLD, &waitForPasswordRequest); // Empfang der Statusmeldung des Prozess i 
        }

        logger->Debug("Started waiting for answers. Currently the received password is [%s]", receivedPassword);
            
        int wasPasswordReceived = false;
        while(wasPasswordReceived == false)
        {
            for (const auto& requestMapping : waitForPasswordRequests) 
            {
                logger->Debug("Waiting for answers from process %d. Currently the received password is [%s]", requestMapping.first, receivedPassword);
            
                MPI_Request waitForPasswordRequest = requestMapping.second;
                MPI_Test(&waitForPasswordRequest, &wasPasswordReceived, MPI_STATUS_IGNORE); //wait for one process to answer the request
                
                if(wasPasswordReceived == true)
                {   
                    logger->Info("Received the answers from process %d. The received password is [%s]", requestMapping.first, receivedPassword);
                    break;
                }
            }

            if(wasPasswordReceived == true)
                break;
            
            sleep(1);
        }

        logger->Info("Received password: [%s]", receivedPassword); // Ausgabe empfangene Nachricht im Kommunikationsraum MPI_COMM_WORLD              
        //MPI_Wait(&waitForPasswordRequest, MPI_STATUS_IGNORE); //wait for one process to answer the request

        for (int processId = 1; processId < numprocs; processId++)
        {
            logger->Debug("Sending abort to process %d:", processId);
            int abort = 1;
            MPI_Send(&abort, 1, MPI_INT, processId, ABORT_PROCESS_TAG, MPI_COMM_WORLD);
        }
    }
    else // myid != 0 => Worker
    {
        char passwordRange[splitRangeLength + 1];
        passwordRange[splitRangeLength] = '\0';

        MPI_Recv(passwordRange, splitRangeLength, MPI_CHAR, MASTER_PROCESS_ID, PASSWORDRANGE_TAG, MPI_COMM_WORLD, &stat);
        int abort = false;
        MPI_Request stopRequest;
        MPI_Irecv(&abort, 1, MPI_INT, MASTER_PROCESS_ID, ABORT_PROCESS_TAG, MPI_COMM_WORLD, &stopRequest);
        logger->Info("I am assigned to password range starting from: [%s]", passwordRange); // Puffer zusammenstellen

        vector<string> passwords;

        for(size_t charId = 0; charId < strlen(passwordRange); charId++) //init chars array for start
        {
            string charStr(1, passwordRange[charId]);
            passwords.push_back(charStr);
        }
        
        MemoryObserver memoryObserver;
        for (size_t length = 2; length <= MAX_PASSWORD_LENGTH;)
        {
            logger->Info("Started checking passwords with length %d", length + 1);
            clock_t startClick = clock();

            string currentPass;
            int passwordsSize =  passwords.size();
            size_t passwordId = 0;
            for (; passwordId < passwordsSize; passwordId++)
            {
                currentPass = passwords[passwordId];   
                length = currentPass.length() + 1;

                logger->Debug("Remaining passwords in loop to be extended: %d", passwordsSize);

                for(size_t charId = 0; charId < charactersCount; charId++)
                {
                    MPI_Test(&stopRequest, &abort, MPI_STATUS_IGNORE);
                    if(abort == true)
                    {   
                        logger->Debug("Aborted: %s!", abort == 1 ? "true" : "false");
                        return StopExecution();
                    }

                    string charStr(1, characters[charId]);
                    string newPass = currentPass + charStr;
                    passwords.push_back(newPass);
                    logger->Debug("Checking password: [%s]. Memory Usage: %d kB", newPass.c_str(), memoryObserver.GetValue()); 
                    bool isValid = passwordChecker.IsPasswordValid(newPass.c_str());
                    if(isValid)
                    {
                        logger->Info("Password cracked! Password is: [%s]", newPass.c_str()); 
                        MPI_Send(newPass.c_str(), MAX_PASSWORD_LENGTH, MPI_CHAR, MASTER_PROCESS_ID, PASSWORDFOUND_TAG, MPI_COMM_WORLD);
                        return StopExecution();
                    }
                }
            }
            
            clock_t finishClick = clock();
            logger->Info("Finished checking passwords with length \t%d in \t%d ms", length + 1, (finishClick-startClick)/(CLOCKS_PER_SEC/1000));
            passwords.erase(passwords.begin(), passwords.begin() + passwordId); //remove all already checked passwords to save storage space
        }
    }

    logger->Info("Finished password search.");
    return StopExecution();
}
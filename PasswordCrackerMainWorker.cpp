#include <mpi.h>
#include "MPI_Definitions.h"
#include "PasswordCrackerMainWorker.h"

bool PasswordCrackerMainWorker::IsPasswordOnlyOneLetter()
{
    _logger.Debug("Master will check all one letter passwords"); 
    for (size_t i = 0; i < _charactersCount; i++)
    {
        string charStr(1, _characters[i]);
        _logger.Debug("Checking password: %s", charStr.c_str()); 
        bool isValid = _passwordChecker.IsPasswordValid(charStr);
        if(isValid)
        {
            _logger.Info("found one letter password! %s", charStr.c_str()); 
            return true;
        }
    }

    return false;
}

void PasswordCrackerMainWorker::StartCracking(int splitRangeLength)
{
    _logger.Debug("We have %d processors for cracking sending start characters to each one", _numProcs - 1); 
    
    char receivedPassword[_maxPasswordLength + 1] = "";
    receivedPassword[_maxPasswordLength] = '\0';

    MPI_Request waitForPasswordRequests[_numProcs - 1];

    for (int processId = 1; processId < _numProcs; processId++)
    {
        if(processId > _charactersCount)
        {   
            _logger.Warning("There are more processors available than splitting possible.");
            break;
        }

        int startId = (processId - 1) * splitRangeLength;
        string startCharacters = _characters.substr(startId, splitRangeLength);
        const char *c_startcharacters = startCharacters.c_str();
        
        MPI_Request sendPasswordRangeRequest;
        MPI_Isend(c_startcharacters, splitRangeLength, MPI_CHAR, processId, PASSWORDRANGE_TAG, MPI_COMM_WORLD, &sendPasswordRangeRequest);  // Senden eines Puffers mit Zeichenkette an Prozess i 
        
        MPI_Irecv(receivedPassword, _maxPasswordLength + 1, MPI_CHAR, processId, PASSWORDFOUND_TAG, MPI_COMM_WORLD, &waitForPasswordRequests[processId - 1]); // Empfang der Statusmeldung des Prozess i 
    }

    int requestsCount = sizeof(waitForPasswordRequests)/sizeof(waitForPasswordRequests[0]);
    int finishedWorkerId = 0;
    _logger.Info("Started waiting for %d answers. Currently the received password is [%s]", requestsCount, receivedPassword);
    
    if(IsPasswordOnlyOneLetter())
    {   
        return;
    }

    MPI_Waitany(requestsCount, waitForPasswordRequests, &finishedWorkerId, MPI_STATUS_IGNORE); //wait for one process to answer the request
    finishedWorkerId++; //worker id starts from 1 so increment is needed
    _logger.Info("Received password [%s] from process: %d", receivedPassword, finishedWorkerId);
    
    for (int processId = 1; processId < _numProcs; processId++)
    {
        if(finishedWorkerId == processId)
            continue; //skip already finished thread
        
        _logger.Info("Sending abort to process %d:", processId);
        int abort = 1;
        MPI_Send(&abort, 1, MPI_INT, processId, ABORT_PROCESS_TAG, MPI_COMM_WORLD);
    }
}

PasswordCrackerMainWorker::PasswordCrackerMainWorker(string characters
                                                    , int maxPasswordLength
                                                    , int numProcs
                                                    , ConsoleLogger& logger
                                                    , PasswordChecker& passwordChecker) :
                                                    _characters{characters}
                                                    , _charactersCount{(int)characters.length()}
                                                    , _maxPasswordLength{maxPasswordLength}
                                                    , _numProcs{numProcs}
                                                    , _logger{logger}
                                                    , _passwordChecker{passwordChecker}
{

}

PasswordCrackerMainWorker::~PasswordCrackerMainWorker()
{

}
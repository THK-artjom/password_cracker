#include <mpi.h>
#include "MPI_Definitions.h"
#include "PasswordCrackerMainWorker.h"
#include <math.h>

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

    int usableWorkerCount = ceil(1.00 * _charactersCount / splitRangeLength);
    MPI_Request waitForPasswordRequests[usableWorkerCount];

    if(_numProcs -1 > usableWorkerCount)
        _logger.Warning("Allgorithm can currently only use %d worker as of work seperation is done by character set length %d and the initial worker count can't be higher than %d.", usableWorkerCount, _charactersCount, _charactersCount); 
    
    for (int processId = 1; processId < usableWorkerCount + 1; processId++)
    {
        int startId = (processId - 1) * splitRangeLength;
        if(startId >= _charactersCount)
        {
            _logger.Warning("Too many cores available. Can not select characters start range from pos %d with length %d from character set [%s} having count %d.", startId, splitRangeLength, _characters.c_str(), _charactersCount);
            break;
        }

        string startCharacters;
        if(startId + splitRangeLength > _charactersCount)
            startCharacters = _characters.substr(startId, _charactersCount - startId);
        else
            startCharacters = _characters.substr(startId, splitRangeLength);
    
        const char *c_startcharacters = startCharacters.c_str();
        
        MPI_Request sendPasswordRangeRequest;
        MPI_Isend(c_startcharacters, startCharacters.length(), MPI_CHAR, processId, PASSWORDRANGE_TAG, MPI_COMM_WORLD, &sendPasswordRangeRequest); //send range to worker without waiting
        MPI_Irecv(receivedPassword, _maxPasswordLength + 1, MPI_CHAR, processId, PASSWORDFOUND_TAG, MPI_COMM_WORLD, &waitForPasswordRequests[processId - 1]); //init receive of password from each worker
    }

    int requestsCount = sizeof(waitForPasswordRequests)/sizeof(waitForPasswordRequests[0]);
    int finishedWorkerId = 0;
    _logger.Info("Started waiting for %d answers. Currently the received password is [%s]", requestsCount, receivedPassword);
    
    if(IsPasswordOnlyOneLetter())
        return;

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
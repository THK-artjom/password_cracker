#include <mpi.h>
#include <vector>
#include <string.h>

#include "MPI_Definitions.h"
#include "MemoryObserver.h"
#include "PasswordCrackerWorker.h"

PasswordCrackerWorker::PasswordCrackerWorker(string characters
                                        , int maxLength
                                        , ConsoleLogger& logger
                                        , PasswordChecker& passwordChecker) : 
                                        _characters{characters}
                                        , _charactersCount{(int)characters.length()}
                                        , _maxLength{maxLength}
                                        , _logger{logger}
                                        , _passwordChecker{passwordChecker}
{

}

void PasswordCrackerWorker::CrackPassword(int splitRangeLength)
{
    char passwordRange[splitRangeLength + 1];
    passwordRange[splitRangeLength] = '\0';

    MPI_Recv(passwordRange, splitRangeLength, MPI_CHAR, MASTER_PROCESS_ID, PASSWORDRANGE_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    int abort = false;
    MPI_Request stopRequest;
    MPI_Irecv(&abort, 1, MPI_INT, MASTER_PROCESS_ID, ABORT_PROCESS_TAG, MPI_COMM_WORLD, &stopRequest);
    _logger.Info("I am assigned to password range starting from: [%s]", passwordRange); // Puffer zusammenstellen

    vector<string> passwords;

    for(size_t charId = 0; charId < strlen(passwordRange); charId++) //init chars array for start
    {
        string charStr(1, passwordRange[charId]);
        passwords.push_back(charStr);
    }
    
    MemoryObserver memoryObserver;
    for (size_t length = 2; length <= _maxLength;)
    {
        _logger.Debug("Started checking passwords with length %d", length + 1);
        clock_t startClick = clock();

        string currentPass;
        int passwordsSize =  passwords.size();
        size_t passwordId = 0;
        for (; passwordId < passwordsSize; passwordId++)
        {
            currentPass = passwords[passwordId];   
            length = currentPass.length() + 1;

            _logger.Debug("Remaining passwords in loop to be extended: %d", passwordsSize);

            for(size_t charId = 0; charId < _charactersCount; charId++)
            {
                MPI_Test(&stopRequest, &abort, MPI_STATUS_IGNORE);
                if(abort == true)
                {   
                    _logger.Debug("Aborted: %s!", abort == 1 ? "true" : "false");
                    return;
                }

                string charStr(1, _characters[charId]);
                string newPass = currentPass + charStr;
                passwords.push_back(newPass);
                _logger.Debug("Checking password: [%s]. Memory Usage: %d kB", newPass.c_str(), memoryObserver.GetValue()); 
                bool isValid = _passwordChecker.IsPasswordValid(newPass.c_str());
                if(isValid)
                {
                    const char* crackedPassword = newPass.c_str(); 
                    _logger.Info("Password cracked! Password is: [%s]", crackedPassword); 
                    MPI_Send(crackedPassword, strlen(crackedPassword), MPI_CHAR, MASTER_PROCESS_ID, PASSWORDFOUND_TAG, MPI_COMM_WORLD);
                    return;
                }
            }
        }
        
        clock_t finishClick = clock();
        _logger.Info("Finished checking passwords with length \t%d in \t%d ms", length + 1, (finishClick-startClick)/(CLOCKS_PER_SEC/1000));
        passwords.erase(passwords.begin(), passwords.begin() + passwordId); //remove all already checked passwords to save storage space
    }
}

PasswordCrackerWorker::~PasswordCrackerWorker()
{
    
}
#include "PasswordChecker.h"
#include <stdio.h>
#include <string>

using namespace std;

PasswordChecker::PasswordChecker(string password, ConsoleLogger* logger): _logger{logger}, _password{password} 
{
}

bool PasswordChecker::IsPasswordValid(string password)
{
    _logger->Debug("Checking Password: %s", password.c_str());
    bool arePasswdEqual = _password.compare(password) == 0;
    _logger->Debug("Password: %s is %s", password.c_str(), arePasswdEqual == true ? "true" : "false");
    return arePasswdEqual;
}

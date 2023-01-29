#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include "MemoryObserver.h"

using namespace std;

int MemoryObserver::ParseLine(char* line)
{
    // This assumes that a digit will be found and the line ends in " Kb".
    // e.g. VmSize:	  188180 kB

    string memoryLine(line);
    int nameLength = strlen("VmSize: ");
    int unitLength = strlen(" kB");
    memoryLine = memoryLine.substr(nameLength, memoryLine.length() - nameLength);
    memoryLine = memoryLine.substr(0, memoryLine.length() - unitLength);
    //cout << "Read Mem Usage: [" << memoryLine << "]";
    int value = atoi(memoryLine.c_str());
    return value;
}

int MemoryObserver::GetValue()
{
    FILE* file = fopen("/proc/self/status", "r"); //read all information about current prozess
    int result = -1;
    char line[128];

    while (fgets(line, 128, file) != NULL)
    {
        //cout << "Checking Line: " << line;
        if (strncmp(line, "VmSize:", 7) == 0)//Der gesamte Speicherverbrauch des Prozesses. Enthalten sind Text- und Datensegment, Stack, statische Variablen sowie Seiten, die mir anderen Prozessen geteilt werden.
        {   
            //cout << "Read Mem Usage: " << line;
            result = ParseLine(line);//Note: this value is in KB!
            break;
        }
    }
    fclose(file);
    return result;
}
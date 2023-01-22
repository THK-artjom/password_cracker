#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "MemoryObserver.h"

int MemoryObserver::ParseLine(char* line)
{
    // This assumes that a digit will be found and the line ends in " Kb".
    int i = strlen(line);
    const char* p = line;
    while (*p <'0' || *p > '9') p++;
    line[i-3] = '\0';
    i = atoi(p);
    return i;
}

int MemoryObserver::GetValue()
{ //Note: this value is in KB!
    FILE* file = fopen("/proc/self/status", "r");
    int result = -1;
    char line[128];

    while (fgets(line, 128, file) != NULL){
        if (strncmp(line, "VmSize:", 7) == 0){
            result = ParseLine(line);
            break;
        }
    }
    fclose(file);
    return result;
}
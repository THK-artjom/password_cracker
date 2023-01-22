#include "PasswordGenerator.h"
#include <string.h>
#include <list>
#include <stdio.h>

PasswordGenerator::PasswordGenerator(const char* characters): _characters{characters}
{
    _characterCount = strlen(_characters);
}

//_charcters: abcd
// _characterCount: 4
// 0: a
// 1: b
// 2: c

// 4=Id/_charactersCount + Id%_charactersCount: aa
// 5: ab

char* PasswordGenerator::Generate(int passwordId)
{
    printf("Started password generation for password id: %d", passwordId);

    char* password;
    char lastCharEnd = GetLastCharacterFromId(&passwordId);
    password = strcat(password, &lastCharEnd);

    if(passwordId < _characterCount)
    {
        int nextCharId = (passwordId - passwordId % _characterCount)/_characterCount - 1;
        char nextChar = GetLastCharacterFromId(&nextCharId);
        password = strcat(password, &nextChar);
    }

    printf("Generated password %s for password id %d", password, passwordId);
    return password;
}

//char** GetPasswordCombinations(int length)
//{
//    char* characterSet = "abc";
//    char** 
//    for (size_t i = 0; i < strlen(characterSet); i++)
//    {
//        char nextChar = *(&characterSet + i);
//    }  
//}

char PasswordGenerator::GetLastCharacterFromId(int *passwordId)
{
    if(*passwordId < 0)
        return 0;

    int lastCharAddr = *passwordId % _characterCount;
    char lastChar = GetCharacterFromSet(lastCharAddr);
    printf("Generating password for password id %d. LastCharacter is %c", *passwordId, lastChar);
    *passwordId-=lastCharAddr;
    return lastChar;
}

//return the character from character set 
// id: 0 - _characterCount - 1
char PasswordGenerator::GetCharacterFromSet(int id)
{
    if(id >= _characterCount
    || id < 0)
        return 0;

    return *(_characters + id);
}

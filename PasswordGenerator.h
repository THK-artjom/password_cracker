#pragma once

class PasswordGenerator
{
    private:
     const char* _characters;
     int _characterCount;
     char GetCharacterFromSet(int id);
     char GetLastCharacterFromId(int* passwordId);
    public:
        PasswordGenerator(const char* characters); 
        char* Generate(int passwordId);
};

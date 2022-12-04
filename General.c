#include "Common.h"
#include "Structs.h"

/* General functions used everywhere */

extern System_t System;

int searchStringList(char* string, char* string_list[], int num_strings)
{
    int i;
    for (i = 0; i < num_strings; i++)
    {
        if (strcmp(string, string_list[i]) == 0)
            return i;
    }
    
    return RETURN_ERROR;
}

void quitError(char* string)
{
    setVideoMode(TEXT_MODE);
    printf("%s", string);
    System.running = 0;
}
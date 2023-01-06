#include "Common.h"
#include "Structs.h"

/* General functions used everywhere */

extern System_t System;

int searchStringList_(char* string, char* string_list[], int num_strings)
{
    int i;
    for (i = 0; i < num_strings; i++)
    {
        if (strcmp(string, string_list[i]) == 0)
            return i;
    }
    
    return RETURN_ERROR;
}

int searchStringList(char* string, StringList_t stringlist)
{
    return searchStringList_(string, stringlist.list, stringlist.count);
}

void freeStringList(StringList_t stringlist)
{
    int i;
    for (i = 0; i < stringlist.count; i++)
        free(stringlist.list[i]);

    free(stringlist.list);
}

void quitError(char* string)
{
    setVideoMode(TEXT_MODE);
    printf("%s", string);
    System.running = 0;
}
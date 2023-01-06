#ifndef GENERAL_H
#define GENERAL_H

#include "Structs.h"

int searchStringList_(char* string, char* string_list[], int num_strings);
int searchStringList(char* string, StringList_t stringlist);
void freeStringList(StringList_t stringlist);
void quitError(char* string);

#endif/* GENERAL_H */

#ifndef FILECH_H
#define FILECH_H

#include "Structs.h"

int checkDirectoryExists(char* dirname);
int checkFileExists(char* filename);
int checkFileExtension(char* filename, char* ext);

void createDirectory(char* path);
void deleteDirectoryContents(char* dirname);
void copyAllFolderToFolder(char* source_dirname, char* dest_dirname);

int countSubdirectories(char* dirname);
int countFiles(char* dirname);
int countFilesByExtension(char* dirname, char* ext);

StringList_t listSubdirectories(char* dirname);
StringList_t listFiles(char* dirname);
StringList_t listFilesByExtension(char* dirname);

#endif/* FILECH_H */

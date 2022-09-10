#ifndef FILECH_H
#define FILECH_H

int checkFileExists(char* filename);
int checkDirectoryExists(char* directory);
void createDirectory(char* path);
void deleteAutosaves();

#endif /* FILECH_H */

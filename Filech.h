#ifndef FILECH_H
#define FILECH_H

int checkFileExists(char* filename);
int checkDirectoryExists(char* directory);
void createDirectory(char* path);
int listSubdirectories(char* directory);
void deleteDirectoryContents(char* directory);

#endif /* FILECH_H */

#ifndef FILECH_H
#define FILECH_H

int checkFileExists(char* filename);
int checkDirectoryExists(char* directory);
void createDirectory(char* path);
void listSubdirectories(char* directory, char** dir_list);
int countSubdirectories(char* directory);
void deleteDirectoryContents(char* directory);
void copyAllFolderToFolder(char* source_dir, char* destination_dir);

#endif /* FILECH_H */

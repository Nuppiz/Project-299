#ifndef FILECH_H
#define FILECH_H

int checkFileExists(char* filename);
int checkDirectoryExists(char* directory);

void createDirectory(char* path);
void deleteDirectoryContents(char* directory);
void copyAllFolderToFolder(char* source_dir, char* destination_dir);

int countSubdirectories(char* directory);
int listSubdirectories(char* directory, char*** dir_list);

int countFiles(char* directory);
int listFiles(char* directory, char** file_list);

int checkFileExtension(char* filename, char* ext);
int countFilesByExtension(char* directory, char* ext);
int listFilesByExtension(char* directory, char* ext, char** file_list);

#endif /* FILECH_H */

#include "Common.h"
#include <dir.h>
#include <sys/stat.h>
#include <dirent.h>

/* File and directory check, creation and deletion functions */

#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)

int checkFileExists(char* filename)
{
    FILE* fp = fopen(filename, "r");
    int exists = FALSE;
    if (fp != NULL)
    {
        exists = TRUE;
        fclose(fp); // close the file
    }
    return exists;
}

int checkDirectoryExists(char* directory)
{
    struct stat stats;

    stat(directory, &stats);

    // Check for folder existence
    if (S_ISDIR(stats.st_mode))
        return TRUE;

    return FALSE;
}

void createDirectory(char* path)
{
    if (!checkDirectoryExists(path))
    {
        mkdir(path);
    }
}

char** listSubdirectories(char* directory, int directory_count)
{
    struct dirent* dir;
    char** dir_list = malloc(directory_count * sizeof(char*));
    int dir_i = 0;

    DIR* dr = opendir(directory);

    if (dr == NULL)  // opendir returns NULL if couldn't open directory
    {
        printf("Could not open current directory");
        return NULL;
    }

    while ((dir = readdir(dr)) != NULL)
    {
        if (dir->d_name[0] != '.')
        {
            dir_list[dir_i] = malloc(strlen(dir->d_name + 1));
            strcpy(dir_list[dir_i], dir->d_name);
            dir_i++;
        }
    }
  
    closedir(dr);
    return dir_list;
}

int countSubdirectories(char* directory)
{
    struct dirent* dir;
    int dir_count = 0;

    DIR* dr = opendir(directory);

    if (dr == NULL)  // opendir returns NULL if couldn't open directory
    {
        printf("Could not open current directory");
        return 0;
    }

    while ((dir = readdir(dr)) != NULL)
    {
        if (dir->d_name[0] != '.')
        {
            dir_count++;
        }
    }
  
    closedir(dr);    
    return dir_count;
}

void deleteDirectoryContents(char* directory)
{

    DIR* folder = opendir(directory);
    struct dirent* next_file;
    char filepath[50];

    while ((next_file = readdir(folder)) != NULL)
    {
        // build the path for each file in the folder
        sprintf(filepath, "%s/%s", directory, next_file->d_name);
        remove(filepath);
    }
    closedir(folder);
}
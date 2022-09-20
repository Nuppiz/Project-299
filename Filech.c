#include "Common.h"
#include <dir.h>
#include <sys/stat.h>
#include <dirent.h>

/* File and directory check, creation and deletion functions */

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
    if (stats.st_mode & S_IFDIR)
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

void listSubdirectories(char* directory, char** dir_list)
{
    struct dirent* dir;
    struct stat stats;
    int dir_i = 0;
    DIR* dr = opendir(directory);

    if (dr == NULL)  // opendir returns NULL if couldn't open directory
    {
        printf("Could not open current directory");
    }

    while ((dir = readdir(dr)) != NULL)
    {
        stat(dir->d_name, &stats);
        if (stats.st_mode & S_IFDIR) // only list folders (dirent can be both a file or a folder)
        {
            if (dir->d_name[0] != '.')
            {
                dir_list[dir_i] = malloc(strlen(dir->d_name + 1));
                strcpy(dir_list[dir_i], dir->d_name);
                dir_i++;
            }
        }
    }
  
    closedir(dr);
}

int countSubdirectories(char* directory)
{
    struct dirent* dir;
    struct stat stats;
    int dir_count = 0;
    DIR* dr = opendir(directory);

    if (dr == NULL)  // opendir returns NULL if couldn't open directory
    {
        printf("Could not open current directory");
        return 0;
    }

    while ((dir = readdir(dr)) != NULL)
    {
        stat(dir->d_name, &stats);
        if (stats.st_mode & S_IFDIR) // only count folders (dirent can be both a file or a folder)
        {
            if (dir->d_name[0] != '.')
            {
                dir_count++;
            }
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

void copyFile(char* source_dir, char* source_filename, char* destination_dir, char* destination_filename)
{
    FILE* source_file, *dest_file;
    char source_path[50] = {'\0'}; // container for the full path to source file
    char dest_path[50] = {'\0'};  // container for the full path to destination file
    char c; // byte that is read/written

    // construct full file paths
    strcpy(source_path, source_dir);
    strcat(source_path, source_filename);
    strcpy(dest_path, destination_dir);
    strcat(dest_path, destination_filename);
  
    // open first file for reading
    source_file = fopen(source_path, "rb");
    if (!source_file)
    {
        printf("%s\n", source_path);
        perror("fopen");
        delay(60000);
    }
  
    // open second file for writing
    dest_file = fopen(dest_path, "wb");
    if (!dest_file)
    {
        printf("%s\n", dest_path);
        perror("fopen");
        delay(60000);
    }
  
    // read contents from file
    c = fgetc(source_file);
    while (!feof(source_file))
    {
        fputc(c, dest_file);
        c = fgetc(source_file);
    }
  
    fclose(source_file);
    fclose(dest_file);
}

void copyAllFolderToFolder(char* source_dir, char* destination_dir)
{
    DIR* folder = opendir(source_dir);
    struct dirent* file;
    char filename[15] = {"\0"};

    while ((file = readdir(folder)) != NULL)
    {
        // copy filename to buffer
        if (file->d_name[0] != '.')
        {
            sprintf(filename, "%s", file->d_name);
            copyFile(source_dir, file->d_name, destination_dir, file->d_name);
        }
    }
    closedir(folder);
}
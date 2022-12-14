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
    mkdir(path);
}

int countSubdirectories(char* directory)
{
    struct dirent* dir;
    struct stat stats;
    int dir_count = 0;
    DIR* dr = opendir(directory);

    if (dr == NULL)  // opendir returns NULL if couldn't open directory
    {
        quitError("Could not open directory!\n");
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

int listSubdirectories(char* directory, char*** dir_list)
{
    struct dirent* dir;
    struct stat stats;
    int dir_i = 0;
    int dir_count = 0;
    DIR* dr;

    dir_count = countSubdirectories(directory);
    *dir_list = malloc(dir_count * sizeof(char*));

    dr = opendir(directory);

    if (dr == NULL)  // opendir returns NULL if couldn't open directory
    {
        quitError("Could not open current directory!\n");
        return -1;
    }

    while ((dir = readdir(dr)) != NULL)
    {
        stat(dir->d_name, &stats);
        if (stats.st_mode & S_IFDIR) // only list folders (dirent can be both a file or a folder)
        {
            if (dir->d_name[0] != '.')
            {
                *dir_list[dir_i] = malloc(strlen(dir->d_name + 1));
                strcpy(*dir_list[dir_i], dir->d_name);
                dir_i++;
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

int checkFileExtension(char* filename, char* ext)
{
    char c;
    int i;
    int ext_start = -1;

    for (i = 0; (c = filename[i]) != '\0'; i++)
    {
        if (c == '.')
            ext_start = i;
    }

    if (ext_start < 0)
        return 0;

    return strcmp(&filename[ext_start+1], ext);
}

int countFiles(char* directory)
{
    struct dirent* file;
    struct stat stats;
    int file_count = 0;
    DIR* dr = opendir(directory);

    if (dr == NULL)  // opendir returns NULL if couldn't open directory
    {
        quitError("Could not open directory!\n");
        return 0;
    }

    while ((file = readdir(dr)) != NULL)
    {
        stat(file->d_name, &stats);
        if ((stats.st_mode & S_IFDIR) == 0) // only count files
            file_count++;
    }
  
    closedir(dr);
    return file_count;
}

int countFilesByExtension(char* directory, char* ext)
{
    struct dirent* file;
    struct stat stats;
    int file_count = 0;
    DIR* dr = opendir(directory);

    if (dr == NULL)  // opendir returns NULL if couldn't open directory
    {
        quitError("Could not open directory!\n");
        return 0;
    }

    while ((file = readdir(dr)) != NULL)
    {
        stat(file->d_name, &stats);
        // only count matching files
        if ((stats.st_mode & S_IFDIR) == 0 && checkFileExtension(file->d_name, ext) == 0)
            file_count++;
    }
  
    closedir(dr);
    return file_count;
}

int listFiles(char* directory, char** file_list)
{
    struct dirent* file;
    struct stat stats;
    int file_i = 0;
    int file_count = 0;
    DIR* dr;

    file_count = countFiles(directory);
    file_list = malloc(file_count * sizeof(char*));

    dr = opendir(directory);

    if (dr == NULL)  // opendir returns NULL if couldn't open directory
    {
        quitError("Could not open current directory!\n");
        return -1;
    }

    while ((file = readdir(dr)) != NULL)
    {
        stat(file->d_name, &stats);
        if ((stats.st_mode & S_IFDIR) == 0) // only list files
        {
            file_list[file_i] = malloc(strlen(file->d_name + 1));
            strcpy(file_list[file_i], file->d_name);
            file_i++;
        }
    }
  
    closedir(dr);
    return file_count;
}

int listFilesByExtension(char* directory, char* ext, char** file_list)
{
    struct dirent* file;
    struct stat stats;
    int file_i = 0;
    int file_count = 0;
    DIR* dr;

    file_count = countFilesByExtension(directory, ext);
    file_list = malloc(file_count * sizeof(char*));

    dr = opendir(directory);

    if (dr == NULL)  // opendir returns NULL if couldn't open directory
    {
        quitError("Could not open current directory!\n");
        return -1;
    }

    while ((file = readdir(dr)) != NULL)
    {
        stat(file->d_name, &stats);
        // only list matching files
        if ((stats.st_mode & S_IFDIR) == 0 && checkFileExtension(file->d_name, ext) == 0)
        {
            file_list[file_i] = malloc(strlen(file->d_name + 1));
            strcpy(file_list[file_i], file->d_name);
            file_i++;
        }
    }
  
    closedir(dr);
    return file_count;
}
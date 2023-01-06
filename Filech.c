#include "Common.h"
#include "Structs.h"
#include <dir.h>
#include <sys/stat.h>
#include <dirent.h>

/* File and directory check, creation and deletion functions */

#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)

int checkDirectoryExists(char* dirname)
{
    struct stat stats;

    stat(dirname, &stats);

    // Check for folder existence
    if (S_ISDIR(stats.st_mode))
        return TRUE;

    return FALSE;
}

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

// returns 0 if filename has a matching extension
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
        return 1;

    return strcmp(&filename[ext_start+1], ext);
}

void createDirectory(char* path)
{
    mkdir(path);
}

void deleteDirectoryContents(char* dirname)
{
    DIR* folder = opendir(dirname);
    struct dirent* next_file;
    char filepath[50];

    while ((next_file = readdir(folder)) != NULL)
    {
        // build the path for each file in the folder
        sprintf(filepath, "%s/%s", dirname, next_file->d_name);
        remove(filepath);
    }

    closedir(folder);
}

void copyFile(char* source_dirname, char* source_filename, char* dest_dirname, char* dest_filename)
{
    FILE* source_file, *dest_file;
    char source_path[50] = {'\0'}; // container for the full path to source file
    char dest_path[50] = {'\0'};  // container for the full path to destination file
    char c; // byte that is read/written

    // construct full file paths
    strcpy(source_path, source_dirname);
    strcat(source_path, source_filename);
    strcpy(dest_path, dest_dirname);
    strcat(dest_path, dest_filename);
  
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

void copyAllFolderToFolder(char* source_dirname, char* dest_dirname)
{
    DIR* folder = opendir(source_dirname);
    struct dirent* file;
    char filename[15] = {"\0"};

    while ((file = readdir(folder)) != NULL)
    {
        // copy filename to buffer
        if (file->d_name[0] != '.')
        {
            sprintf(filename, "%s", file->d_name);
            copyFile(source_dirname, file->d_name, dest_dirname, file->d_name);
        }
    }

    closedir(folder);
}

int countSubdirectories(char* dirname)
{
    struct dirent* dir;
    struct stat stats;
    int dir_count = 0;
    DIR* dr = opendir(dirname);

    if (dr == NULL)  // opendir returns NULL if couldn't open directory
    {
        quitError("Could not open directory!\n");

        return 0;
    }

    while ((dir = readdir(dr)) != NULL)
    {
        stat(dir->d_name, &stats);

        // only count folders (dirent can be both a file or a folder)
        if ((stats.st_mode & S_IFDIR) && dir->d_name[0] != '.')
            dir_count++;
    }
  
    closedir(dr);

    return dir_count;
}

int countFiles(char* dirname)
{
    struct dirent* file;
    struct stat stats;
    int file_count = 0;
    DIR* dr = opendir(dirname);

    if (dr == NULL)  // opendir returns NULL if couldn't open directory
    {
        quitError("Could not open directory!\n");
        return 0;
    }

    while ((file = readdir(dr)) != NULL)
    {
        stat(file->d_name, &stats);
        // only count files
        if ((stats.st_mode & S_IFDIR) == 0)
            file_count++;
    }
  
    closedir(dr);

    return file_count;
}

int countFilesByExtension(char* dirname, char* ext)
{
    struct dirent* file;
    struct stat stats;
    int file_count = 0;
    DIR* dr = opendir(dirname);

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

StringList_t listSubdirectories(char* dirname)
{
    StringList_t subdirs;
    struct dirent* dir;
    struct stat stats;
    int dir_i = 0;
    DIR* dr;

    subdirs.count = 0;
    subdirs.list = NULL;

    dr = opendir(dirname);

    if (dr == NULL)  // opendir returns NULL if couldn't open directory
    {
        quitError("Could not open current directory!\n");
        
        return subdirs;
    }

    subdirs.count = countSubdirectories(dirname);
    subdirs.list = malloc(subdirs.count * sizeof(char*));

    while ((dir = readdir(dr)) != NULL)
    {
        stat(dir->d_name, &stats);

        // only list folders (dirent can be both a file or a folder)
        if ((stats.st_mode & S_IFDIR) && dir->d_name[0] != '.')
        {
            subdirs.list[dir_i] = malloc(strlen(dir->d_name + 1));
            strcpy(subdirs.list[dir_i], dir->d_name);
            dir_i++;
        }
    }
  
    closedir(dr);

    return subdirs;
}

StringList_t listFiles(char* dirname)
{
    StringList_t files;
    struct dirent* file;
    struct stat stats;
    int file_i = 0;
    DIR* dr;

    files.count = 0;
    files.list = NULL;

    dr = opendir(dirname);

    if (dr == NULL)  // opendir returns NULL if couldn't open directory
    {
        quitError("Could not open current directory!\n");
        
        return files;
    }

    files.count = countFiles(dirname);
    files.list = malloc(files.count * sizeof(char*));
    
    while ((file = readdir(dr)) != NULL)
    {
        stat(file->d_name, &stats);
        // only list files
        if ((stats.st_mode & S_IFDIR) == 0)
        {
            files.list[file_i] = malloc(strlen(file->d_name + 1));
            strcpy(files.list[file_i], file->d_name);
            file_i++;
        }
    }
  
    closedir(dr);

    return files;
}

StringList_t listFilesByExtension(char* dirname, char* ext)
{
    StringList_t files;
    struct dirent* file;
    struct stat stats;
    int file_i = 0;
    DIR* dr;

    files.count = 0;
    files.list = NULL;

    dr = opendir(dirname);

    if (dr == NULL)  // opendir returns NULL if couldn't open directory
    {
        quitError("Could not open current directory!\n");
        
        return files;
    }

    files.count = countFilesByExtension(dirname, ext);
    files.list = malloc(files.count * sizeof(char*));
    
    while ((file = readdir(dr)) != NULL)
    {
        stat(file->d_name, &stats);
        // only list matching files
        if ((stats.st_mode & S_IFDIR) == 0 && checkFileExtension(file->d_name, ext) == 0)
        {
            files.list[file_i] = malloc(strlen(file->d_name + 1));
            strcpy(files.list[file_i], file->d_name);
            file_i++;
        }
    }
  
    closedir(dr);

    return files;
}
#include "Common.h"
#include <dir.h>
#include <sys/stat.h>

/* File and directory check and creation functions */

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
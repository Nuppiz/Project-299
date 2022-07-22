#include "Common.h"
#include "Structs.h"

/* Graphics loading functions */

Texture_t* Textures = NULL;
int texture_count = 1;

void loadGfx(char* filename, uint8_t* destination, uint16_t data_size)
{
    // load raw graphics data (no dimensions and flags)
    FILE* file_ptr;
    file_ptr = fopen(filename, "rb");
    fread(destination, 1, data_size, file_ptr);
    fclose(file_ptr);
}

int findTexture(char* filename)
{
    int i;

    for (i = 0; i < texture_count; i++)
    {
        if (strcmp(filename, Textures[i].filename) == 0)
            return i;
    }
    return 0;
}

void createErrorTexture()
{
    int i;
    Textures = malloc(sizeof(Texture_t));
    Textures[0].filename = "ERROR.7UP";
    Textures[0].width = 20;
    Textures[0].height = 20;
    Textures[0].pixels = malloc(Textures[0].width * Textures[0].height);
    ASSERT(Textures[0].pixels != NULL);
    for (i = 0; i < 400; i++)
    {
        Textures[0].pixels[i] = COLOUR_RED;
    }
    ASSERT(Textures[0].pixels[0] == COLOUR_RED);
}

int loadTexture(char* filename)
{
    //loop all textures here to check if it was already loaded,
    //return id of that texture if it was found.
    int texture_id = 1;
    int filename_length;
    FILE* file_ptr;

    if ((texture_id = findTexture(filename)) != 0)
        return texture_id;

    file_ptr = fopen(filename, "rb");
    if (filename == NULL)
        return 0;

    if (Textures == NULL)
        createErrorTexture();
    else
        Textures = realloc(Textures, (texture_count + 1) * sizeof(Texture_t));

    ASSERT(Textures != NULL);

    texture_id = texture_count;
    ASSERT(texture_id != 0);
    texture_count++;

    filename_length = strlen(filename) + 1;
    Textures[texture_id].filename = malloc(filename_length);
    strcpy(Textures[texture_id].filename, filename);

    fread(&Textures[texture_id].width, 2, 1, file_ptr);
    fseek(file_ptr, 2, SEEK_SET);
    fread(&Textures[texture_id].height, 2, 1, file_ptr);
    fseek(file_ptr, 6, SEEK_SET);
    fread(&Textures[texture_id].transparent, 2, 1, file_ptr);
	fseek(file_ptr, 8, SEEK_SET);
    Textures[texture_id].pixels = malloc(Textures[texture_id].width * Textures[texture_id].height);
    fread(Textures[texture_id].pixels, 1, Textures[texture_id].width * Textures[texture_id].height, file_ptr);
    Textures[texture_id].offset_x = 0;
    Textures[texture_id].offset_y = 0;
    fclose(file_ptr);

    return texture_id;
}
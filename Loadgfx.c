#include "Common.h"
#include "Structs.h"
#include "Exit.h"

/* Graphics loading functions */

extern System_t System;

static uint8_t error_pixels[400] =
{
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,5,5,5,0,5,5,0,0,5,5,0,0,0,0,0,0,0,0,0,
0,5,0,0,0,5,0,5,0,5,0,5,0,0,0,0,0,0,0,0,
0,5,5,5,0,5,5,0,0,5,5,0,0,0,0,0,0,0,0,0,
0,5,0,0,0,5,0,5,0,5,0,5,0,0,0,0,0,0,0,0,
0,5,5,5,0,5,0,5,0,5,0,5,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

Texture_array BaseTextures = {0};
Texture_array ActorTextures = {0};
Texture_array TileTextures = {0};

void loadGfx(char* filename, uint8_t* destination, uint16_t data_size)
{
    // load raw graphics data (no dimensions and flags)
    FILE* file_ptr;
    file_ptr = fopen(filename, "rb");
    fread(destination, 1, data_size, file_ptr);
    fclose(file_ptr);
}

void loadTexturesFromList(char* list_filename, Texture_array* array)
{
    FILE* tex_list_file;
    char filename[20];

    tex_list_file = fopen(list_filename, "r");

    if (tex_list_file == NULL)
    {
        fclose(tex_list_file);
        setVideoMode(TEXT_MODE);
        printf("Unable to open texture list file!\n");
        printf("Please check the file actually exists!\n");
        System.running = 0;
    }

    do
    {
        fscanf(tex_list_file, "%s", filename);
        loadTexture(filename, array);
    } while (fgetc(tex_list_file) != EOF);
    
    fclose(tex_list_file);
}

int findTexture(char* filename, Texture_array* array)
{
    int i;

    for (i = 0; i < array->texture_count; i++)
    {
        if (strcmp(filename, array->textures[i].filename) == 0)
            return i;
    }
    return 0;
}

void createErrorTextures()
{
    BaseTextures.textures = malloc(sizeof(Texture_t));
    ActorTextures.textures = malloc(sizeof(Texture_t));
    TileTextures.textures = malloc(sizeof(Texture_t));
    BaseTextures.textures[0].filename = "ERROR.7UP";
    BaseTextures.textures[0].width = 20;
    BaseTextures.textures[0].height = 20;
    BaseTextures.textures[0].pixels = error_pixels;
    BaseTextures.texture_count++;
    ActorTextures.textures[0].filename = "ERROR.7UP";
    ActorTextures.textures[0].width = 20;
    ActorTextures.textures[0].height = 20;
    ActorTextures.textures[0].pixels = error_pixels;
    ActorTextures.textures[0].filename = "ERROR.7UP";
    ActorTextures.texture_count++;
    TileTextures.textures[0].width = 20;
    TileTextures.textures[0].height = 20;
    TileTextures.textures[0].pixels = error_pixels;
    TileTextures.texture_count++;
}

int loadTexture(char* filename, Texture_array* array)
{
    //loop all textures here to check if it was already loaded,
    //return id of that texture if it was found.
    int texture_id = 0;
    int filename_length;
    FILE* file_ptr;

    //return 0;

    if ((texture_id = findTexture(filename, array)) != 0)
        return texture_id;

    file_ptr = fopen(filename, "rb");
    if (file_ptr == NULL)
        return 0;

    array->textures = realloc(array->textures, (array->texture_count + 1) * sizeof(Texture_t));

    ASSERT(array->textures != NULL);

    texture_id = array->texture_count;
    array->texture_count++;

    filename_length = strlen(filename) + 1;
    array->textures[texture_id].filename = malloc(filename_length);
    strcpy(array->textures[texture_id].filename, filename);

    fread(&array->textures[texture_id].width, 2, 1, file_ptr);
    fseek(file_ptr, 2, SEEK_SET);
    fread(&array->textures[texture_id].height, 2, 1, file_ptr);
    fseek(file_ptr, 6, SEEK_SET);
    fread(&array->textures[texture_id].transparent, 2, 1, file_ptr);
	fseek(file_ptr, 8, SEEK_SET);
    array->textures[texture_id].pixels = malloc(array->textures[texture_id].width * array->textures[texture_id].height);
    fread(array->textures[texture_id].pixels, 1, array->textures[texture_id].width * array->textures[texture_id].height, file_ptr);
    array->textures[texture_id].offset_x = 0;
    array->textures[texture_id].offset_y = 0;

    fclose(file_ptr);
    
    return texture_id;
}

void loadBaseTextures()
{
    loadTexturesFromList("SPRITES/BASETEX.txt", &BaseTextures);
    ASSERT(BaseTextures.texture_count == 3);
    loadTexturesFromList("SPRITES/ACTORTEX.txt", &ActorTextures);
}

void freeAllTextures()
{
    int i;

    for (i = 1; i < TileTextures.texture_count; i++)
    {
        free(TileTextures.textures[i].filename);
        free(TileTextures.textures[i].pixels);
    }
    TileTextures.textures = realloc(TileTextures.textures, sizeof(Texture_t));
    TileTextures.texture_count = 1;

    /*for (i = 1; i < ActorTextures.texture_count; i++)
    {
        free(ActorTextures.textures[i].filename);
        free(ActorTextures.textures[i].pixels);
    }
    ActorTextures.textures = realloc(ActorTextures.textures, sizeof(Texture_t));
    ActorTextures.texture_count = 1;*/
}
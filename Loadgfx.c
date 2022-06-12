#include "Common.h"
#include "Enums.h"
#include "Structs.h"

/* Graphics loading functions */

Texture Textures[NUM_TEXTURES];

void loadGfx(char* filename, uint8_t* destination, uint16_t data_size)
{
    // load raw graphics data (no dimensions and flags)
    FILE* file_ptr;
    file_ptr = fopen(filename, "rb");
    fread(destination, 1, data_size, file_ptr);
    fclose(file_ptr);
}

void loadTexture(char* filename, int texture_index)
{
    FILE* file_ptr;
    file_ptr = fopen(filename, "rb");
    fread(&Textures[texture_index].width, 2, 1, file_ptr);
    fseek(file_ptr, 2, SEEK_SET);
    fread(&Textures[texture_index].height, 2, 1, file_ptr);
    fseek(file_ptr, 6, SEEK_SET);
    fread(&Textures[texture_index].transparent, 2, 1, file_ptr);
	fseek(file_ptr, 8, SEEK_SET);
    Textures[texture_index].pixels = malloc(Textures[texture_index].width * Textures[texture_index].height);
    fread(Textures[texture_index].pixels, 1, Textures[texture_index].width * Textures[texture_index].height, file_ptr);
    Textures[texture_index].offset_x = 0;
    Textures[texture_index].offset_y = 0;
    fclose(file_ptr);
}

void loadAllTextures()
{
    loadTexture("BRICKS.7UP", BRICKS);
    loadTexture("GRASS.7UP", GRASS);
    loadTexture("SAND.7UP", SAND);
    loadTexture("FLOOR1.7UP", FLOOR1);
    loadTexture("FLOOR2.7UP", FLOOR2);
    loadTexture("DUDE1.7UP", DUDE1);
    loadTexture("DUDE2.7UP", DUDE2);
    loadTexture("DUDE3.7UP", DUDE3);
    loadTexture("GRICKS.7UP", GRICKS);
    loadTexture("GLOOR1.7UP", GLOOR1);
    loadTexture("GLOOR2.7UP", GLOOR2);
}
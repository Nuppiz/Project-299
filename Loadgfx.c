#include "Common.h"
#include "Enums.h"
#include "Structs.h"

/* Graphics loading functions */

Texture_t Textures[NUM_TEXTURES];
extern Object_t Objects[NUM_OBJECTS];

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
    loadTexture("FLOOR1.7UP", FLOOR1);
    loadTexture("FLOOR2.7UP", FLOOR2);
    loadTexture("DUDE1.7UP", DUDE1);
    loadTexture("DUDE2.7UP", DUDE2);
    loadTexture("DUDE3.7UP", DUDE3);
    loadTexture("CAR.7UP", CAR);
}

void setTextures()
{
    Objects[0].orig_sprite = Textures[DUDE1];
    Objects[1].orig_sprite = Textures[DUDE2];
    Objects[2].orig_sprite = Textures[DUDE3];
}
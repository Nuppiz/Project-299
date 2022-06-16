#include "Common.h"
#include "Structs.h"

/* Graphics loading functions */

Texture_t Textures[NUM_TEXTURES];
Texture_t Tiles[NUM_TILES];
//extern Object_t* Objects;

void loadGfx(char* filename, uint8_t* destination, uint16_t data_size)
{
    // load raw graphics data (no dimensions and flags)
    FILE* file_ptr;
    file_ptr = fopen(filename, "rb");
    fread(destination, 1, data_size, file_ptr);
    fclose(file_ptr);
}

void loadTile(char* filename, int tile_index)
{
    // load static map tiles
    FILE* file_ptr;
    file_ptr = fopen(filename, "rb");
    fread(&Tiles[tile_index].width, 2, 1, file_ptr);
    fseek(file_ptr, 2, SEEK_SET);
    fread(&Tiles[tile_index].height, 2, 1, file_ptr);
	fseek(file_ptr, 8, SEEK_SET);
    Tiles[tile_index].pixels = malloc(Tiles[tile_index].width * Tiles[tile_index].height);
    fread(Tiles[tile_index].pixels, 1, Tiles[tile_index].width * Tiles[tile_index].height, file_ptr);
    Tiles[tile_index].offset_x = 0;
    Tiles[tile_index].offset_y = 0;
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
    loadTexture("SPRITES/DUDE1.7UP", DUDE1);
    loadTexture("SPRITES/DUDE2.7UP", DUDE2);
    loadTexture("SPRITES/DUDE3.7UP", DUDE3);
    loadTexture("SPRITES/CAR.7UP", CAR);
}

// Load all map tiles

void loadAllTiles()
{
    loadTile("TILES/DIRT.7UP", DIRT);
    loadTile("TILES/GRASS.7UP", GRASS);
    loadTile("TILES/GRAVEL.7UP", GRAVEL);
    loadTile("TILES/WATER.7UP", WATER);
    loadTile("TILES/BA_FLOOR.7UP", FLOOR_BATH);
    loadTile("TILES/WO_FLOOR.7UP", FLOOR_WOOD);
    loadTile("TILES/BRICKS.7UP", WALL_BRICKS);
    loadTile("TILES/CON_H.7UP", WALL_C_HOR);
    loadTile("TILES/CON_V.7UP", WALL_C_VER);
    loadTile("TILES/CON_C_LL.7UP", WALL_C_LL);
    loadTile("TILES/CON_C_LR.7UP", WALL_C_LR);
    loadTile("TILES/CON_C_UL.7UP", WALL_C_UL);
    loadTile("TILES/CON_C_UR.7UP", WALL_C_UR);
    loadTile("TILES/CON_I.7UP", WALL_C_INTER);
    loadTile("TILES/CON_T_D.7UP", WALL_C_T_DOWN);
    loadTile("TILES/CON_T_L.7UP", WALL_C_T_LEFT);
    loadTile("TILES/CON_T_R.7UP", WALL_C_T_RIGHT);
    loadTile("TILES/CON_T_U.7UP", WALL_C_T_UP);
    loadTile("TILES/DIRP_H.7UP", PATH_D_HOR);
    loadTile("TILES/DIRP_V.7UP", PATH_D_VER);
    loadTile("TILES/DIRP_UR.7UP", PATH_D_UR);
    loadTile("TILES/DIRP_UL.7UP", PATH_D_UL);
    loadTile("TILES/DIRP_DR.7UP", PATH_D_DR);
    loadTile("TILES/DIRP_DL.7UP", PATH_D_DL);
    loadTile("TILES/DIRP_I.7UP", PATH_D_INTER);
    loadTile("TILES/DIRP_TD.7UP", PATH_D_T_DOWN);
    loadTile("TILES/DIRP_TL.7UP", PATH_D_T_LEFT);
    loadTile("TILES/DIRP_TR.7UP", PATH_D_T_RIGHT);
    loadTile("TILES/DIRP_TU.7UP", PATH_D_T_UP);
    loadTile("TILES/BA_TOILD.7UP", BA_TOILETD);
    loadTile("TILES/BA_TOILL.7UP", BA_TOILETL);
    loadTile("TILES/BA_TOILR.7UP", BA_TOILETR);
    loadTile("TILES/BA_TOILU.7UP", BA_TOILETU);

}

/*void setTextures()
{
    Objects[0].sprite = Textures[DUDE1];
    Objects[1].sprite = Textures[DUDE2];
    Objects[2].sprite = Textures[DUDE3];
}*/
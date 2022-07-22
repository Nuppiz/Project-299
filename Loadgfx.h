#ifndef LOADGFX
#define LOADGFX

void loadGfx(char* filename, uint8_t* destination, uint16_t data_size);
int loadTileGfx(char* filename);
void loadTile(char* filename, int tile_index);
int loadTexture(char* filename);
void createErrorTexture();
void loadAllTextures();
void loadAllTiles();

#endif/* LOADGFX */

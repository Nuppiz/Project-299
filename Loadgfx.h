#ifndef LOADGFX
#define LOADGFX

void loadGfx(char* filename, uint8_t* destination, uint16_t data_size);
void loadTile(char* filename, int tile_index);
void loadTexture(char* filename, int texture_index);
void loadAllTextures();
void loadAllTiles();

#endif/* LOADGFX */

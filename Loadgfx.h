#ifndef LOADGFX
#define LOADGFX

void loadGfx(char* filename, uint8_t* destination, uint16_t data_size);
void loadTexture(char* filename, Texture_t* texture, int texture_index, int width, int height, int transparent);
void loadAllTextures();

#endif/* LOADGFX */

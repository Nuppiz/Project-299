#ifndef LOADGFX_H
#define LOADGFX_H

void loadGfx(char* filename, uint8_t* destination, uint16_t data_size);
void createErrorTextures();
int loadTexture(char* filename, Texture_array* array);
void loadBaseTextures();
void freeAllTextures();

#endif/* LOADGFX_H */

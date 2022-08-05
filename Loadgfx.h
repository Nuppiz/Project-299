#ifndef LOADGFX_H
#define LOADGFX_H

void loadGfx(char* filename, uint8_t* destination, uint16_t data_size);
void createErrorTexture();
int loadTexture(char* filename);
void freeAllTextures();

#endif/* LOADGFX_H */

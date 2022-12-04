#ifndef LOADGFX_H
#define LOADGFX_H

int findAnim(char* name);
void loadGfx(char* filename, uint8_t* destination, uint16_t data_size);
void createErrorTextures();
int loadTexture(char* filename, Texture_array* array);
void loadBaseTextures();
int loadAnimation(char* filename);
void loadAnimsFromList(char* list_filename);
void makeSprites();
void makeAnimset();
void freeAllTextures();

#endif/* LOADGFX_H */

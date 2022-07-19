#ifndef DRAW_H
#define DRAW_H

#include "Structs.h"

void drawTexture(int x, int y, Texture_t* texture);
void createParticle(Vec2 pos, uint8_t);
void titleDraw();
void gameDraw();
void pauseDraw();

#endif/* DRAW_H */

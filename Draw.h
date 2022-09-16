#ifndef DRAW_H
#define DRAW_H

#include "Structs.h"

void drawTexture(int x, int y, Texture_t* texture);
void particleFx(Vec2 pos, Vec2 dir, uint8_t fx_type);
void spawnCorpse(Vec2 pos, double angle, int8_t life);
void emptyCorpseArray();
void titleDraw();
void menuDraw();
void gameDraw();
void pauseDraw();

#endif/* DRAW_H */

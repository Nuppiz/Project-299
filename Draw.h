#ifndef DRAW_H
#define DRAW_H

#include "Structs.h"

int boundaryCheck(int x, int y);
void drawTexture(int x, int y, Texture_t* texture);
void drawRectangle(int x, int y, int w, int h, uint8_t color);
void particleFx(Vec2 pos, Vec2 dir, uint8_t fx_type);
void spawnCorpse(Vec2 pos, double angle, int8_t life);
void emptyCorpseArray();
int spawnTempSprite(uint8_t draw_type, uint8_t move_type, Vec2 pos, Vec2 vel, double angle, Sprite_t* sprite);
void updateTempSprite(int index, Vec2 pos, Vec2 vel, double angle);
void deleteTempSprite(int index);
void titleDraw();
void menuDraw();
void gameDraw();
void pauseDraw();
void ingameMenuDraw();

#endif/* DRAW_H */

#ifndef GAME_H
#define GAME_H

#include "Structs.h"

id_t createObject(float x, float y, double angle, int radius, uint8_t control, uint8_t ai_mode, id_t ai_target, id_t sprite);
void deleteObject(id_t id);
void deleteLastObject();
void initGame();

#endif/* GAME_H */

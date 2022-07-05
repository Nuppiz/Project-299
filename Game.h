#ifndef GAME_H
#define GAME_H

#include "Structs.h"

id_t createObject(float x, float y, double angle, int radius, uint8_t control, uint8_t ai_mode, int ai_timer, id_t ai_target, id_t sprite_id);
void deleteObject(id_t id);
void deleteLastObject();

#endif/* GAME_H */

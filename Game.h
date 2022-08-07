#ifndef GAME_H
#define GAME_H

#include "Structs.h"

id_t createObject(float x, float y, double angle, int radius, uint8_t control, uint8_t ai_mode, int ai_timer, id_t ai_target, int health, int8_t trigger_on_death, char* texture_name);
void deleteObject(id_t id);
void deleteLastObject();
void initGameData(int levelname_length);
void freeGameData();

#endif/* GAME_H */

#ifndef GAME_H
#define GAME_H

#include "Structs.h"

id_t getNewId();
void initActorTemplates();
id_t createActorFromTemplate(char* template_name, float x, float y, double angle, uint8_t control, uint8_t ai_mode, int ai_timer, id_t ai_target, id_t trigger_on_death);
id_t createActor(float x, float y, double angle, int radius, uint8_t control, uint8_t ai_mode, int ai_timer, id_t ai_target, int health, id_t trigger_on_death, id_t primary_weapon, char* texture_name);
void deleteActor(id_t id);
void deleteLastActor();
void initGameData(id_t actor_capacity, id_t id_capacity);
void freeGameData();
void testInitPlayerAnim();

#endif/* GAME_H */

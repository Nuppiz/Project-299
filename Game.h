#ifndef GAME_H
#define GAME_H

#include "Structs.h"

id_t getNewId();
void initActorTemplates();
int loadActorTemplate(char* filename);
id_t createActorFromTemplate(Actor_t actor, ActorTemplate_t* template);
id_t createActor(Actor_t actor, char* texture_name);
void deleteActor(id_t id);
void deleteLastActor();
void initGameData(id_t actor_capacity, id_t id_capacity);
void freeGameData();
void testInitPlayerAnim();

#endif/* GAME_H */

#ifndef ACTION_H
#define ACTION_H

#include "Structs.h"

int checkForHit(Vec2 projectile, Vec2 target, int radius);
void shootWeapon(Actor_t* source);
void useTile(Vec2 pos, Vec2 dir);
void entityLoop();
void actorLoop();

#endif /* ACTION_H */

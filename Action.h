#ifndef ACTION_H
#define ACTION_H

#include "Structs.h"

void checkForInteractive();
void checkForItem();
Tile_t* getEntityTile(Entity_t* entity);
int checkForHit(Vec2 projectile, Vec2 target, int radius);
void shootWeapon(Weapon_t* weapon, Actor_t* source);
void useTile(Vec2 pos, Vec2 dir);
void moveProjectiles();
void entityLoop();
void actorDeathLoop();

#endif /* ACTION_H */

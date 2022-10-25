#ifndef MOVECOLL_H
#define MOVECOLL_H

#include "Structs.h"

void updateGridLoc(Actor_t* actor);
int getTileCollision(Vec2 pos);
int getTileBulletBlock(Vec2 pos);
int checkForPortal(Vec2_int grid_location);
Vec2 moveFromPortal(Vec2 pos);
//void moveAllActors();
//void collideAllActors();
void physics();

#endif/* MOVECOLL_H */

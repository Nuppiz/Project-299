#ifndef MOVECOLL_H
#define MOVECOLL_H

#include "Structs.h"

void updateGridLoc(Object_t* obj);
int getTileCollision(Vec2 pos);
int getTileBulletBlock(Vec2 pos);
int checkForPortal(Vec2_int grid_location);
Vec2 moveFromPortal(Vec2 pos);
//void moveAllObjects();
//void collideAllObjects();
void physics();

#endif/* MOVECOLL_H */

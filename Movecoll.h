#ifndef MOVECOLL_H
#define MOVECOLL_H

#include "Structs.h"

//void updateGridLoc(Object_t* obj);
//void calcCameraOffset(Object_t* target);
int getTileCollision(Vec2 pos);
int getTileBulletBlock(Vec2 pos);
//void moveAllObjects();
//void collideAllObjects();
void physics();

#endif/* MOVECOLL_H */

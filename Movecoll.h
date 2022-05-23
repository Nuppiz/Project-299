#ifndef MOVECOLL_H
#define MOVECOLL_H

#include "Structs.h"

void checkGridLoc(Object* obj);
void calcCameraOffset(Object* target);
int tileDetectColor(Vec2 pos, Map* map);
void calculateMovements(Map* map);
void collision(Map* map);

#endif /* MOVECOLL_H */

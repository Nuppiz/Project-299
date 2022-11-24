#ifndef MOVECOLL_H
#define MOVECOLL_H

#include "Structs.h"

Vec2_int getGridLocation(Vec2 pos);
void updateGridLoc(Actor_t* actor);
Tile_t* getTileAt(Vec2_int grid_location);
int getEntityTypeAt(Vec2_int grid_location);
Vec2 forceMove(Vec2 pos);
//void moveAllActors();
//void collideAllActors();
void physics();

#endif/* MOVECOLL_H */

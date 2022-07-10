#include "Common.h"
#include "Structs.h"
#include "Movecoll.h"
#include "Sound.h"
#include "Vectors.h"

/* Various actions between the player and other objects */

extern GameData_t Game;

int checkForHit(Vec2 projectile, Vec2 target, int radius)
{
    Vec2 collision_box;

    collision_box.x = target.x - radius;
    collision_box.y = target.y - radius;
    if (projectile.x >= collision_box.x && projectile.y >= collision_box.y)
        return TRUE;
    
    collision_box.x = target.x + radius;
    collision_box.y = target.y - radius;
    if (projectile.x <= collision_box.x && projectile.y >= collision_box.y)
        return TRUE;

    collision_box.x = target.x + radius;
    collision_box.y = target.y + radius;
    if (projectile.x <= collision_box.x && projectile.y <= collision_box.y)
        return TRUE;

    collision_box.x = target.x - radius;
    collision_box.y = target.y + radius;
    if (projectile.x >= collision_box.x && projectile.y <= collision_box.y)
        return TRUE;

    else
        return FALSE;
}

int bulletCollision(Object_t* source, Object_t* target)
{
    //Vec2 s_to_t = getVec2(source->position, target->position);
    Vec2 bullet_loc;
    int bulletpath;
    //float angle = getVec2Angle2(s_to_t, source->direction);

    bullet_loc.x = source->position.x;
    bullet_loc.y = source->position.y;

    for (bulletpath = 0; bulletpath < BULLET_MAX_DISTANCE; bulletpath += BULLET_STEP)
    {
        bullet_loc.x += cos(source->angle) * BULLET_STEP;
        bullet_loc.y += sin(source->angle) * BULLET_STEP;

        if (getTileType(bullet_loc) == WALL)
            return FALSE;
        else if (checkForHit(bullet_loc, target->position, target->radius) == TRUE)
            return TRUE;
    }
    return FALSE;
}

void shootWeapon()
{
    int i = 1;

    playSounds(SOUND_SHOOT);

    while (i < Game.object_count)
    {
        if (bulletCollision(&Game.Objects[0], &Game.Objects[i]) == TRUE)
            playSounds(SOUND_AARGH);

        i++;
    }
}
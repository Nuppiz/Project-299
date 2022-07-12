#include "Common.h"
#include "Structs.h"
#include "Movecoll.h"
#include "Sound.h"
#include "Vectors.h"
#include "Text.h"

/* Various actions between the player and other objects */

extern GameData_t Game;
extern Vec2 camera_offset;

int checkForHit(Vec2 projectile, Vec2 target, int radius)
{
    int collision_left, collision_right, collision_top, collision_bottom;

    target.x -= camera_offset.x;
    target.y -= camera_offset.y;

    collision_left = target.x - radius;
    collision_right = target.x + radius;
    collision_top = target.y - radius;
    collision_bottom = target.y + radius;

    if (projectile.x >= collision_left && projectile.x <= collision_right
    && projectile.y >= collision_top && projectile.y <= collision_bottom)
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

    for (bulletpath = 0; bulletpath < BULLET_MAX_DISTANCE; bulletpath++)
    {
        bullet_loc.x += source->direction.x;
        bullet_loc.y += source->direction.y;

        if (getTileType(bullet_loc) == WALL)
        {
            //playSounds(SOUND_EXPLO);
            return FALSE;
        }
        else if (checkForHit(bullet_loc, target->position, target->radius) == TRUE)
            return TRUE;
    }
    return FALSE;
}

void shootWeapon()
{
    int i = 1;

    playSounds(SOUND_SHOOT);

    while (i < Game.object_count - 1)
    {
        if (bulletCollision(&Game.Objects[0], &Game.Objects[i]) == TRUE)
        {
            playSounds(SOUND_AARGH);
            sprintf(debug[DEBUG_SHOOT], "LAST HIT: %d", i);
        }

        i++;
    }
}
#include "Common.h"
#include "Structs.h"
#include "Movecoll.h"
#include "Sound.h"
#include "Vectors.h"
#include "Text.h"
#include "Game.h"
#include "Draw.h"
#include "Video.h" // temp

/* Various actions between the player and other objects */

extern GameData_t Game;
extern Vec2 camera_offset;

int checkForHit(Vec2 projectile, Vec2 target, int radius)
{
    int collision_left, collision_right, collision_top, collision_bottom;

    //target.x -= camera_offset.x;
    //target.y -= camera_offset.y;

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

void bulletTrace(int source_id, Vec2 pos, Vec2 dir, int max_range)
{
    int bulletpath, i, b;

    for (bulletpath = 0; bulletpath < max_range; bulletpath += BULLET_STEP)
    {
        pos.x += dir.x * BULLET_STEP;
        pos.y += dir.y * BULLET_STEP;

        if (getTileType(pos) == WALL)
        {
            //playSounds(SOUND_EXPLO);
            createParticle(pos, 48);
            break;
        }
        else
        {
            for (i = 0; i < Game.object_count; i++)
            {
                if (Game.Objects[i].id != source_id && checkForHit(pos, Game.Objects[i].position, Game.Objects[i].radius) == TRUE)
                {
                    playSounds(SOUND_AARGH);
                    for (b = 0; b < 20; b++)
                    {
                        createParticle(pos, 176);
                    }
                    deleteObject(Game.Objects[i].id);
                    sprintf(debug[DEBUG_SHOOT], "LAST HIT: %d", i);
                }
                //SET_PIXEL_VGA(((int)(pos.x - camera_offset.x)), ((int)(pos.y - camera_offset.y)), COLOUR_WHITE);
            }
        }
    }
    createParticle(pos, 32);
}

void shootWeapon(Object_t* source)
{
    Vec2 bullet_loc, direction;
    double angle;
    int i;

    playSounds(SOUND_SHOOT);

    bullet_loc.x = source->position.x + direction.x * (source->radius * 1.5);
    bullet_loc.y = source->position.y + direction.y * (source->radius * 1.5);

    for (i = 1; i < 4; i++)
    {
        angle = source->angle + ((rand() % 20 - 10) * RAD_1);
        direction = getDirVec2(angle);
        bulletTrace(source->id, bullet_loc, direction, BULLET_MAX_DISTANCE + (rand() % 20 -10));
    }
}
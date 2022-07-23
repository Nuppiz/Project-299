#include "Common.h"
#include "Structs.h"
#include "Movecoll.h"
#include "Sound.h"
#include "Vectors.h"
#include "Text.h"
#include "Game.h"
#include "Draw.h"

/* Various actions between the player and other objects */

extern GameData_t Game;
extern Vec2 camera_offset;
Entity_t Entities[32];

int checkForHit(Vec2 projectile, Vec2 target, int radius)
{
    int collision_left, collision_right, collision_top, collision_bottom;

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
    int bulletpath, i;

    for (bulletpath = 0; bulletpath < max_range; bulletpath += BULLET_STEP)
    {
        pos.x += dir.x * BULLET_STEP;
        pos.y += dir.y * BULLET_STEP;

        if (getTileBulletBlock(pos) == TRUE)
        {
            particleFx(pos, dir, FX_SPARKS);
            break;
        }
        else
        {
            for (i = 0; i < Game.object_count; i++)
            {
                if (Game.Objects[i].id != source_id && checkForHit(pos, Game.Objects[i].position, Game.Objects[i].radius) == TRUE)
                {
                    //playSounds(SOUND_AARGH);
                    particleFx(pos, dir, FX_BLOOD);
                    sprintf(debug[DEBUG_SHOOT], "LAST HIT: %d", i);
                }
            }
        }
    }
    particleFx(pos, dir, FX_DIRT);
}

void shootWeapon(Object_t* source)
{
    Vec2 bullet_loc, direction;
    double angle;
    int i;

    playSounds(SOUND_SHOOT);
    particleFx(source->position, source->direction, FX_WATERGUN);

    bullet_loc.x = source->position.x + direction.x * (source->radius * 1.5);
    bullet_loc.y = source->position.y + direction.y * (source->radius * 1.5);

    for (i = 1; i < 4; i++)
    {
        angle = source->angle + ((rand() % 20 - 10) * RAD_1);
        direction = getDirVec2(angle);
        bulletTrace(source->id, bullet_loc, direction, BULLET_MAX_DISTANCE + (rand() % 20 -10));
    }
}

Tile_t* getEntityTile(int x, int y)
{
    Tile_t* tile_location = &Game.Map.tilemap[y * Game.Map.width + x];

    return tile_location;
}

void useDoor(Entity_t* ent)
{

}

void useTile(Vec2 pos, Vec2 dir)
{
    Vec2_int target;
    Tile_t* tile;
    uint8_t tile_x, tile_y;
    target.x = pos.x * dir.x * 2;
    target.y = pos.y * dir.y * 2;

    tile_x = target.x / SQUARE_SIZE;
    tile_y = target.y / SQUARE_SIZE;

    if ((tile = &Game.Map.tilemap[tile_y * Game.Map.width + tile_x])->is_entity == TRUE)
    {
        Entity_t* ent = &Entities[tile->entity_value];
        playSounds(SOUND_EXPLO);
        switch (ent->type)
        {
        case ENT_DOOR:
            // useDoor(uint8 ent_id)
            // the below should become useDoor function
            // then you need to find its tile some oher way
        // we would need that anyway,
        // for when we don't yet know the entity's location (e.g. a door used by a switch elsewhere)
        // need a getEntityTile function that uses entity's x y to look up TileMap[] and return Tile pointer
            if (ent->data.door.locked == 0)
            {
                ent->state ^= 1;
                tile->texture_id = (ent->state) == 1 ? tile->texture_id+1 : tile->texture_id-1;
            }
            //else
                //playSound(DOOR_LOCKED_CLICKETY_CLACK_WONT_BUDGE)
            break;
        case ENT_BUTTON:
            // bla bla
            // ent = our button entity
            // useDoor(ent->data.button.target_id, use_mode) // usemode USE_DIRECTLY, USE_VIA_BUTTON
            // if used via button, bypass "locked" variable;
            // otherwise check for locked, and make noise when you fail to budge door.
        }

    }
}
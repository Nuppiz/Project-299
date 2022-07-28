#include "Common.h"
#include "Structs.h"
#include "Movecoll.h"
#include "Sound.h"
#include "Vectors.h"
#include "Text.h"
#include "Game.h"
#include "Draw.h"

/* Various actions between the player and other entities/actors */

extern GameData_t Game;
Entity_t Entities[32];
uint8_t key_acquired = 0;

 // tbd
void entityInit()
{
    int i;

    
}

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

void useDoor(Entity_t* door, uint8_t use_mode)
{
    Tile_t* tile = getEntityTile(door->x, door->y);

    if (use_mode == USE_VIA_BUTTON)
    {
        door->state ^= 1;
        door->data.door.locked ^= 1;
        tile->obstacle ^= 1;
        tile->block_bullets ^= 1;
        tile->texture_id = (door->state) == 1 ? tile->texture_id + 1 : tile->texture_id - 1;
    }
    else if (door->data.door.locked == TRUE && use_mode == USE_DIRECTLY & key_acquired == TRUE)
    {
        playSounds(SOUND_DOOR_O);
        door->state ^= 1;
        door->data.door.locked ^= 1;
        tile->obstacle ^= 1;
        tile->block_bullets ^= 1;
        tile->texture_id = (door->state) == 1 ? tile->texture_id + 1 : tile->texture_id - 1;
    }
    else if (door->data.door.locked == TRUE && use_mode == USE_DIRECTLY)
        playSounds(SOUND_LOCKED);
    else if (door->state == 0)
    {
        playSounds(SOUND_DOOR_C);
        door->state ^= 1;
        tile->obstacle ^= 1;
        tile->block_bullets ^= 1;
        tile->texture_id = (door->state) == 1 ? tile->texture_id + 1 : tile->texture_id - 1;
    }
    else if (door->state == 1)
    {
        playSounds(SOUND_DOOR_O);
        door->state ^= 1;
        tile->obstacle ^= 1;
        tile->block_bullets ^= 1;
        tile->texture_id = (door->state) == 1 ? tile->texture_id + 1 : tile->texture_id - 1;
    }
}

void useTile(Vec2 pos, Vec2 dir)
{
    Vec2_int target;
    Tile_t* tile;
    uint8_t tile_x, tile_y;
    target.x = pos.x + dir.x * 30;
    target.y = pos.y + dir.y * 30;

    tile_x = target.x / SQUARE_SIZE;
    tile_y = target.y / SQUARE_SIZE;

    if ((tile = &Game.Map.tilemap[tile_y * Game.Map.width + tile_x])->is_entity == TRUE)
    {
        Entity_t* ent = &Entities[tile->entity_value];
        switch (ent->type)
        {
        case ENT_DOOR:
            useDoor(ent, USE_DIRECTLY);
            break;
        case ENT_BUTTON:
            playSounds(SOUND_SWITCH);
            ent->state ^= 1;
            tile->texture_id = (ent->state) == 1 ? tile->texture_id + 1 : tile->texture_id - 1;
            useDoor(&Entities[ent->data.button.target], USE_VIA_BUTTON);
            break;
        }
    }
}
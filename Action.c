#include "Common.h"
#include "Structs.h"
#include "Movecoll.h"
#include "Sound.h"
#include "Vectors.h"
#include "Text.h"
#include "Game.h"
#include "Draw.h"
#include "LvlLoad.h"

/* Various actions between the player and other entities/actors */

extern System_t System;
extern GameData_t Game;
extern Entity_t Entities[];
uint8_t key_acquired = 0; // replace later with proper inventory system

void checkForInteractive() // temporary, will be replaced with better system later
{
    int player_tilemap_loc;
    static time_t last_sfx_played = 0;

    player_tilemap_loc = Game.Objects[0].grid_loc.y * Game.Map.width + Game.Objects[0].grid_loc.x;

    if (Game.Map.tilemap[player_tilemap_loc].is_entity == 0)
    {
        if (Game.Map.tilemap[player_tilemap_loc].entity_value == TILE_KEY_RED)
        {
            key_acquired = TRUE;
            Game.Map.tilemap[player_tilemap_loc].entity_value = 0;
            playSounds(SOUND_ITEM);
        }
        else if (Game.Map.tilemap[player_tilemap_loc].entity_value == TILE_SPIKES)
        {
            if (last_sfx_played + SFX_INTERVAL < System.ticks)
            {
                last_sfx_played = System.ticks;
                playSounds(SOUND_HURT);
            }
        }
    }
}

void deleteEntity(int entity_id, int tilemap_loc)
{
    Game.Map.tilemap[tilemap_loc].is_entity = 0;
    Game.Map.tilemap[tilemap_loc].entity_value = 0;
    Entities[entity_id].type = ENT_DELETED;
}

Tile_t* getEntityTile(int x, int y)
{
    Tile_t* tile_location = &Game.Map.tilemap[y * Game.Map.width + x];

    return tile_location;
}

void runSpawner(Entity_t* spawner)
{
    Vec2 direction = getDirVec2(spawner->data.spawner.angle);
    int tilemap_loc;

    if (spawner->state == 1)
    {
        if (spawner->data.spawner.last_spawn_time + spawner->data.spawner.spawn_time_interval < System.ticks)
        {
            spawner->data.spawner.last_spawn_time = System.ticks;
            if (spawner->data.spawner.num_objects < spawner->data.spawner.max_objects || spawner->data.spawner.max_objects == -1)
            {
                createObject(spawner->x * SQUARE_SIZE + direction.x * (rand() % 50), spawner->y * SQUARE_SIZE + direction.y * (rand() % 50), spawner->data.spawner.angle,
                7, 0, 1, 0, Game.player_id, spawner->data.spawner.trigger_on_death, "SPRITES/DUDE2.7UP");
                spawner->data.spawner.num_objects++;
            }
            if (spawner->data.spawner.num_objects >= spawner->data.spawner.max_objects && spawner->data.spawner.max_objects != -1)
            {
                spawner->state = 0;
                if (spawner->data.spawner.only_once == 1)
                {
                    tilemap_loc = spawner->y * Game.Map.width + spawner->x;
                    deleteEntity(Game.Map.tilemap[tilemap_loc].entity_value, tilemap_loc);
                }
            }
        }
    }
}

void runTrigger(Entity_t* trigger)
{
    int i, tilemap_loc;

    if (Game.Objects[0].grid_loc.x == trigger->x && Game.Objects[0].grid_loc.y == trigger->y && trigger->state == 0)
    {
        trigger->data.trigger.last_trigger_time = System.ticks;
        playSounds(SOUND_DOOR_O);
        trigger->state = 1;
        for (i = 0; i < 4; i++)
        {
            if (trigger->data.trigger.target_ids[i] != -1 && Entities[trigger->data.trigger.target_ids[i]].type != ENT_COUNTER)
                Entities[trigger->data.trigger.target_ids[i]].state ^= 1;
            else
                Entities[trigger->data.trigger.target_ids[i]].data.counter.value++;
        }
        if (trigger->data.trigger.only_once == 1)
        {
            tilemap_loc = trigger->y * Game.Map.width + trigger->x;
            deleteEntity(Game.Map.tilemap[tilemap_loc].entity_value, tilemap_loc);
        }
    }
    if (trigger->data.trigger.last_trigger_time + trigger->data.trigger.trigger_interval < System.ticks && trigger->state == 1)
        trigger->state = 0;
}

void toggleDoor(Entity_t* door)
{
    Tile_t* tile = getEntityTile(door->x, door->y);

    door->state ^= 1;
    tile->obstacle ^= 1;
    tile->block_bullets ^= 1;
    tile->texture_id = (door->state) == 1 ? tile->texture_id + 1 : tile->texture_id - 1;
}

void useDoor(Entity_t* door, uint8_t use_mode)
{
    if (use_mode == USE_VIA_BUTTON)
    {
        toggleDoor(door);
    }
    else if (door->data.door.locked == TRUE && use_mode == USE_DIRECTLY & key_acquired == TRUE)
    {
        playSounds(SOUND_DOOR_O);
        door->data.door.locked ^= 1;
        toggleDoor(door);
    }
    else if (door->data.door.locked == TRUE && use_mode == USE_DIRECTLY)
        playSounds(SOUND_LOCKED);
    else if (door->data.door.locked == FALSE && door->state == 0)
    {
        playSounds(SOUND_DOOR_C);
        toggleDoor(door);
    }
    else if (door->data.door.locked == FALSE && door->state == 1)
    {
        playSounds(SOUND_DOOR_O);
        toggleDoor(door);
    }
}

void toggleButton(Entity_t* button)
{
    Tile_t* tile = getEntityTile(button->x, button->y);

    button->state ^= 1;
    tile->texture_id = (button->state) == 1 ? tile->texture_id + 1 : tile->texture_id - 1;
    if (Entities[button->data.button.target].type == ENT_DOOR)
        useDoor(&Entities[button->data.button.target], USE_VIA_BUTTON);
    else if (Entities[button->data.button.target].type == ENT_SPAWNER && Entities[button->data.button.target].data.spawner.toggleable == TRUE)
    {
        Entities[button->data.button.target].state ^= 1;
    }
}

void toggleCounter(Entity_t* counter)
{
    switch (Entities[counter->data.counter.target_id].type)
    {
    case ENT_DOOR: toggleDoor(&Entities[counter->data.counter.target_id]); break;
    case ENT_BUTTON: toggleButton(&Entities[counter->data.counter.target_id]); break;
    }
}

void runCounter(Entity_t* counter)
{
    if (counter->data.counter.value == counter->data.counter.max_value)
    {
        toggleCounter(counter);
        if (counter->data.counter.only_once == 1)
            counter->type = ENT_DELETED;
        else
            counter->data.counter.value = 0;
    }
}

void runPortal(Entity_t* portal)
{
    if (Game.Objects[0].grid_loc.x == portal->x && Game.Objects[0].grid_loc.y == portal->y && portal->state == 1)
    {
        playSounds(SOUND_PORTAL);
        /*levelLoader(portal->data.portal.level_name, LOAD_PORTAL_LEVEL);*/
        Game.Objects[0].position.x = portal->data.portal.x;
        Game.Objects[0].position.y = portal->data.portal.y;
        Game.Objects[0].angle = portal->data.portal.angle;
    }
}

void deathTrigger(int object_id)
{
    switch (Entities[Game.Objects[object_id].trigger_on_death].type)
    {
    case ENT_DOOR: toggleDoor(&Entities[Game.Objects[object_id].trigger_on_death]); break;
    case ENT_BUTTON: toggleButton(&Entities[Game.Objects[object_id].trigger_on_death]); break;
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
        case ENT_DOOR: useDoor(ent, USE_DIRECTLY);
            break;
        case ENT_BUTTON: playSounds(SOUND_SWITCH), toggleButton(ent);
            break;
        }
    }
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
                    particleFx(pos, dir, FX_BLOOD);
                    sprintf(debug[DEBUG_SHOOT], "LAST HIT: %d", i);
                    if (Game.Objects[i].trigger_on_death != -1)
                    {
                        deathTrigger(i);
                    }
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
    //particleFx(source->position, source->direction, FX_WATERGUN);

    bullet_loc.x = source->position.x + direction.x * (source->radius * 1.5);
    bullet_loc.y = source->position.y + direction.y * (source->radius * 1.5);

    for (i = 1; i < 4; i++)
    {
        angle = source->angle + ((rand() % 20 - 10) * RAD_1);
        direction = getDirVec2(angle);
        bulletTrace(source->id, bullet_loc, direction, BULLET_MAX_DISTANCE + (rand() % 20 -10));
    }
}

void entityLoop()
{
    int i;

    for (i = 0; i < MAX_ENTITIES; i++)
    {
        if (Entities[i].type != ENT_DELETED)
        {
            if (Entities[i].type == ENT_SPAWNER)
                runSpawner(&Entities[i]);
            else if (Entities[i].type == ENT_TRIGGER)
                runTrigger(&Entities[i]);
            else if (Entities[i].type == ENT_COUNTER)
                runCounter(&Entities[i]);
            else if (Entities[i].type == ENT_PORTAL)
                runPortal(&Entities[i]);
        }
    }
}
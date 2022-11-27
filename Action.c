#include "Common.h"
#include "Structs.h"
#include "Movecoll.h"
#include "Sound.h"
#include "Vectors.h"
#include "Game.h"
#include "Draw.h"
#include "LvlLoad.h"
#include "Filech.h"

/* Various actions between the player and other Entities/actors */

extern System_t System;
extern Timer_t Timers;
extern GameData_t Game;
extern Entity_t Entities[];
extern Item_t* Items;
extern Weapon_t Weapons[];
WeaponEffect_t Effects[NUM_EFFECTS] = {SOUND_EXPLOSION};
Projectile_t Projectiles[64] = {0};

uint8_t key_acquired = 0; // replace later with proper inventory system

void checkForItem() // might be replaced with better system later
{
    int tilemap_loc, i, a;

    for (i = 0; i < Game.actor_count; i++)
    {
        Actor_t* actor = &Game.Actors[i];
        tilemap_loc = actor->grid_loc.y * Game.Map.width + actor->grid_loc.x;
        for (a = 0; a < Game.item_count; a++)
        {
            Item_t* item = &Items[a];
            if (tilemap_loc == item->index && item->state == 1)
            {
                if (item->type == ITEM_KEY_RED && actor->id == Game.player_id)
                {
                    key_acquired = TRUE;
                    item->state = 0;
                    playSFX(SOUND_ITEM);
                }
            }
        }
    }
}

void checkForInteractive() // temporary, will be replaced with better system later
{
    int i;
    static time_t last_env_damage = 0;
    Actor_t* actor;
    Tile_t* tile;

    for (i = 0; i < Game.actor_count; i++)
    {
        actor = &Game.Actors[i];
        tile = getTileAt(actor->grid_loc);

        if (tile->is_entity == 0)
        {
            if (tile->entity_value == TILE_DMG_10)
            {
                if (last_env_damage + HURT_INTERVAL < System.ticks)
                {
                    last_env_damage = System.ticks;

                    if (actor->id == Game.player_id)
                        playSFX(SOUND_HURT);
                    else
                        playSFX(SOUND_HURT_E);

                    actor->health -= 10;

                    #if DEBUG == 1
                    sprintf(debug[DEBUG_ENTITIES], "TARGET HP: %d", actor->health);
                    #endif
                }
            }
        }
    }
}

Tile_t* getTileAtXY(int x, int y)
{
    Tile_t* tile_location = &Game.Map.tilemap[y * Game.Map.width + x];

    return tile_location;
}

Tile_t* getEntityTile(Entity_t* entity)
{
    return &Game.Map.tilemap[entity->y * Game.Map.width + entity->x];
}

void deleteEntity(Entity_t* entity)
{
    Tile_t* entity_tile = getEntityTile(entity);
    entity_tile->is_entity = 0;
    entity_tile->entity_value = 0;
    Entities[entity->id].type = ENT_NONE;
}

void runSpawner(Entity_t* entity)
{
    Vec2 direction = getDirVec2(entity->data.spawner.angle);
    int tilemap_loc;

    if (entity->state == 1)
    {
        struct t_spawner* spawner = &entity->data.spawner;

        if (spawner->last_spawn_time + spawner->spawn_time_interval < System.ticks)
        {
            spawner->last_spawn_time = System.ticks;
            if (spawner->num_actors < spawner->max_actors || spawner->max_actors == -1)
            {
                createActor(entity->x * SQUARE_SIZE + direction.x * (rand() % 50), entity->y * SQUARE_SIZE + direction.y * (rand() % 50), spawner->angle,
                7, 0, 1, 0, Game.player_id, 100, spawner->trigger_on_death, 20, "SPRITES/DUDE2.7UP");
                spawner->num_actors++;
            }
            if (spawner->num_actors >= spawner->max_actors && spawner->max_actors != -1)
            {
                entity->state = 0;
                if (spawner->only_once == 1)
                {
                    deleteEntity(entity);
                }
            }
        }
    }
}

void runTrigger(Entity_t* entity)
{
    int i;
    struct t_trigger* trigger = &entity->data.trigger;

    if (PLAYER_ACTOR.grid_loc.x == entity->x && PLAYER_ACTOR.grid_loc.y == entity->y && entity->state == 0)
    {

        trigger->last_trigger_time = System.ticks;
        playSFX(SOUND_DOOR_O);
        entity->state = 1;
        for (i = 0; i < 4; i++)
        {
            if (trigger->target_ids[i] != -1 && Entities[trigger->target_ids[i]].type != ENT_COUNTER)
                Entities[trigger->target_ids[i]].state ^= 1;
            else
                Entities[trigger->target_ids[i]].data.counter.value++;
        }
        if (trigger->only_once == 1)
        {
            deleteEntity(entity);
        }
    }
    if (trigger->last_trigger_time + trigger->trigger_interval < System.ticks && entity->state == 1)
        entity->state = 0;
}

void toggleDoor(Entity_t* door)
{
    Tile_t* tile = getTileAtXY(door->x, door->y);

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
        playSFX(SOUND_DOOR_O);
        door->data.door.locked ^= 1;
        toggleDoor(door);
    }
    else if (door->data.door.locked == TRUE && use_mode == USE_DIRECTLY)
        playSFX(SOUND_LOCKED);
    else if (door->data.door.locked == FALSE && door->state == 1)
    {
        playSFX(SOUND_DOOR_C);
        toggleDoor(door);
    }
    else if (door->data.door.locked == FALSE && door->state == 0)
    {
        playSFX(SOUND_DOOR_O);
        toggleDoor(door);
    }
}

void toggleButton(Entity_t* button)
{
    Tile_t* tile = getTileAtXY(button->x, button->y);

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
    Entity_t* target = &Entities[counter->data.counter.target_id];
    switch (target->type)
    {
    case ENT_DOOR: toggleDoor(target); break;
    case ENT_BUTTON: toggleButton(target); break;
    }
}

void runCounter(Entity_t* entity)
{
    struct t_counter* counter = &entity->data.counter;

    if (counter->value == counter->max_value)
    {
        toggleCounter(entity);
        if (counter->only_once == 1)
            entity->type = ENT_NONE;
        else
            counter->value = 0;
    }
}

void usePortal(Entity_t* entity)
{
    uint16_t portal_x, portal_y;
    double portal_angle;
    char levelpath[30] = LEVEL_PATH;
    struct t_portal* portal = &entity->data.portal;

    if (PLAYER_ACTOR.grid_loc.x == entity->x && PLAYER_ACTOR.grid_loc.y == entity->y && entity->state == 1)
    {
        playSFX(SOUND_PORTAL);
        if (portal->level_name != NULL)
        {
            strcat(levelpath, portal->level_name);
            if (checkFileExists(levelpath) == FALSE)
                return;
            else
            {
                portal_x = portal->x;
                portal_y = portal->y;
                portal_angle = portal->angle;
                levelTransition(Game.current_level_name, portal->level_name);
                PLAYER_ACTOR.velocity.x = 0.0;
                PLAYER_ACTOR.velocity.y = 0.0;
                PLAYER_ACTOR.position.x = portal_x;
                PLAYER_ACTOR.position.y = portal_y;
                PLAYER_ACTOR.angle = portal_angle;
                updateGridLoc(&PLAYER_ACTOR);
                saveGameState("AUTO/");
            }
        }
        else
        {
            PLAYER_ACTOR.position.x = portal->x;
            PLAYER_ACTOR.position.y = portal->y;
            PLAYER_ACTOR.angle = portal->angle;
        }
    }
}

void deathTrigger(int actor_id)
{
    switch (Entities[Game.Actors[actor_id].trigger_on_death].type)
    {
    case ENT_DOOR: toggleDoor(&Entities[Game.Actors[actor_id].trigger_on_death]); break;
    case ENT_BUTTON: toggleButton(&Entities[Game.Actors[actor_id].trigger_on_death]); break;
    }
}

void useTile(Vec2 pos, Vec2 dir)
{
    Vec2_int target;
    Tile_t* tile;
    uint8_t tile_x, tile_y;
    target.x = pos.x + dir.x * 20;
    target.y = pos.y + dir.y * 20;

    tile_x = target.x / SQUARE_SIZE;
    tile_y = target.y / SQUARE_SIZE;

    if ((tile = &Game.Map.tilemap[tile_y * Game.Map.width + tile_x])->is_entity == TRUE)
    {
        Entity_t* ent = &Entities[tile->entity_value];
        switch (ent->type)
        {
        case ENT_DOOR: useDoor(ent, USE_DIRECTLY);
            break;
        case ENT_BUTTON: playSFX(SOUND_SWITCH), toggleButton(ent);
            break;
        }
    }
}

int checkForHit(Vec2 projectile_location, Vec2 target, int radius)
{
    int collision_left, collision_right, collision_top, collision_bottom;

    collision_left = target.x - radius;
    collision_right = target.x + radius;
    collision_top = target.y - radius;
    collision_bottom = target.y + radius;

    if (projectile_location.x >= collision_left && projectile_location.x <= collision_right
    && projectile_location.y >= collision_top && projectile_location.y <= collision_bottom)
        return TRUE;
    else
        return FALSE;
}

void hitScan(id_t weapon_id, id_t source_id, Vec2 pos, Vec2 dir, int max_range, int damage)
{
    int bulletpath, i;
    uint8_t hit_something = FALSE;

    for (bulletpath = 0; bulletpath < max_range; bulletpath += BULLET_STEP)
    {
        pos.x += dir.x * BULLET_STEP;
        pos.y += dir.y * BULLET_STEP;

        if (getTileAt(getGridLocation(pos))->block_bullets == TRUE)
        {
            if (weapon_id != WEAPON_FIST)
                particleFx(pos, dir, FX_SPARKS);
            hit_something = TRUE;
            break;
        }
        else if (hit_something == FALSE)
        {
            for (i = 0; i < Game.actor_count; i++)
            {
                Actor_t* actor = &Game.Actors[i];
                if (checkForHit(pos, actor->position, actor->radius) == TRUE)
                {
                    hit_something = TRUE;
                    particleFx(pos, dir, FX_BLOOD);
                    actor->health -= damage;
                    #if DEBUG == 1
                    sprintf(debug[DEBUG_SHOOT], "LAST HIT: %d", i);
                    sprintf(debug[DEBUG_ENTITIES], "TARGET HP: %d", actor->health);
                    #endif
                    if (actor->target_id_secondary == UINT16_MAX)
                        actor->target_id_secondary = actor->target_id_primary; // save previous primary target but only if secondary slot is blank
                    actor->target_id_primary = source_id; // infighting mechanic
                    if (actor->ai_mode != AI_CHASE) // if not yet fighting, fight!
                        actor->ai_mode = AI_CHASE;
                    if (Timers.last_sfx + SFX_INTERVAL < System.ticks)
                    {
                        Timers.last_sfx = System.ticks;
                        if (actor->id == Game.player_id)
                            playSFX(SOUND_HURT);
                        else
                            playSFX(SOUND_HURT_E);
                    }
                    break;
                }
            }
        }
    }
    if (hit_something == FALSE && weapon_id != WEAPON_FIST)
        particleFx(pos, dir, FX_DIRT);
}

void createProjectile(id_t weapon_id, id_t source_id, Vec2 pos, Vec2 dir, int max_range)
{
    static int i = 0;

    Projectiles[i].source_id = source_id;
    Projectiles[i].origin.x = pos.x;
    Projectiles[i].origin.y = pos.y;
    Projectiles[i].position.x = pos.x;
    Projectiles[i].position.y = pos.y;
    Projectiles[i].velocity.x = dir.x * Weapons[weapon_id].projectile_speed;
    Projectiles[i].velocity.y = dir.y * Weapons[weapon_id].projectile_speed;
    Projectiles[i].max_range = max_range * max_range; // max range is squared to save an expensive sqrt operation later
    Projectiles[i].damage = Weapons[weapon_id].damage;
    Projectiles[i].effect_id = Weapons[weapon_id].effect_id;
    Projectiles[i].state = TRUE;
    i++;

    if (i == 64)
        i = 0;
}

void moveProjectiles()
{
    int i, a;
    Vec2 distance;
    Projectile_t* projectile;

    for (i = 0; i < 64; i++)
    {
        if (Projectiles[i].state == TRUE)
        {
            projectile = &Projectiles[i];

            projectile->position.x += projectile->velocity.x;
            projectile->position.y += projectile->velocity.y;
            distance.x = projectile->position.x - projectile->origin.x;
            distance.y = projectile->position.y - projectile->origin.y;
            for (a = 0; a < Game.actor_count; a++)
            {
                Actor_t* actor = &Game.Actors[a];
                if (actor->id != projectile->source_id && checkForHit(projectile->position, actor->position, actor->radius) == TRUE)
                {
                    projectile->state = FALSE;
                    actor->health -= projectile->damage;
                    #if DEBUG == 1
                    sprintf(debug[DEBUG_SHOOT], "LAST HIT: %d", a);
                    sprintf(debug[DEBUG_ENTITIES], "TARGET HP: %d", actor->health);
                    #endif
                    if (actor->target_id_secondary == UINT16_MAX)
                        actor->target_id_secondary = actor->target_id_primary; // save previous primary target but only if secondary slot is blank
                    actor->target_id_primary = projectile->source_id; // infighting mechanic
                    if (actor->ai_mode != AI_CHASE) // if not yet fighting, fight!
                        actor->ai_mode = AI_CHASE;
                    if (Timers.last_sfx + SFX_INTERVAL < System.ticks)
                    {
                        Timers.last_sfx = System.ticks;
                        if (actor->id == Game.player_id)
                        {
                            playSFX(Effects[projectile->effect_id].sound_id);
                            playSFX(SOUND_HURT);
                        }
                        else
                        {
                            playSFX(Effects[projectile->effect_id].sound_id);
                            playSFX(SOUND_HURT_E);
                        }
                    }
                }
            }

            if (projectile->state == TRUE && getVec2LengthSquared(distance) >= projectile->max_range)
            {
                playSFX(Effects[projectile->effect_id].sound_id);
                projectile->state = FALSE;
            }
            else if (projectile->state == TRUE && getTileAt(getGridLocation(projectile->position))->block_bullets == TRUE)
            {
                playSFX(Effects[projectile->effect_id].sound_id);
                projectile->state = FALSE;
            }
        }
    }
}

void shootWeapon(Weapon_t* weapon, Actor_t* source)
{
    Vec2 projectile_loc, direction;
    double angle;
    int i;

    if (source->last_shot + weapon->shot_delay < System.ticks)
    {
        source->last_shot = System.ticks;
        playSFX(weapon->sound_id);
        //particleFx(source->position, source->direction, FX_WATERGUN);

        projectile_loc.x = source->position.x + source->direction.x * (source->radius * 1.5);
        projectile_loc.y = source->position.y + source->direction.y * (source->radius * 1.5);

        for (i = 0; i < weapon->num_projectiles; i++)
        {
            angle = source->angle + ((rand() % weapon->projectile_spread - (weapon->projectile_spread / 2)) * RAD_1);
            direction = getDirVec2(angle);
            if (weapon->projectile_speed == HITSCAN)
                hitScan(weapon->id, source->id, projectile_loc, direction, weapon->range + (rand() % weapon->projectile_spread - (weapon->projectile_spread / 2)), weapon->damage);
            else
                createProjectile(weapon->id, source->id, projectile_loc, direction, weapon->range + (rand() % weapon->projectile_spread - (weapon->projectile_spread / 2)));
        }
    }
}

void entityLoop()
{
    int i;

    for (i = 0; i < MAX_ENTITIES; i++)
    {
        Entity_t* entity = &Entities[i];
        switch (entity->type)
        {
        case ENT_SPAWNER:
            runSpawner(entity);
            break;
        case ENT_TRIGGER:
            runTrigger(entity);
            break;
        case ENT_COUNTER:
            runCounter(entity);
            break;
        case ENT_PORTAL:
            usePortal(entity);
            break;
        default:
            break;
        }
    }
}

void actorDeathLoop()
{
    int i;
    Actor_t* actor;

    for (i = 0; i < Game.actor_count; i++)
    {
        actor = &Game.Actors[i];
        if (actor->health <= 0)
        {
            if (actor->id == Game.player_id)
            { 
                playSFX(SOUND_DEATH);
                loadAfterDeath(Game.current_level_name);
            }       
            else if (actor->trigger_on_death != UINT16_MAX)
            {
                playSFX(SOUND_DEATH_E);
                deathTrigger(i);
                spawnCorpse(actor->position, actor->angle, -1);
                deleteActor(actor->id);
            }
            else
            {
                playSFX(SOUND_DEATH_E);
                spawnCorpse(actor->position, actor->angle, -1);
                deleteActor(actor->id);
            }
        }
    }
}
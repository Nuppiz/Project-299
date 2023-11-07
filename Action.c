#include "Common.h"
#include "Structs.h"
#include "Movecoll.h"
#include "Sound.h"
#include "Vectors.h"
#include "Game.h"
#include "Draw.h"
#include "LvlLoad.h"
#include "Filech.h"
#include "Video.h"

/* Various actions between the player and other Entities/actors */

extern System_t System;
extern Timer_t Timers;
extern GameData_t Game;
extern Entity_t Entities[];
extern Item_t* Items;
extern Weapon_t Weapons[];
WeaponEffect_t Effects[NUM_EFFECTS] = {SOUND_EXPLOSION, 50, 50, 5};

Projectile_t Projectiles[MAX_PROJECTILES] = {0};
int projectile_read = 0;
int projectile_write = 0;
int num_projectiles = 0;

extern Sprite_t Rocket;
extern Sprite_t Explosion;
extern Vec2 camera_offset;

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
                    if (System.debug_mode == TRUE)
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

    if (entity->state == 1)
    {
        struct t_spawner* spawner = &entity->data.spawner;

        if (spawner->last_spawn_time + spawner->spawn_time_interval < System.ticks)
        {
            spawner->last_spawn_time = System.ticks;

            if (spawner->num_actors < spawner->max_actors || spawner->max_actors == -1)
            {
                Actor_t actor = {0};
                
                actor.position.x = entity->x * SQUARE_SIZE + direction.x * (rand() % 50);
                actor.position.y = entity->y * SQUARE_SIZE + direction.y * (rand() % 50);
                actor.angle = spawner->angle;
                /* replace variables from here onwards with template system */
                actor.radius = 7;
                actor.control = 0;
                actor.ai_mode = AI_IDLE;
                actor.ai_timer = 100;
                actor.target_id_primary = Game.player_id;
                actor.health = 100;
                actor.trigger_on_death = -1;
                actor.primary_weapon_id = WEAPON_PISTOL;

                createActor(actor, "SPRITES/DUDE2.7UP");
                spawner->num_actors++;
            }

            if (spawner->num_actors >= spawner->max_actors && spawner->max_actors != -1)
            {
                entity->state = 0;

                if (spawner->only_once == 1)
                    deleteEntity(entity);
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
                saveGameState("CURRENT/");
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

int actorHitLoop(id_t source_id, Vec2 pos, int damage)
{
    int i;
    for (i = 0; i < Game.actor_count; i++)
    {
        Actor_t* actor = &Game.Actors[i];
        if (actor->id != source_id && checkForHit(pos, actor->position, actor->radius) == TRUE)
        {
            actor->health -= damage;
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
            #if DEBUG == 1
            if (System.debug_mode == TRUE)
            {
                sprintf(debug[DEBUG_SHOOT], "LAST HIT: %d", i);
                sprintf(debug[DEBUG_ENTITIES], "TARGET HP: %d", actor->health);
            }
            #endif
            return TRUE;
        }
    }
    return FALSE;
}

void actorHit(id_t source_id, int damage, Actor_t* actor)
{
    actor->health -= damage;
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
    #if DEBUG == 1
    if (System.debug_mode == TRUE)
    {
        sprintf(debug[DEBUG_SHOOT], "LAST HIT: %d", actor->id);
        sprintf(debug[DEBUG_ENTITIES], "TARGET HP: %d", actor->health);
    }
    #endif
}

void splashDamage(id_t source_id, Vec2 pos, uint16_t damage, uint16_t radius)
{
    int splashline, i;

    for (i = 0; i < Game.actor_count; i++)
    {
        Actor_t* actor = &Game.Actors[i];
        for (splashline = 0; splashline <= radius; splashline++)
        {
            Vec2 v = getVec2(pos, actor->position); // vector between impact site and actor
            float distance = normalizeAndGetLength(&v);

            if (distance > radius) // if target is too far
                break;
            if (checkForHit(pos, actor->position, actor->radius) == TRUE)
            {
                actorHit(source_id, damage - splashline, actor);
                #if DEBUG == 1
                if (System.debug_mode == TRUE)
                    sprintf(debug[DEBUG_SHOOT], "S: %d", splashline);
                #endif
                break;
            }
            pos.x += (v.x * 10);
            pos.y += (v.y * 10);
            if (getTileAt(getGridLocation(pos))->block_bullets == TRUE)
            {
                break;
            }
        }
    }
}

void hitScan(id_t weapon_id, id_t source_id, Vec2 pos, Vec2 dir, int max_range, int damage)
{
    int bulletpath, i;
    uint8_t hit_something = FALSE;

    for (bulletpath = 0; bulletpath < max_range; bulletpath += BULLET_STEP)
    {
        #if DEBUG == 1
        if (System.debug_mode == TRUE && boundaryCheck(((int)(pos.x - camera_offset.x)), ((int)(pos.y - camera_offset.y))) == TRUE)
            SET_PIXEL_VGA(((int)(pos.x - camera_offset.x)), ((int)(pos.y - camera_offset.y)), COLOUR_WHITE);
        #endif
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
            hit_something = actorHitLoop(source_id, pos, damage);
            if (hit_something == TRUE)
                particleFx(pos, dir, FX_BLOOD);            
        }
    }
    if (hit_something == FALSE && weapon_id != WEAPON_FIST)
        particleFx(pos, dir, FX_DIRT);
}

void increaseProjectileRead()
{
    projectile_read++;

    if (projectile_read == MAX_PROJECTILES)
        projectile_read = 0;
}

void increaseProjectileWrite()
{
    projectile_write++;

    if (projectile_write == MAX_PROJECTILES)
        projectile_write = 0;

    if (projectile_write == projectile_read)
        increaseProjectileRead();
}

void decrementProjectileWrite()
{
    projectile_write--;

    if (projectile_write < 0)
        projectile_write = MAX_PROJECTILES - 1;
}

void deleteProjectile(int index)
{
    int last_projectile = (projectile_write == 0) ? MAX_PROJECTILES - 1 : projectile_write - 1;

    if (last_projectile != projectile_read)
        Projectiles[index] = Projectiles[last_projectile];

    decrementProjectileWrite();
}

void createProjectile(id_t weapon_id, id_t source_id, Vec2 pos, double angle, Vec2 dir, int max_range)
{
    Projectile_t* projectile = &Projectiles[projectile_write];

    projectile->source_id = source_id;
    projectile->origin.x = pos.x;
    projectile->origin.y = pos.y;
    projectile->position.x = pos.x;
    projectile->position.y = pos.y;
    projectile->angle = angle;
    projectile->velocity.x = dir.x * Weapons[weapon_id].projectile_speed;
    projectile->velocity.y = dir.y * Weapons[weapon_id].projectile_speed;
    projectile->max_range = max_range * max_range; // max range is squared to save an expensive sqrt operation later
    projectile->damage = Weapons[weapon_id].damage;
    projectile->effect_id = Weapons[weapon_id].effect_id;
    projectile->state = TRUE;
    projectile->sprite_id = spawnTempSprite(KILL_COMMAND, MOVING_SPRITE, projectile->position, projectile->velocity, projectile->angle, &Rocket);

    increaseProjectileWrite();
    num_projectiles++;
}

void projectileImpact(int index)
{
    Projectile_t* projectile = &Projectiles[index];
    playSFX(Effects[projectile->effect_id].sound_id);
    spawnTempSprite(RUN_ONCE, STATIC_SPRITE, projectile->position, projectile->velocity, projectile->angle, &Explosion);
    deleteTempSprite(projectile->sprite_id);
    splashDamage(projectile->source_id, projectile->position, Effects[projectile->effect_id].damage, Effects[projectile->effect_id].radius);
    deleteProjectile(index);
    num_projectiles--;
}

void updateProjectile(Projectile_t* projectile)
{
    projectile->position.x += projectile->velocity.x;
    projectile->position.y += projectile->velocity.y;
    updateTempSprite(projectile->sprite_id, projectile->position, projectile->velocity, projectile->angle);
}

void handleProjectiles()
{
    int i = 0;
    int a;
    Vec2 distance;

    while (i != projectile_write)
    {
        Projectile_t* projectile = &Projectiles[i];

        updateProjectile(projectile);
        distance.x = projectile->position.x - projectile->origin.x;
        distance.y = projectile->position.y - projectile->origin.y;

        if (actorHitLoop(projectile->source_id, projectile->position, projectile->damage) == TRUE || // hit someone
            getTileAt(getGridLocation(projectile->position))->block_bullets == TRUE || // hit a solid wall
            getVec2LengthSquared(distance) >= projectile->max_range) // ran out of range
        {
            projectileImpact(i);
            continue;
        }
        i++;
        if (i == MAX_PROJECTILES)
        {
            i = 0;
        }
    }
}

void emptyProjectileArray()
{
    int i;
    memset(Projectiles, 0, sizeof(Projectile_t) * MAX_PROJECTILES);
    projectile_read = 0;
    projectile_write = 0;
}

void shootWeapon(int weapon_id, Actor_t* source)
{
    Vec2 projectile_loc, direction;
    double angle;
    int i;
    Weapon_t* weapon = &Weapons[weapon_id];

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
                createProjectile(weapon->id, source->id, projectile_loc, angle, direction, weapon->range + (rand() % weapon->projectile_spread - (weapon->projectile_spread / 2)));
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
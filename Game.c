#include "Common.h"
#include "LvlLoad.h"
#include "Structs.h"
#include "Loadgfx.h"
#include "Vectors.h"

/* Game data and actor array functions */

GameData_t Game = {0};
extern Texture_array ActorTextures;
extern System_t System;

id_t getNewId()
{
    id_t id;
    // to do; optimize with a cached "free list" to avoid looping through really long id lists
    // fixed bug where only every other number was used
    // start id count from 1 so 0 is never used; assign 0 as "no id" if needed
    for (id = 1; id < Game.id_capacity; id++)
        if (Game.ActorsById[id] == UINT16_MAX)
            return id;
    // no free IDs found; allocate more
    Game.id_capacity += 16; // define as CHUNK or BLOCK or something
    Game.ActorsById = realloc(Game.ActorsById, Game.id_capacity * sizeof(id_t));
    memset(&Game.ActorsById[Game.id_capacity - 16], UINT8_MAX, 16 * sizeof(id_t)); // set new ids to -1
    // to do later: ensure successful allocation
    return id;
}

id_t createActor(float x, float y, double angle, int radius, uint8_t control, uint8_t ai_mode, int ai_timer, id_t ai_target, int health, int8_t trigger_on_death, uint16_t shot_delay, char* texture_name)
{
	Vec2 direction; // temporary container for direction value
    id_t id = getNewId();
	direction = getDirVec2(angle);

    if (Game.actor_count >= Game.actor_capacity)
    {
        Game.actor_capacity += 16; // define as CHUNK or BLOCK or something
        Game.Actors = realloc(Game.Actors, Game.actor_capacity * sizeof(Actor_t));
        // to do later: ensure successful allocation
    }

    Game.ActorsById[id] = Game.actor_count;

    memset(&Game.Actors[Game.actor_count], 0, sizeof(Actor_t));

    Game.Actors[Game.actor_count].id = id;
    Game.Actors[Game.actor_count].position.x = x;
    Game.Actors[Game.actor_count].position.y = y;
    Game.Actors[Game.actor_count].angle = angle;
    Game.Actors[Game.actor_count].radius = radius;
    Game.Actors[Game.actor_count].control = control;
    Game.Actors[Game.actor_count].ai_mode = ai_mode;
    Game.Actors[Game.actor_count].ai_timer = ai_timer;
    Game.Actors[Game.actor_count].target_id = ai_target;
    Game.Actors[Game.actor_count].health = health;
    Game.Actors[Game.actor_count].trigger_on_death = trigger_on_death;
    Game.Actors[Game.actor_count].shot_delay = shot_delay;
    Game.Actors[Game.actor_count].texture_id = loadTexture(texture_name, &ActorTextures);
	Game.Actors[Game.actor_count].direction.x = direction.x;
	Game.Actors[Game.actor_count].direction.y = direction.y;
    Game.Actors[Game.actor_count].last_shot = System.ticks;

    Game.actor_count++;
    
    return id;
}

void deleteActor(id_t id)
{
    if (Game.ActorsById[id] != Game.actor_count - 1)
    {
        Game.Actors[Game.ActorsById[id]] = Game.Actors[Game.actor_count - 1];
        Game.ActorsById[Game.actor_count] = Game.actor_count;
    }
    
    Game.actor_count--;
    // set id value to UINT16_MAX so it can be reused
    Game.ActorsById[id] = UINT16_MAX;

    if (Game.actor_count < Game.actor_capacity - 16)
    {
        Game.actor_capacity -= 16;
        Game.Actors = realloc(Game.Actors, Game.actor_capacity * sizeof(Actor_t));
    }
}

void deleteLastActor()
{
    if (Game.actor_count > 0)
        deleteActor(Game.actor_count - 1);
}

void initGameData(id_t actor_capacity, id_t id_capacity)
{
    Game.Actors = calloc(actor_capacity, sizeof(Actor_t));
    Game.ActorsById = calloc(id_capacity, sizeof(id_t));
    memset(Game.ActorsById, UINT8_MAX, id_capacity * sizeof(id_t));
}

void freeGameData()
{
    memset(Game.Map.tilemap, 0, Game.Map.width * Game.Map.height);
    free(Game.Map.tilemap);
    memset(Game.Actors, 0, Game.actor_capacity * sizeof(Actor_t));
    free(Game.Actors);
    Game.actor_count = 0;
    memset(Game.ActorsById, 0, Game.id_capacity * sizeof(id_t));
    free(Game.ActorsById);
    Game.id_capacity = 0;
    memset(Game.current_level_name, 0, strlen(Game.current_level_name) * sizeof(char));
}
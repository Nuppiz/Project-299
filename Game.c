#include "Common.h"
#include "LvlLoad.h"
#include "Structs.h"
#include "Loadgfx.h"
#include "Vectors.h"

/* Game data and object array functions */

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
        if (Game.ObjectsById[id] == UINT16_MAX)
            return id;
    // no free IDs found; allocate more
    Game.id_capacity += 16; // define as CHUNK or BLOCK or something
    Game.ObjectsById = realloc(Game.ObjectsById, Game.id_capacity * sizeof(id_t));
    memset(&Game.ObjectsById[Game.id_capacity - 16], UINT8_MAX, 16 * sizeof(id_t)); // set new ids to -1
    // to do later: ensure successful allocation
    return id;
}

id_t createObject(float x, float y, double angle, int radius, uint8_t control, uint8_t ai_mode, int ai_timer, id_t ai_target, int health, int8_t trigger_on_death, uint16_t shot_delay, char* texture_name)
{
	Vec2 direction; // temporary container for direction value
    id_t id = getNewId();
	direction = getDirVec2(angle);

    if (Game.object_count >= Game.object_capacity)
    {
        Game.object_capacity += 16; // define as CHUNK or BLOCK or something
        Game.Objects = realloc(Game.Objects, Game.object_capacity * sizeof(Object_t));
        // to do later: ensure successful allocation
    }

    Game.ObjectsById[id] = Game.object_count;

    memset(&Game.Objects[Game.object_count], 0, sizeof(Object_t));

    Game.Objects[Game.object_count].id = id;
    Game.Objects[Game.object_count].position.x = x;
    Game.Objects[Game.object_count].position.y = y;
    Game.Objects[Game.object_count].angle = angle;
    Game.Objects[Game.object_count].radius = radius;
    Game.Objects[Game.object_count].control = control;
    Game.Objects[Game.object_count].ai_mode = ai_mode;
    Game.Objects[Game.object_count].ai_timer = ai_timer;
    Game.Objects[Game.object_count].target_id = ai_target;
    Game.Objects[Game.object_count].health = health;
    Game.Objects[Game.object_count].trigger_on_death = trigger_on_death;
    Game.Objects[Game.object_count].shot_delay = shot_delay;
    Game.Objects[Game.object_count].texture_id = loadTexture(texture_name, &ActorTextures);
	Game.Objects[Game.object_count].direction.x = direction.x;
	Game.Objects[Game.object_count].direction.y = direction.y;
    Game.Objects[Game.object_count].last_shot = System.ticks;

    Game.object_count++;
    
    return id;
}

void deleteObject(id_t id)
{
    if (Game.ObjectsById[id] != Game.object_count - 1)
    {
        Game.Objects[Game.ObjectsById[id]] = Game.Objects[Game.object_count - 1];
        Game.ObjectsById[Game.object_count] = Game.object_count;
    }
    
    Game.object_count--;
    // set id value to UINT16_MAX so it can be reused
    Game.ObjectsById[id] = UINT16_MAX;

    if (Game.object_count < Game.object_capacity - 16)
    {
        Game.object_capacity -= 16;
        Game.Objects = realloc(Game.Objects, Game.object_capacity * sizeof(Object_t));
    }
}

void deleteLastObject()
{
    if (Game.object_count > 0)
        deleteObject(Game.object_count - 1);
}

void initGameData(id_t object_capacity, id_t id_capacity)
{
    Game.Objects = calloc(object_capacity, sizeof(Object_t));
    Game.ObjectsById = calloc(id_capacity, sizeof(id_t));
    memset(Game.ObjectsById, UINT8_MAX, id_capacity * sizeof(id_t));
}

void freeGameData()
{
    memset(Game.Map.tilemap, 0, Game.Map.width * Game.Map.height);
    free(Game.Map.tilemap);
    memset(Game.Objects, 0, Game.object_capacity * sizeof(Object_t));
    free(Game.Objects);
    Game.object_count = 0;
    memset(Game.ObjectsById, 0, Game.id_capacity * sizeof(id_t));
    free(Game.ObjectsById);
    Game.id_capacity = 0;
    memset(Game.current_level_name, 0, strlen(Game.current_level_name) * sizeof(char));
}
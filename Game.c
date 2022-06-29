#include "Common.h"
#include "LvlLoad.h"
#include "Structs.h"
#include "Vectors.h"

GameData_t Game = {0};

// instead of referring to a Texture array, objects should simply store an id
// which acts as an index to an array of Texture pointers
extern Texture_t Textures[NUM_TEXTURES];

id_t getNewId()
{
    id_t id;
    // to do; optimize with a cached "free list" to avoid looping through really long id lists
    // fixed bug where only every other number was used
    // start id count from 1 so 0 is never used; assign 0 as "no id" if needed
    for (id = 0; id < Game.id_capacity; id++)
        if (Game.ObjectsById[id] == NULL)
            return id;
    // no free IDs found; allocate more
    Game.id_capacity += 16; // define as CHUNK or BLOCK or something
    Game.ObjectsById = realloc(Game.ObjectsById, Game.id_capacity * sizeof(void*));
    memset(Game.ObjectsById, 0, 16 * sizeof(void*)); // set new pointers to NULL
    // to do later: ensure successful allocation
    return id;
}

id_t createObject(float x, float y, double angle, int radius, uint8_t control, uint8_t ai_mode, id_t ai_target, Texture_t* sprite)
{
	Vec2 direction; //temporary container for direction value
    id_t id = getNewId();
	direction = getDirVec2(angle);

    if (Game.object_count >= Game.object_capacity)
    {
        Game.object_capacity += 16; // define as CHUNK or BLOCK or something
        Game.Objects = realloc(Game.Objects, Game.object_capacity * sizeof(Object_t));
        // to do later: ensure successful allocation
    }
    // set the ID entry in the hashmap (ObjectsById) to point to the newly created object
    Game.ObjectsById[id] = &Game.Objects[Game.object_count];

    Game.Objects[Game.object_count].id = id;
    Game.Objects[Game.object_count].position.x = x;
    Game.Objects[Game.object_count].position.y = y;
    Game.Objects[Game.object_count].angle = angle;
    Game.Objects[Game.object_count].radius = radius;
    Game.Objects[Game.object_count].control = control;
    Game.Objects[Game.object_count].ai_mode = ai_mode;
    Game.Objects[Game.object_count].target_id = ai_target;
    Game.Objects[Game.object_count].sprite = *sprite;
	Game.Objects[Game.object_count].direction.x = direction.x;
	Game.Objects[Game.object_count].direction.y = direction.y;
	
	Game.Objects[Game.object_count].velocity.x = 0.0;
	Game.Objects[Game.object_count].velocity.y = 0.0;

    Game.object_count++;
    
    return id;
}

void deleteObject(id_t id)
{
    // overwrite memory
    if (Game.ObjectsById[id] != &Game.Objects[Game.object_count])
        *Game.ObjectsById[id] = Game.Objects[Game.object_count-1];
    Game.object_count--;
    // set hashmap value to NULL
    Game.ObjectsById[id] = NULL;

    if (Game.object_count < Game.object_capacity - 16)
    {
        Game.object_capacity -= 16;
        Game.Objects = realloc(Game.Objects, Game.object_capacity * sizeof(Object_t));
    }
}

void deleteLastObject()
{
    if (Game.object_count > 0)
        deleteObject(Game.object_count-1);
}

void createInitialObjects()
{   
    Game.player_id =
    createObject(170, 350, 0.1, 7, 0, NULL, 0, &Textures[DUDE1]);
    
    createObject(280, 90,  1, 7, 0, AI_IDLE, Game.player_id, &Textures[DUDE3]);
    createObject(80,  110, 0.1, 7, 0, AI_IDLE, 1,              &Textures[DUDE2]);
    createObject(200, 350, 0.1, 7, 0, AI_IDLE, 2,              &Textures[DUDE1]);
    /*createObject(200, 350, 0, 7, 0, AI_IDLE, 2,              &Textures[DUDE1]);
    createObject(200, 350, 0, 7, 0, AI_IDLE, 2,              &Textures[DUDE1]);
    createObject(200, 350, 0, 7, 0, AI_IDLE, 2,              &Textures[DUDE1]);
    createObject(200, 350, 0, 7, 0, AI_IDLE, 2,              &Textures[DUDE1]);
    createObject(200, 350, 0, 7, 0, AI_IDLE, 2,              &Textures[DUDE1]);
    createObject(200, 350, 0, 7, 0, AI_IDLE, 2,              &Textures[DUDE1]);
    createObject(200, 350, 0, 7, 0, AI_IDLE, 2,              &Textures[DUDE1]);
    createObject(200, 350, 0, 7, 0, AI_IDLE, 2,              &Textures[DUDE1]);
    createObject(200, 350, 0, 7, 0, AI_IDLE, 2,              &Textures[DUDE1]);
    createObject(200, 350, 0, 7, 0, AI_IDLE, 2,              &Textures[DUDE1]);
    createObject(200, 350, 0, 7, 0, AI_IDLE, 2,              &Textures[DUDE1]);
    createObject(200, 350, 0, 7, 0, AI_IDLE, 2,              &Textures[DUDE1]);*/
}

void initGame()
{
    //Game.Map.width = 24;
    //Game.Map.height = 20;
    //Game.Map.collision = CollisionMap;
    //Game.Map.tiles = TileMap;
    Game.object_capacity = 16;
    Game.id_capacity = 16;
    Game.Objects = malloc(Game.object_capacity * sizeof(Object_t));
    Game.ObjectsById = calloc(Game.id_capacity, sizeof(void*));
    Game.Map.level_num = 1;
    levelLoader();
    //createInitialObjects();
}
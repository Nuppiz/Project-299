#include "Common.h"
#include "Structs.h"

GameData_t Game = {0};

// instead of referring to a Texture array, objects should simply store an id
// which acts as an index to an array of Texture pointers
extern Texture_t Textures[NUM_TEXTURES];

// array which determines the collision data of each square on the grid
uint8_t CollisionMap [] =
{
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0,
  0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0,
  0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0,
  0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0,
  0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0,
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

uint8_t TileMap [] =
{
 11, 7, 7, 7, 7,14, 7, 7,14, 7, 7,14, 7, 7, 7, 7,12, 6, 6, 6, 6, 6, 3, 3,
  8, 5, 5, 5, 5, 4, 4,30, 8,31, 4, 4, 5, 5, 5, 5, 8, 1, 1, 1, 1, 1, 3, 3,
  8, 5, 5, 5, 5, 9, 7, 7,13, 7, 7,10, 5, 5, 5, 5, 8, 1, 1, 1, 1, 3, 3, 3,
  8, 5, 5, 5, 5, 5, 5, 5, 8, 5, 5, 5, 5, 5, 5, 5, 8, 1, 1, 1, 1, 3, 3, 3,
  8, 5, 5, 5, 5, 5, 5, 5, 8, 5, 5, 5, 5, 5, 5, 5, 8, 1, 1, 1, 1, 3, 3, 3,
  8, 5, 5, 5, 5, 5, 5, 5, 8, 5, 5, 5, 5, 5, 5, 5, 8, 1, 1, 1, 1, 3, 3, 3,
  9, 7, 7, 7, 7, 7, 7, 5,17, 5, 7, 7, 7, 7, 7, 7,10, 1, 1, 1, 1, 3, 3, 3,
  6, 0, 0, 0, 0, 0, 0,19, 0,19, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 3, 3, 3, 3,
  6, 0, 0, 0, 0, 0, 0,19, 0,19, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 3, 3, 3, 3,
  6, 0, 0, 0, 0,22,18,28,18,26, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 3, 3, 3, 3,
  6, 0, 0, 0, 0,19, 0, 0, 0,19, 0, 0, 0, 0, 0, 0, 0, 1, 1, 3, 3, 3, 3, 3,
  6, 0, 0, 0, 0,19, 0, 0, 0,19, 0, 0, 0, 0, 0, 0, 1, 1, 3, 3, 3, 3, 3, 3,
  6, 0, 0, 0, 0,19, 0, 0, 0,19, 0, 0, 0, 0, 0, 0, 1, 1, 3, 3, 3, 3, 3, 3,
 18,18,18,18,18,24,18,18,18,24,18,18,18,18,18,18, 1, 1, 3, 3, 3, 3, 3, 3,
  6, 0, 0, 0, 0,19, 0, 0, 0,19, 0, 0, 0, 0, 0, 1, 1, 3, 3, 3, 3, 3, 3, 3,
  6, 0, 0, 0, 0,19, 0, 0, 0,19, 0, 0, 0, 0, 0, 1, 1, 3, 3, 3, 3, 3, 3, 3,
  6, 0, 0, 0, 0,20,18,18,18,28,18,18,23, 0, 1, 1, 3, 3, 3, 3, 3, 3, 3, 3,
  6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,19, 0, 1, 1, 3, 3, 3, 3, 3, 3, 3, 3,
  6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,19, 1, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,19, 6, 6, 3, 3, 3, 3, 3, 3, 3, 3, 3
};

id_t getNewId()
{
    id_t id;
    // to do; optimize with a cached "free list" to avoid looping through really long id lists
    for (id = 0; id <= Game.id_capacity; id++)
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
    id_t id = getNewId();

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
    createObject(170, 350, 0, 7, 0, NULL, 0, &Textures[DUDE1]);
    
    createObject(280, 90,  1, 7, 0, AI_NONE, Game.player_id, &Textures[DUDE3]);
    createObject(80,  110, 0, 7, 0, AI_IDLE, 1,              &Textures[DUDE2]);
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
    createObject(200, 350, 0, 7, 0, AI_IDLE, 2,              &Textures[DUDE1]);
    createObject(200, 350, 0, 7, 0, AI_IDLE, 2,              &Textures[DUDE1]);
    createObject(200, 350, 0, 7, 0, AI_IDLE, 2,              &Textures[DUDE1]);
}

void initGame()
{
    Game.Map.width = 24;
    Game.Map.height = 20;
    Game.Map.collision = CollisionMap;
    Game.Map.tiles = TileMap;
    Game.object_capacity = 16;
    Game.id_capacity = 16;
    Game.Objects = malloc(Game.object_capacity * sizeof(Object_t));
    Game.ObjectsById = calloc(Game.id_capacity, sizeof(void*));
    createInitialObjects();
}
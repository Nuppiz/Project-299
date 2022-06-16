#include "Common.h"
#include "Structs.h"

Map_t map1;
Map_t* currentMap;
Object_t* Objects;
int player;
int object_count = 0;
int object_capacity = 16;
int sprite_index = 0;
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

// array which holds all objects
/*Object_t Objects[NUM_OBJECTS] =
{ // position         gridloc direction     angle velocity    magn rad ctrl aimode aitimer ai_target             color sprite
    {{170.0, 350.0,}, {0,0},  {1.0, 1.0},   0,    {0.0, 0.0}, 0,   7,  0,   NULL,  0,      NULL,                 14,   NULL},
    {{80.0,  110.0,}, {0,0},  {1.0, 1.0},   0,    {0.0, 0.0}, 0,   7,  0,   IDLE,  100,    &Objects[2].position, 43,   NULL},
    {{280.0,  90.0,}, {0,0},  {-1.0, -2.0}, 1,    {0.0, 0.0}, 0,   7,  0,   IDLE,  100,    &player.position,     12,   NULL}
};*/

void createObject(float x, float y, double angle, int radius, uint8_t control, uint8_t ai_mode, Vec2* ai_target, Texture_t* sprite)
{
    if (object_count >= object_capacity)
    {
        object_capacity += 16;
        Objects = realloc(Objects, object_capacity * sizeof(Object_t));
    }
    Objects[object_count].position.x = x;
    Objects[object_count].position.y = y;
    Objects[object_count].angle = angle;
    Objects[object_count].radius = radius;
    Objects[object_count].control = control;
    Objects[object_count].ai_mode = ai_mode;
    Objects[object_count].ai_target = ai_target;
    Objects[object_count].sprite = *sprite;

    object_count++;
    //return object_count - 1;
}

void deleteObject(int object_index)
{
    if (object_index != object_count - 1)
        Objects[object_index] = Objects[object_count - 1];
    object_count--;

    if (object_count < object_capacity - 16)
    {
        object_capacity -= 16;
        Objects = realloc(Objects, object_capacity * sizeof(Object_t));
    }
}

void createInitialObjects()
{   
    player = 0;
    createObject(170, 350, 0, 7, 0, NULL, NULL, &Textures[DUDE1]);
    createObject(280, 90,  1, 7, 0, IDLE, &Objects[player].position, &Textures[DUDE3]);
    createObject(80,  110, 0, 7, 0, IDLE, &Objects[1].position, &Textures[DUDE2]);
    createObject(200, 350, 0, 7, 0, IDLE, &Objects[2].position, &Textures[DUDE1]);
    createObject(200, 350, 0, 7, 0, IDLE, &Objects[2].position, &Textures[DUDE1]);
    createObject(200, 350, 0, 7, 0, IDLE, &Objects[2].position, &Textures[DUDE1]);
    createObject(200, 350, 0, 7, 0, IDLE, &Objects[2].position, &Textures[DUDE1]);
    createObject(200, 350, 0, 7, 0, IDLE, &Objects[2].position, &Textures[DUDE1]);
    createObject(200, 350, 0, 7, 0, IDLE, &Objects[2].position, &Textures[DUDE1]);
    createObject(200, 350, 0, 7, 0, IDLE, &Objects[2].position, &Textures[DUDE1]);
    createObject(200, 350, 0, 7, 0, IDLE, &Objects[2].position, &Textures[DUDE1]);
    createObject(200, 350, 0, 7, 0, IDLE, &Objects[2].position, &Textures[DUDE1]);
    createObject(200, 350, 0, 7, 0, IDLE, &Objects[2].position, &Textures[DUDE1]);
    createObject(200, 350, 0, 7, 0, IDLE, &Objects[2].position, &Textures[DUDE1]);
    createObject(200, 350, 0, 7, 0, IDLE, &Objects[2].position, &Textures[DUDE1]);
    createObject(200, 350, 0, 7, 0, IDLE, &Objects[2].position, &Textures[DUDE1]);
    createObject(200, 350, 0, 7, 0, IDLE, &Objects[2].position, &Textures[DUDE1]);
    createObject(200, 350, 0, 7, 0, IDLE, &Objects[2].position, &Textures[DUDE1]);
    createObject(200, 350, 0, 7, 0, IDLE, &Objects[2].position, &Textures[DUDE1]);
    createObject(200, 350, 0, 7, 0, IDLE, &Objects[2].position, &Textures[DUDE1]);
}

void initGame()
{
    map1.width = 24;
    map1.height = 20;
    map1.collision = CollisionMap;
    map1.tiles = TileMap;
    currentMap = &map1;
    Objects = malloc(object_capacity * sizeof(Object_t));
    createInitialObjects();
}
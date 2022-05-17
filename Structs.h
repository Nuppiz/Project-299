#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct
{
    float x, y;
} Vec2;

typedef struct
{
    int x, y;
} Vec2_int;

typedef struct
{
    Vec2 position;
    Vec2_int grid_loc; // location on the grid
    Vec2 direction; // direction
    Vec2 velocity; // velocity
    float magnitude; // magnitude of velocity
    int radius; // circle radius
    uint8_t color;
    uint8_t ai_mode;
    int ai_timer;
    Vec2* ai_target;
} Object;

typedef struct
{
    uint8_t r,g,b;
} Color;

typedef struct
{
    Color colors[256];
} Palette;

typedef struct
{
    uint8_t* pixels;
    uint16_t height;
    uint16_t width;
    uint16_t transparent;
} Texture;

typedef struct
{
    int width; // width in tiles
    int height; // height in tiles
    uint8_t* collision; // array to store collision data
    uint8_t* textures; // array to store texture pointers
} Map;

#endif

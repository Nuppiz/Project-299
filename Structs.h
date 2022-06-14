#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct
{
    int    running;
    time_t time;        // global timer
    time_t seconds;     // second timer (time/1000)
    time_t ticks;       // game logic cycles
    time_t frames;      // total frames drawn
    int    tick_rate;   // not used yet
    int    frame_rate;  // target fps
    int    frame_time;  // time of a single frame in milliseconds
    int    fps;         // measured fps
    float  fps_avg;     // frames / seconds
} System_t;

typedef struct
{
    uint8_t keycode;
    uint8_t type;
    time_t time;
} KeyEvent_t;

typedef struct
{
    KeyEvent_t queue[256];
    uint8_t keystates[256];
    uint8_t queue_head;
    uint8_t queue_tail;
} Keyboard_t;

typedef struct
{
    double x, y;
} Vec2;

typedef struct
{
    int x, y;
} Vec2_int;

typedef struct
{
    uint8_t* pixels;
    uint16_t height;
    uint16_t width;
    uint16_t transparent;
    int offset_x, offset_y;
} Texture_t;

typedef struct
{
    Vec2 position;
    Vec2_int grid_loc; // location on the grid
    Vec2 direction; // direction
    double angle; // current direction in radians
    Vec2 velocity; // velocity
    float magnitude; // magnitude of velocity
    int radius; // collision box/circle radius
    
    uint8_t control;
    uint8_t ai_mode;
    int ai_timer;
    Vec2* ai_target;

    uint8_t color;
    Texture_t sprite;
    Texture_t orig_sprite;
} Object_t;

typedef struct
{
    uint8_t r,g,b;
} Color_t;

typedef struct
{
    Color_t colors[256];
} Palette_t;

typedef struct
{
    int width; // width in tiles
    int height; // height in tiles
    uint8_t* collision; // array to store collision data
    uint8_t* textures; // array to store texture pointers
} Map_t;

#endif/* STRUCTS_H */

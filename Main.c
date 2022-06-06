#include "Common.h"
#include "Structs.h"
#include "AI.h"
#include "Defines.h"
#include "Draw.h"
#include "Enums.h"
#include "Input.h"
#include "Loadgfx.h"
#include "Movecoll.h"
#include "Text.h"
#include "Video.h"

int running = 1;
int heading = 0; // current direction in degrees
float radians = 0.0; // current direction in radians, as that's what the math functions use
Vec2 camera_offset;

extern Texture Textures[];

// array which holds all objects (circles in this case)
Object object_array[Num_Objects] = {
//    pos_x  pos_y    grid_x grid_y   direction     velocity    magnitude radius  colour  ai_mode       ai_timer  ai_target
    {{170.0, 100.0,}, {9,    5,},     {1.0, 1.0},   {0.0, 0.0}, 0.0,      7,      14,     NULL,         0,        NULL},
    {{80.0,  110.0,}, {4,    5,},     {1.0, 1.0},   {0.0, 0.0}, 0.0,      7,      43,     IDLE,         100,      &object_array[2].position},
    {{280.0, 140.0,}, {14,   7,},     {-1.0, -2.0}, {0.0, 0.0}, 0.0,      7,      12,     IDLE,         100,      &player.position}
};

// array which determines the collision data of each square on the grid
uint8_t collision_array [] = {7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
                         7,  54, 55, 54, 55, 54, 55, 54, 55, 54, 55, 54, 55, 54, 55, 54, 55, 54, 7,
                         7,  55, 7,  7,  7,  55, 54, 55, 54, 55, 54, 55, 54, 55, 54, 55, 54, 55, 7,
                         7,  54, 7,  54, 55, 54, 55, 54, 55, 54, 55, 54, 7,  7,  55, 54, 55, 54, 7,
                         7,  55, 7,  55, 54, 55, 54, 55, 54, 55, 7,  55, 54, 7,  54, 55, 54, 55, 7,
                         7,  54, 55, 54, 55, 54, 7,  7,  55, 54, 7,  54, 55, 7,  55, 54, 55, 54, 7,
                         7,  55, 54, 55, 54, 55, 7,  7,  54, 55, 7,  55, 54, 55, 54, 55, 54, 55, 7,
                         7,  54, 55, 54, 55, 54, 55, 54, 55, 54, 55, 54, 55, 54, 55, 54, 55, 54, 7,
                         7,  55, 54, 55, 54, 55, 54, 55, 54, 55, 54, 55, 54, 55, 54, 55, 54, 55, 7,
                         7,  54, 55, 54, 55, 54, 7,  7,  55, 54, 7,  54, 55, 54, 55, 54, 55, 54, 7,
                         7,  55, 54, 55, 54, 55, 7,  7,  54, 55, 7,  55, 54, 7,  54, 55, 54, 55, 7,
                         7,  54, 55, 54, 55, 54, 55, 54, 55, 54, 55, 54, 55, 54, 55, 54, 55, 54, 7,
                         7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7
};

/* array which determines the texture of each square on the grid */
uint8_t texture_array [] = {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 0,
            0, 2, 0, 0, 0, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 0,
            0, 1, 0, 1, 2, 1, 2, 1, 2, 1, 2, 1, 0, 0, 2, 1, 2, 1, 0,
            0, 2, 0, 2, 1, 2, 1, 2, 1, 2, 0, 2, 1, 0, 1, 2, 1, 2, 0,
            0, 1, 2, 1, 2, 1, 0, 0, 2, 1, 0, 1, 2, 0, 2, 1, 2, 1, 0,
            0, 2, 1, 2, 1, 2, 0, 0, 1, 2, 0, 2, 1, 2, 1, 2, 1, 2, 0,
            0, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 0,
            0, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 0,
            0, 1, 2, 1, 2, 1, 0, 0, 2, 1, 0, 1, 2, 1, 2, 1, 2, 1, 0,
            0, 2, 1, 2, 1, 2, 0, 0, 1, 2, 0, 2, 1, 0, 1, 2, 1, 2, 0,
            0, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

Map map1;

void quit()
{   
    deinitKeyboard();
    setMode(TEXT_MODE);
}

void main()
{
    setMode(VGA_256_COLOR_MODE);
    initKeyboard();
    loadFont();
    loadAllTextures();

    map1.width = 19;
    map1.height = 13;

    map1.collision = collision_array;
    map1.textures = texture_array;

    object_array[0].orig_sprite = Textures[DUDE1];
    object_array[1].orig_sprite = Textures[DUDE2];
    object_array[2].orig_sprite = Textures[DUDE3];
    
    while (running == 1)
    {
        processInput();
        radians = degToRad(heading);
        AILoop();
        calculateMovements(&map1);
        collision(&map1);
        calcCameraOffset(&player);
        render();
        drawStuff();
        delay(50);
    }
    quit();
}
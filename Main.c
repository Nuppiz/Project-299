#include "Common.h"

#include "AI.h"
#include "Defines.h"
#include "Draw.h"
#include "Enums.h"
#include "Input.h"
#include "Loadgfx.h"
#include "Movecoll.h"
#include "Structs.h"
#include "Text.h"
#include "Video.h"

int running = 1;
int heading = 0; // current direction in degrees
float radians = 0.0; // current direction in radians, as that's what the math functions use
Vec2_int pl_grid; // vector to hold the player's grid location

// array which holds all objects (circles in this case)
Object object_array[Num_Objects] = {
//    pos_x  pos_y    grid_x grid_y   direction     velocity    magnitude radius  colour  ai_mode       ai_timer  ai_target
    {{170.0, 100.0,}, {10,   5,},     {1.0, 1.0},   {0.0, 0.0}, 0.0,      8,      14,     IDLE,         0,        &object_array[2].position},
    {{40.0,  110.0,}, {4,    5,},     {1.0, 1.0},   {0.0, 0.0}, 0.0,      8,      43,     IDLE,         100,      &object_array[2].position},
    {{250.0, 120.0,}, {14,   7,},     {-1.0, -2.0}, {0.0, 0.0}, 0.0,      8,      12,     IDLE,         100,      &player.position}
};

// array which determines the colour of each square on the grid
/*uint8_t grid_array [] = {7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
                         7,  54, 55, 54, 55, 54, 55, 54, 55, 54, 55, 54, 55, 54, 55, 7,
                         7,  55, 7,  7,  7,  55, 54, 55, 54, 55, 54, 55, 54, 55, 54, 7,
                         7,  54, 7,  54, 55, 54, 55, 54, 55, 54, 55, 54, 7,  7,  55, 7,
                         7,  55, 7,  55, 54, 55, 54, 55, 54, 55, 7,  55, 54, 7,  54, 7,
                         7,  54, 55, 54, 55, 54, 7,  7,  55, 54, 7,  54, 55, 7,  55, 7,
                         7,  55, 54, 55, 54, 55, 7,  7,  54, 55, 7,  55, 54, 55, 54, 7,
                         7,  54, 55, 54, 55, 54, 55, 54, 55, 54, 55, 54, 55, 54, 55, 7,
                         7,  55, 54, 55, 54, 55, 54, 55, 54, 55, 54, 55, 54, 55, 54, 7,
                         7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7
};

// array which determines the texture of each square on the grid
uint8_t texture_array [] = {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 0,
            0, 2, 0, 0, 0, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 0,
            0, 1, 0, 1, 2, 1, 2, 1, 2, 1, 2, 1, 0, 0, 2, 0,
            0, 2, 0, 2, 1, 2, 1, 2, 1, 2, 0, 2, 1, 0, 1, 0,
            0, 1, 2, 1, 2, 1, 0, 0, 2, 1, 0, 1, 2, 0, 2, 0,
            0, 2, 1, 2, 1, 2, 0, 0, 1, 2, 0, 2, 1, 2, 1, 0,
            0, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 0,
            0, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};*/

// array which determines the colour of each square on the grid
uint8_t grid_array [] = {7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
                         7,  54, 55, 54, 55, 54, 55, 54, 55, 54, 55, 54, 55, 54, 55, 54, 55, 54, 7,
                         7,  55, 7,  7,  7,  55, 54, 55, 54, 55, 54, 55, 54, 55, 54, 55, 54, 55, 7,
                         7,  54, 7,  54, 55, 54, 55, 54, 55, 54, 55, 54, 7,  7,  55, 54, 55, 54, 7,
                         7,  55, 7,  55, 54, 55, 54, 55, 54, 55, 7,  55, 54, 7,  54, 55, 54, 55, 7,
                         7,  54, 55, 54, 55, 54, 7,  7,  55, 54, 7,  54, 55, 7,  55, 54, 55, 54, 7,
                         7,  55, 54, 55, 54, 55, 7,  7,  54, 55, 7,  55, 54, 55, 54, 55, 54, 55, 7,
                         7,  54, 55, 54, 55, 54, 55, 54, 55, 54, 55, 54, 55, 54, 55, 54, 55, 54, 7,
                         7,  55, 54, 55, 54, 55, 54, 55, 54, 55, 54, 55, 54, 55, 54, 55, 54, 55, 7,
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
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

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
    
    while (running == 1)
    {
        processInput();
        radians = degToRad(heading);
        AILoop();
        calculateMovements();
        collision();
        render();
        drawStuff();
        delay(50);
    }
    quit();
}
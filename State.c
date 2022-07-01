#include "Init.h"
#include "Input.h"
#include "Update.h"
#include "Draw.h"

/* Game states and their manager */

extern State* current_state;

State Titlescreen = {
    titleInit,
    titleInput,
    titleUpdate,
    titleDraw
};

State Ingame = {
    gameInit,
    gameInput,
    gameUpdate,
    gameDraw
};

void switchState(uint8_t state)
{
        switch (state)
        {
            case STATE_TITLE:   current_state = &Titlescreen; 
                                current_state->init();
                                break;
            case STATE_INGAME:  current_state = &Ingame;
                                current_state->init();
                                break;
        }
}
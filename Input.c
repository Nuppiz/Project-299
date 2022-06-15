#include "Common.h"
#include "Structs.h"
#include "Vectors.h"
#include "Keyb.h"

uint8_t player_control = 0;
extern System_t System;
extern Keyboard_t Keyboard;

int control_up = KEY_UP;
int control_down = KEY_DOWN;
int control_left = KEY_LEFT;
int control_right = KEY_RIGHT;
int control_fast = KEY_LSHIFT;

void playerControl()
{
    #if DEBUG == 1
    char* d = debug[DEBUG_AICHASE];
    #endif

    if (KEY_IS_PRESSED(control_up))     player_control |= CONTROL_UP;
    else                                player_control &= ~CONTROL_UP;

    if (KEY_IS_PRESSED(control_down))   player_control |= CONTROL_DOWN;
    else                                player_control &= ~CONTROL_DOWN;

    if (KEY_IS_PRESSED(control_left))   player_control |= CONTROL_LEFT;
    else                                player_control &= ~CONTROL_LEFT;

    if (KEY_IS_PRESSED(control_right))  player_control |= CONTROL_RIGHT;
    else                                player_control &= ~CONTROL_RIGHT;

    if (KEY_IS_PRESSED(control_fast))   player_control |= CONTROL_FAST;
    else                                player_control &= ~CONTROL_FAST;

    #if DEBUG == 1
    if (player_control & CONTROL_UP)    d += sprintf(d, "UP ");
    if (player_control & CONTROL_DOWN)  d += sprintf(d, "DOWN ");
    if (player_control & CONTROL_LEFT)  d += sprintf(d, "LEFT ");
    if (player_control & CONTROL_RIGHT) d += sprintf(d, "RIGHT ");
    if (player_control & CONTROL_FAST)  d += sprintf(d, "FAST ");
    #endif
}

void processKeyEvents() // unused right now
{
    while (Keyboard.queue_head != Keyboard.queue_tail) 
    {
        KeyEvent_t event = Keyboard.queue[Keyboard.queue_head];
        Keyboard.queue_head++;

        // process event here
    }
}

void clearKeys()
{
    int i;
    for (i = 0; i < KB_ARRAY_LENGTH; i++)
        Keyboard.keystates[i] &= KEY_PRESSED_FLAG;
}

void input()
{
    processKeyEvents();
    playerControl();
    
    // F10 always exits, wherever you are
    if (KEY_WAS_HIT(KEY_F10))
        System.running = 0;

    clearKeys();
}
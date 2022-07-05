#include "Common.h"
#include "Structs.h"
#include "State.h"
#include "Game.h"
#include "Vectors.h"
#include "Keyb.h"

flags_t player_control = 0;
extern System_t System;
extern Keyboard_t Keyboard;
extern State States[];
extern uint8_t music_on;

int control_up = KEY_UP;
int control_down = KEY_DOWN;
int control_left = KEY_LEFT;
int control_right = KEY_RIGHT;
int control_fast = KEY_LSHIFT;
int control_strafe_l = KEY_A;
int control_strafe_r = KEY_D;
int control_strafe_mod = KEY_LALT;

void playerControl()
{
    #if DEBUG == 1
    char* d = debug[DEBUG_AICHASE];
    #endif

    if (KEY_IS_PRESSED(control_up))         player_control |= CONTROL_UP;
    else                                    player_control &= ~CONTROL_UP;

    if (KEY_IS_PRESSED(control_down))       player_control |= CONTROL_DOWN;
    else                                    player_control &= ~CONTROL_DOWN;

    if (KEY_IS_PRESSED(control_fast))       player_control |= CONTROL_FAST;
    else                                    player_control &= ~CONTROL_FAST;

    if (KEY_IS_PRESSED(control_strafe_mod))
    {
        player_control &= ~CONTROL_LEFT;
        player_control &= ~CONTROL_RIGHT;
        if (KEY_IS_PRESSED(control_left) || KEY_IS_PRESSED(control_strafe_l))
            player_control |= CONTROL_STRAFE_L;
        else
            player_control &= ~CONTROL_STRAFE_L;

        if (KEY_IS_PRESSED(control_right) || KEY_IS_PRESSED(control_strafe_r))
            player_control |= CONTROL_STRAFE_R;
        else
            player_control &= ~CONTROL_STRAFE_R;
    }
    else
    {
        if (KEY_IS_PRESSED(control_strafe_l))   player_control |= CONTROL_STRAFE_L;
        else                                    player_control &= ~CONTROL_STRAFE_L;

        if (KEY_IS_PRESSED(control_strafe_r))   player_control |= CONTROL_STRAFE_R;
        else                                    player_control &= ~CONTROL_STRAFE_R;

        if (KEY_IS_PRESSED(control_left))       player_control |= CONTROL_LEFT;
        else                                    player_control &= ~CONTROL_LEFT;

        if (KEY_IS_PRESSED(control_right))      player_control |= CONTROL_RIGHT;
        else                                    player_control &= ~CONTROL_RIGHT;
    }

    if (KEY_WAS_HIT(KEY_ESC))
    {
        popState();
        pushState(STATE_TITLE);
    }
    if (KEY_WAS_HIT(KEY_P))
    {
        pushState(STATE_PAUSE);
    }

    #if DEBUG == 1
    d[0] = '\0';
    if (player_control & CONTROL_UP)            d += sprintf(d, "UP ");
    if (player_control & CONTROL_DOWN)          d += sprintf(d, "DOWN ");
    if (player_control & CONTROL_LEFT)          d += sprintf(d, "LEFT ");
    if (player_control & CONTROL_RIGHT)         d += sprintf(d, "RIGHT ");
    if (player_control & CONTROL_FAST)          d += sprintf(d, "FAST ");
    if (player_control & CONTROL_STRAFE_L)      d += sprintf(d, "STRAFE-L ");
    if (player_control & CONTROL_STRAFE_R)      d += sprintf(d, "STRAFE-R ");
    if (player_control & CONTROL_STRAFE_MOD)    d += sprintf(d, "STRAFE ");
        
    #endif
}

void testButtons()
{
    if (KEY_WAS_HIT(KEY_DELETE))
        deleteLastObject();

    if (KEY_WAS_HIT(KEY_M))
    {
        if (music_on == TRUE)
            stopMusic();
        else
            playMusic("MUSIC/PELIMUSA.S3M");        
    }

    if (KEY_WAS_HIT(KEY_2))
        deleteObject(2);

    if (KEY_WAS_HIT(KEY_7))
        playSounds(1);
    if (KEY_WAS_HIT(KEY_8))
        playSounds(2);
    if (KEY_WAS_HIT(KEY_9))
        playSounds(3);
    if (KEY_WAS_HIT(KEY_PAGEUP))
        changeMusicVolume(VOLUME_UP);
    if (KEY_WAS_HIT(KEY_PAGEDOWN))
        changeMusicVolume(VOLUME_DOWN);
    if (KEY_WAS_HIT(KEY_PAD_PLUS))
        changeSFXVolume(VOLUME_UP);
    if (KEY_WAS_HIT(KEY_PAD_MINUS))
        changeSFXVolume(VOLUME_DOWN);
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

void gameInput()
{
    playerControl();

    #if DEBUG == 1
    testButtons();
    #endif
    
    // F10 always exits, wherever you are
    if (KEY_WAS_HIT(KEY_F10))
        System.running = 0;
}

void titleInput()
{
    if (KEY_IS_PRESSED(KEY_SPACEBAR))
    {
        popState();
        pushState(STATE_INGAME);
    }
    
    // F10 always exits, wherever you are
    if (KEY_WAS_HIT(KEY_F10))
        System.running = 0;
}

void pauseInput()
{
    if (KEY_WAS_HIT(KEY_P))
    {
        popState();
    }

    #if DEBUG == 1
    testButtons();
    #endif
    
    // F10 always exits, wherever you are
    if (KEY_WAS_HIT(KEY_F10))
        System.running = 0;
}
#include "Common.h"
#include "Structs.h"
#include "State.h"
#include "Game.h"
#include "Keyb.h"
#include "Action.h"
#include "LvlLoad.h"
#include "Menu.h"
#include "Text.h"
#include "Mouse.h"

flags_t player_control = 0;
extern System_t System;
extern Keyboard_t Keyboard;
extern State_t States[];
extern GameData_t Game;
extern uint8_t music_on;
extern Menu_t* current_menu;
extern Menu_t ingamemenu;
extern Weapon_t Weapons[];

int control_up = KEY_UP;
int control_down = KEY_DOWN;
int control_left = KEY_LEFT;
int control_right = KEY_RIGHT;
int control_fast = KEY_LSHIFT;
int control_strafe_l = KEY_A;
int control_strafe_r = KEY_D;
int control_strafe_mod = KEY_LALT;

void processKeyEvents(uint8_t text_input, TextInput_t* destination);

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
        pushState(STATE_MENU_INGAME);
    }
    if (KEY_WAS_HIT(KEY_P))
    {
        pushState(STATE_PAUSE);
    }
    if (KEY_IS_PRESSED(KEY_SPACEBAR))
        shootWeapon(PLAYER_ACTOR.primary_weapon_id, &PLAYER_ACTOR);

    if (KEY_WAS_HIT(KEY_E))
        useTile(PLAYER_ACTOR.position, PLAYER_ACTOR.direction);

    if (KEY_WAS_HIT(KEY_1))
        PLAYER_ACTOR.primary_weapon_id = WEAPON_FIST;
    if (KEY_WAS_HIT(KEY_2))
        PLAYER_ACTOR.primary_weapon_id = WEAPON_PISTOL;
    if (KEY_WAS_HIT(KEY_3))
        PLAYER_ACTOR.primary_weapon_id = WEAPON_SHOTGUN;
    if (KEY_WAS_HIT(KEY_4))
        PLAYER_ACTOR.primary_weapon_id = WEAPON_CHAINGUN;
    if (KEY_WAS_HIT(KEY_5))
        PLAYER_ACTOR.primary_weapon_id = WEAPON_ROCKET;

    #if DEBUG == 1
    if (System.debug_mode == TRUE)
    {
        d[0] = '\0';
        if (player_control & CONTROL_UP)            d += sprintf(d, "UP ");
        if (player_control & CONTROL_DOWN)          d += sprintf(d, "DOWN ");
        if (player_control & CONTROL_LEFT)          d += sprintf(d, "LEFT ");
        if (player_control & CONTROL_RIGHT)         d += sprintf(d, "RIGHT ");
        if (player_control & CONTROL_FAST)          d += sprintf(d, "FAST ");
        if (player_control & CONTROL_STRAFE_L)      d += sprintf(d, "STRAFE-L ");
        if (player_control & CONTROL_STRAFE_R)      d += sprintf(d, "STRAFE-R ");
        if (player_control & CONTROL_STRAFE_MOD)    d += sprintf(d, "STRAFE ");
    }
        
    #endif
}

void testButtons()
{
    if (KEY_WAS_HIT(KEY_DELETE))
        deleteLastActor();

    if (KEY_WAS_HIT(KEY_M))
    {
        if (music_on == TRUE)
            stopMusic();
        else
            playMusic("MUSIC/PELIMUSA.S3M");        
    }

    if (KEY_WAS_HIT(KEY_K))
        PLAYER_ACTOR.health = 10;

    if (KEY_WAS_HIT(KEY_PAGEUP))
        changeMusicVolume(VOLUME_UP);
    if (KEY_WAS_HIT(KEY_PAGEDOWN))
        changeMusicVolume(VOLUME_DOWN);
    if (KEY_WAS_HIT(KEY_PAD_PLUS))
        changeSFXVolume(VOLUME_UP);
    if (KEY_WAS_HIT(KEY_PAD_MINUS))
        changeSFXVolume(VOLUME_DOWN);
    if (KEY_WAS_HIT(KEY_F5))
    {
        quickSave();
    }
    if (KEY_WAS_HIT(KEY_F6))
    {
        quickLoad();
    }
    if (KEY_WAS_HIT(KEY_Q) && KEY_WAS_HIT(KEY_BACKSPACE))
    {
        if (System.debug_mode == FALSE)
            System.debug_mode = TRUE;
        else
            System.debug_mode = FALSE;
    }
}

void processKeyEvents(uint8_t text_input, TextInput_t* destination)
{
    while (Keyboard.queue_head != Keyboard.queue_tail) 
    {
        KeyEvent_t event = Keyboard.queue[Keyboard.queue_head];
        if (text_input == TRUE)
        {
            handleTextInput(&event, destination);
        }

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

    testButtons();

    handleMouseInput();
    
    if (KEY_IS_PRESSED(KEY_H))
    {
        playerMouseAim();
        showMouse();
    }
    
    // F10 always exits, wherever you are
    if (KEY_WAS_HIT(KEY_F10))
        System.running = 0;
}

void titleInput()
{
    if (KEY_IS_PRESSED(KEY_SPACEBAR))
    {
        popState();
        pushState(STATE_MENU_MAIN);
    }
    
    // F10 always exits, wherever you are
    if (KEY_WAS_HIT(KEY_F10))
        System.running = 0;
}

void menuInput()
{
    if (KEY_WAS_HIT(KEY_ENTER))
        current_menu->options[current_menu->cursor_loc].action();

    else if (KEY_WAS_HIT(KEY_UP))
    {
        cursorUp();
    }

    else if (KEY_WAS_HIT(KEY_DOWN))
    {
        cursorDown();
    }

    if (KEY_WAS_HIT(KEY_ESC))
    {
        menuMain();
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

void ingameMenuInput()
{
    if (KEY_WAS_HIT(KEY_ENTER))
        current_menu->options[current_menu->cursor_loc].action();

    else if (KEY_WAS_HIT(KEY_UP))
    {
        cursorUp();
    }

    else if (KEY_WAS_HIT(KEY_DOWN))
    {
        cursorDown();
    }

    if (KEY_WAS_HIT(KEY_ESC))
    {
        if (current_menu != &ingamemenu)
            current_menu = &ingamemenu;
        else
            popState();
    }
    // F10 always exits, wherever you are
    if (KEY_WAS_HIT(KEY_F10))
        System.running = 0;
}
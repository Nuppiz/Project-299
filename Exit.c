#include "Game.h"
#include "LvlLoad.h"
#include "Loadgfx.h"
#include "Draw.h"
#include "Filech.h"

/* State exit functions */

extern State_t States[];
extern System_t System;
extern uint8_t music_on;

void titleExit()
{
    // do nothing atm
}

void menuExit()
{
    
}

void gameExit()
{
    if (music_on == TRUE)
        stopMusic();
    freeAllEntities();
    freeAllTextures();
    emptyCorpseArray();
}

void pauseExit()
{
    System.paused = FALSE;
    States[STATE_INGAME].flags |= STATE_ENABLE_UPDATE;
}

void ingameMenuExit()
{
    System.paused = FALSE;
    States[STATE_INGAME].flags |= STATE_ENABLE_UPDATE;
}
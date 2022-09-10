#include "Game.h"
#include "LvlLoad.h"
#include "Loadgfx.h"
#include "Draw.h"
#include "Filech.h"

/* State exit functions */

extern State_t States[];
extern uint8_t music_on;

void titleExit()
{
    // do nothing atm
}

void gameExit()
{
    if (music_on == TRUE)
        stopMusic();
    freeAllEntities();
    freeAllTextures();
    emptyCorpseArray();
    deleteAutosaves();
}

void pauseExit()
{
    States[STATE_INGAME].flags |= STATE_ENABLE_UPDATE;
}
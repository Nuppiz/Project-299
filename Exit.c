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
    if (checkFileExists("SAVES/CURRENT/CRTSTATE.SAV"))
    {
        remove("SAVES/CURRENT/CRTSTATE.SAV");
    }
    if (checkFileExists("SAVES/CURRENT/LEVEL1.SAV"))
    {
        remove("SAVES/CURRENT/LEVEL1.SAV");
    }
    if (checkFileExists("SAVES/CURRENT/LEVEL2.SAV"))
    {
        remove("SAVES/CURRENT/LEVEL2.SAV");
    }
    if (checkFileExists("SAVES/CURRENT/LEVEL3.SAV"))
    {
        remove("SAVES/CURRENT/LEVEL3.SAV");
    }
    if (checkFileExists("SAVES/CURRENT/LEVEL4.SAV"))
    {
        remove("SAVES/CURRENT/LEVEL4.SAV");
    }
    if (checkFileExists("SAVES/CURRENT/LEVEL5.SAV"))
    {
        remove("SAVES/CURRENT/LEVEL5.SAV");
    }
}

void pauseExit()
{
    States[STATE_INGAME].flags |= STATE_ENABLE_UPDATE;
}
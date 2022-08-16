#include "Game.h"
#include "LvlLoad.h"
#include "Loadgfx.h"
#include "Draw.h"
#include "Filech.h"

/* State exit functions */

extern State_t States[];
extern GameData_t Game;
extern uint8_t music_on;
extern Entity_t Entities[];
extern Texture_t* Textures;
extern Corpse_t Corpses[];

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
}

void pauseExit()
{
    States[STATE_INGAME].flags |= STATE_ENABLE_UPDATE;
}
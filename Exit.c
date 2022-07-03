#include "Game.h"

/* State exit functions */

extern State States[];
extern GameData_t Game;

void titleExit()
{
    States[STATE_TITLE].flags &= ~ STATE_ENABLE_INPUT;
    States[STATE_TITLE].flags &= ~ STATE_ENABLE_DRAW;
}

void gameExit()
{
    memset(Game.Map.tiles, 0, Game.Map.width * Game.Map.height);
    free(Game.Map.tiles);
    memset(Game.Map.collision, 0, Game.Map.width * Game.Map.height);
    free(Game.Map.collision);
    Game.object_count = 0;
    memset(Game.Objects, 0, Game.object_capacity * sizeof(Object_t));
    free(Game.Objects);
    Game.object_capacity = 0;
    memset(Game.ObjectsById, 0, Game.id_capacity * sizeof(void*));
    free(Game.ObjectsById);
    Game.id_capacity = 0;
    States[STATE_TITLE].flags &= ~ STATE_ENABLE_INPUT;
    States[STATE_TITLE].flags &= ~ STATE_ENABLE_UPDATE;
    States[STATE_TITLE].flags &= ~ STATE_ENABLE_DRAW;
}

void pauseExit()
{
    States[STATE_PAUSE].flags &= ~ STATE_ENABLE_INPUT;
    States[STATE_PAUSE].flags &= ~ STATE_ENABLE_DRAW;
    States[STATE_INGAME].flags |= STATE_ENABLE_INPUT;
}
#include "Game.h"

/* State exit functions */

extern State States[];
extern GameData_t Game;
extern uint8_t music_on;

void titleExit()
{
    // do nothing atm
}

void gameExit()
{
    if (music_on == TRUE)
        stopMusic();
    memset(Game.Map.tilemap, 0, Game.Map.width * Game.Map.height);
    free(Game.Map.tilemap);
    Game.object_count = 0;
    memset(Game.Objects, 0, Game.object_capacity * sizeof(Object_t));
    free(Game.Objects);
    Game.object_capacity = 0;
    memset(Game.ObjectsById, 0, Game.id_capacity * sizeof(void*));
    free(Game.ObjectsById);
    Game.id_capacity = 0;
}

void pauseExit()
{
    States[STATE_INGAME].flags |= STATE_ENABLE_UPDATE;
}
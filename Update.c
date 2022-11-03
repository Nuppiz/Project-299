#include "Action.h"
#include "AI.h"
#include "Movecoll.h"

/* State logic, status etc. update functions */

extern GameData_t Game;

void titleUpdate()
{
    // do nothing atm
}

void menuUpdate()
{
    // do nothing atm
}

void gameUpdate()
{
    AILoop();
    physics();
    checkForItem();
    checkForInteractive();
    actorDeathLoop();
    entityLoop();
}

void pauseUpdate()
{
    // do nothing atm
}

void ingameMenuUpdate()
{
    // do nothing atm
}
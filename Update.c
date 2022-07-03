#include "AI.h"
#include "Movecoll.h"

/* State logic, status etc. update functions */

void titleUpdate()
{
    // do nothing atm
}

void gameUpdate()
{
    AILoop();
    physics();
}

void pauseUpdate()
{
    // do nothing atm
}
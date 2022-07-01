#include "AI.h"
#include "Movecoll.h"

/* Logic, status etc. update functions */

void gameUpdate()
{
    AILoop();
    physics();
}

void titleUpdate()
{
    // do nothing atm
}
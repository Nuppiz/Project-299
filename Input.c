#include "Common.h"
#include "Defines.h"
#include "Structs.h"
#include "Vectors.h"
#include "Keyb.h"

extern int running;
extern int heading;
extern double radians;
extern Object object_array [];

void controlIngame()
{
    player.direction.x = cos(radians); // calculate directional x-vector
    player.direction.y = sin(radians); // calculate directional y-vector
    
    if (KEY_IS_PRESSED(KEY_UP) && player.magnitude <= MAX_SPEED)
    {

        player.velocity.x += player.direction.x * ACCELERATION_RATE;
        player.velocity.y += player.direction.y * ACCELERATION_RATE;
        // calculate the player's current movement speed
        //player.magnitude = getVec2Length(player.velocity);
    }
    else if (KEY_IS_PRESSED(KEY_DOWN) && player.magnitude <= MAX_SPEED)
    {
        player.velocity.x -= player.direction.x * ACCELERATION_RATE;
        player.velocity.y -= player.direction.y * ACCELERATION_RATE;
        // calculate the player's current movement speed
        //player.magnitude = getVec2Length(player.velocity);
    }
    else if (player.magnitude > 0)
    {
        player.velocity.x /= DRAG;
        player.velocity.y /= DRAG;
        //player.magnitude = getVec2Length(player.velocity);
    }

    if (KEY_IS_PRESSED(KEY_LEFT))
    {
        heading -= TURN_RATE;
        if (heading < 0)
            heading = 360;
    }
    if (KEY_IS_PRESSED(KEY_RIGHT))
    {
        heading += TURN_RATE;
        if (heading > 360)
            heading = 0;
    }
}

void processInput()
{
    getKeyboard();

    controlIngame();
    
    // F10 always exits, wherever you are
    if (KEY_WAS_HIT(KEY_F10))
        running = 0;

    clearKeys();
}
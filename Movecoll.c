#include "Common.h"
#include "Structs.h"
#include "Vectors.h"
#include "Keyb.h"
#include "Sound.h"

/* Actor_t movement and collision detection */

extern GameData_t Game;
extern Entity_t Entities[];

void updateGridLoc(Actor_t* actor) // actor's location on the grid
{   
    // calculated by dividing the actor's x/y location by square size
    actor->grid_loc.x = actor->position.x / SQUARE_SIZE;
    actor->grid_loc.y = actor->position.y / SQUARE_SIZE;
}

int getTileCollision(Vec2 pos)
{
    int object_tile; // tile which the object is on (or attempting to be), i.e. array index number from grid_array
    uint8_t tile_obstacle;
    
    // calculate current grid position
    pos.x /= SQUARE_SIZE;
    pos.y /= SQUARE_SIZE;
    
    // check which grid_array index it corresponds to
    object_tile = (int)pos.y * Game.Map.width + (int)pos.x;
    
    tile_obstacle = Game.Map.tilemap[object_tile].obstacle; // check collision flag at that index
    
    return tile_obstacle; // return said data
}

int getTileBulletBlock(Vec2 pos)
{
    int object_tile; // tile which the object is on (or attempting to be), i.e. array index number from grid_array
    uint8_t tile_bullet;
    
    // calculate current grid position
    pos.x /= SQUARE_SIZE;
    pos.y /= SQUARE_SIZE;
    
    // check which grid_array index it corresponds to
    object_tile = (int)pos.y * Game.Map.width + (int)pos.x;
    
    tile_bullet = Game.Map.tilemap[object_tile].block_bullets; // check bullet block flag at that index
    
    return tile_bullet; // return said data
}

int checkForPortal(Vec2_int grid_location)
{
    int actor_tile; // tile which the actor is on (or attempting to be), i.e. array index number from grid_array
    
    // check which grid_array index it corresponds to
    actor_tile = grid_location.y * Game.Map.width + grid_location.x;
    
    if (Game.Map.tilemap[actor_tile].is_entity == 1 && Entities[Game.Map.tilemap[actor_tile].entity_value].type == ENT_PORTAL) // check if there's a portal at that tile
        return TRUE;
    
    return FALSE;
}

Vec2 moveFromPortal(Vec2 pos)
{
    Vec2 temp_position;
    Vec2 new_position;

    temp_position.x = pos.x;
    temp_position.y = pos.y - SQUARE_SIZE;
    if (getTileCollision(temp_position) == FALSE)
    {
        new_position = temp_position;
        return new_position;
    }
    temp_position.x = pos.x + SQUARE_SIZE;
    temp_position.y = pos.y;
    if (getTileCollision(temp_position) == FALSE)
    {
        new_position = temp_position;
        return new_position;
    }
    temp_position.x = pos.x;
    temp_position.y = pos.y + SQUARE_SIZE;
    if (getTileCollision(temp_position) == FALSE)
    {
        new_position = temp_position;
        return new_position;
    }
    temp_position.x = pos.x - SQUARE_SIZE;
    temp_position.y = pos.y;
    if (getTileCollision(temp_position) == FALSE)
    {
        new_position = temp_position;
        return new_position;
    }
    // can't move, return original position
    return pos;
}

void edgeDetectActor(Actor_t* actor)
{
    if (actor->position.x - actor->radius <= 0) // left edge
    {
        actor->position.x = 0 + actor->radius;
        actor->velocity.x = 0.0;
    }
    else if (actor->position.x + actor->radius >= (SCREEN_WIDTH - 1)) // right edge
    {
        actor->position.x = (SCREEN_WIDTH - 1) - actor->radius;
        actor->velocity.x = 0.0;
    }
    
    if (actor->position.y - actor->radius <= 0) // top edge
    {
        actor->position.y = 0 + actor->radius;
        actor->velocity.y = 0.0;
    }
    else if (actor->position.y + actor->radius >= (SCREEN_HEIGHT - 1)) // bottom edge
    {
        actor->position.y = (SCREEN_HEIGHT - 1) - actor->radius;
        actor->velocity.y = 0.0;
    }
}

void edgeDetectAllActors()
{
    int i = 0;
    while (i < Game.actor_count)
    {
        edgeDetectActor(&Game.Actors[i]);
        i++;
    }
}

void controlActor(Actor_t* actor)
{
    float max_speed, strafe_speed;
    double turn_rate;

    max_speed = ((actor->control & CONTROL_FAST)) ? RUN_SPEED : WALK_SPEED;
    turn_rate = ((actor->control & CONTROL_FAST)) ? FAST_TURN_RATE : TURN_RATE;
    strafe_speed = ((actor->control & CONTROL_FAST)) ? FAST_STRAFE_SPEED : STRAFE_SPEED;

    actor->direction = getDirVec2(actor->angle); // calculate directional vector
    
    if ((actor->control & CONTROL_UP) && actor->magnitude <= max_speed)
    {
        actor->velocity.x += actor->direction.x * ACCELERATION_RATE;
        actor->velocity.y += actor->direction.y * ACCELERATION_RATE;
    }
    else if ((actor->control & CONTROL_DOWN) && actor->magnitude <= max_speed)
    {
        actor->velocity.x -= actor->direction.x * ACCELERATION_RATE;
        actor->velocity.y -= actor->direction.y * ACCELERATION_RATE;
    }
    else if (actor->magnitude > 0)
    {
        if (actor->magnitude < STOP_SPEED)
        {
            actor->velocity.x = 0;
            actor->velocity.y = 0;
        }
        else
        {
            actor->velocity.x /= DECELERATION_RATE;
            actor->velocity.y /= DECELERATION_RATE;
        }
    }

    if ((actor->control & CONTROL_LEFT))
    {
        actor->angle -= turn_rate;
        if (actor->angle < 0)
            actor->angle = RAD_360;
    }
    if ((actor->control & CONTROL_RIGHT))
    {
        actor->angle += turn_rate;
        if (actor->angle > RAD_360)
            actor->angle = 0;
    }

    if ((actor->control & CONTROL_STRAFE_L) && actor->magnitude <= max_speed)
    {
        actor->velocity.x += actor->direction.y * strafe_speed;
        actor->velocity.y += -actor->direction.x * strafe_speed;
    }
    if ((actor->control & CONTROL_STRAFE_R) && actor->magnitude <= max_speed)
    {
        actor->velocity.x += -actor->direction.y * strafe_speed;
        actor->velocity.y += actor->direction.x * strafe_speed;
    }
}

void controlAllActors()
{
    int i = 0;
    
    // copy control variable from Input.c to the player actor's control variable
    // in this way, completely separating input handling and physics with a single-variable "abstraction layer"
    extern flags_t player_control;
    PLAYER_ACTOR.control = player_control;

    while (i < Game.actor_count)
    {
        controlActor(&Game.Actors[i]);
        i++;
    }
}

void moveActor(Actor_t* actor, Vec2 movement)
{
    // collision box around the actor
    Vec2 test_point_a;
    Vec2 test_point_b;
    
    updateGridLoc(actor);
    
    if (movement.x > 0) // if moving to the right
    {
        actor->position.x += movement.x; // temporarily move the actor
        // test_point_a = top right corner
        test_point_a.x = actor->position.x + actor->radius;
        test_point_a.y = actor->position.y - actor->radius;
        // test_point_b = bottom right corner
        test_point_b.x = actor->position.x + actor->radius;
        test_point_b.y = actor->position.y + actor->radius;
        
        // if the movement would result in the actor moving inside of a wall...
        if (getTileCollision(test_point_a) == TRUE || getTileCollision(test_point_b) == TRUE)
        {
            // ...cancel movement and set velocity to 0
            actor->position.x = (actor->grid_loc.x + 1) * SQUARE_SIZE - actor->radius - 1;
            actor->velocity.x = 0.0;
        }
    }
    else if (movement.x < 0) // if moving to the left
    {
        actor->position.x += movement.x;
        // test_point_a = top left corner
        test_point_a.x = actor->position.x - actor->radius;
        test_point_a.y = actor->position.y - actor->radius;
        // test_point_b = bottom left corner
        test_point_b.x = actor->position.x - actor->radius;
        test_point_b.y = actor->position.y + actor->radius;
        
        if (getTileCollision(test_point_a) == TRUE || getTileCollision(test_point_b) == TRUE)
        {
            actor->position.x = actor->grid_loc.x * SQUARE_SIZE + actor->radius;
            actor->velocity.x = 0.0;
        }
    }

    if (movement.y < 0) // if moving towards the top
    {
        actor->position.y += movement.y;
        // test_point_a = top right corner
        test_point_a.x = actor->position.x + actor->radius;
        test_point_a.y = actor->position.y - actor->radius;
        // test_point_b = top left corner
        test_point_b.x = actor->position.x - actor->radius;
        test_point_b.y = actor->position.y - actor->radius;
        
        if (getTileCollision(test_point_a) == TRUE || getTileCollision(test_point_b) == TRUE)
        {
            actor->position.y = actor->grid_loc.y * SQUARE_SIZE + actor->radius;
            actor->velocity.y = 0.0;
        }
    }
    else if (movement.y > 0) // if moving towards the bottom
    {
        actor->position.y += movement.y;
        // test_point_a = bottom right corner
        test_point_a.x = actor->position.x + actor->radius;
        test_point_a.y = actor->position.y + actor->radius;
        // test_point_b = bottom left corner
        test_point_b.x = actor->position.x - actor->radius;
        test_point_b.y = actor->position.y + actor->radius;
        
        if (getTileCollision(test_point_a) == TRUE || getTileCollision(test_point_b) == TRUE)
        {
            actor->position.y = (actor->grid_loc.y + 1) * SQUARE_SIZE - actor->radius - 1;
            actor->velocity.y = 0.0;
        }
    }
    updateGridLoc(actor);
}

void moveAllActors()
{
    int i = 0;
    
    // iterate through the actor array
    while (i < Game.actor_count)
    {
        moveActor(&Game.Actors[i], Game.Actors[i].velocity);
        Game.Actors[i].magnitude = getVec2Length(Game.Actors[i].velocity);
        i++;
    }
}

void collideTwoActors(Actor_t* actor_a, Actor_t* actor_b)
{
    float distance_x;
    float distance_y;
    float distance_squared;
    float collision_depth;
    int radii_squared = (actor_a->radius + actor_b->radius) * (actor_a->radius + actor_b->radius);
    Vec2 u; // how much each actor moves in case of a collision
    
    distance_x = actor_a->position.x - actor_b->position.x;  // x-distance between the two actors
    distance_y = actor_a->position.y - actor_b->position.y;  // y-distance between the two actors
    
    // squared distance between the two actors (spares an expensive sqrt operation)
    distance_squared = (distance_x * distance_x) + (distance_y * distance_y);
    
    // if distance is less than combined radiuses squared
    if (distance_squared < radii_squared)
    {
        // calculate how much the actors are "inside" each other
        collision_depth = radii_squared - distance_squared;
        
        // each actor is moved for half of that
        u.x = (distance_x / distance_squared) * (collision_depth / 2);
        u.y = (distance_y / distance_squared) * (collision_depth / 2);
        
        // first actor gets the values as is...
        moveActor(actor_a, u);
        
        // ...and for the second actor they are flipped
        u.x = -u.x;
        u.y = -u.y;
        moveActor(actor_b, u);
    }
}

void collideAllActors()
{
    int x;
    int i;
    
    // iterate through each actor pair to see if they collide
    for (i = 0; i < Game.actor_count; i++)
    {
        for (x = i; x < Game.actor_count-1; x++)
        {
            collideTwoActors(&Game.Actors[i], &Game.Actors[x+1]);
        }
    }
}

void physics()
{
    controlAllActors();
    moveAllActors();
    collideAllActors();

    #if DEBUG == 1
    sprintf(debug[DEBUG_VELOCITY], "V.X: %f\nV.Y %f", PLAYER_ACTOR.velocity.x, PLAYER_ACTOR.velocity.y);
    #endif
}
#include "Common.h"
#include "Movecoll.h"
#include "Structs.h"
#include "Vectors.h"
#include "Action.h"

/* AI functions */

extern System_t System;
extern GameData_t Game;

int testLineOfSight(Vec2 p, Vec2 target)
{
    Vec2 v = getVec2(p, target);
    float distance = normalizeAndGetLength(&v);
    float los = 0;
    
    for (los = 0.0; los < distance; los += LOS_STEP)
    {        
        //SET_PIXEL((int)p.x - (CAMERA_OFFSET * SQUARE_SIZE), (int)p.y, COLOUR_WHITE);
        
        if (getTileCollision(p) == TRUE)
            return OUT_OF_SIGHT;
        
        p.x += (v.x * LOS_STEP);
        p.y += (v.y * LOS_STEP);
    }
    return IN_SIGHT;
}

int testFieldOfView(Vec2 origin, Vec2 direction, Vec2 target)
{
    Vec2 origin_to_target = getVec2(origin, target);
    float distance_sq = getVec2LengthSquared(origin_to_target);
    float angle;
    
    if (distance_sq < CHASE_DISTANCE_SQ)
    {
        angle = getVec2Angle2(origin_to_target, direction);
        
        if (angle > VIEW_ANGLE_COS && testLineOfSight(origin, target) == IN_SIGHT)
        {
             return IN_SIGHT;
        }
    }

    return OUT_OF_SIGHT;
}

int whichSide(Vec2 object_direction, Vec2 object_to_target)
{
    if (crossVec2(object_to_target, object_direction) >= 0)
        return LEFT_SIDE;
    
    return RIGHT_SIDE;
}

void turnTowards(Object_t* object, Vec2 target)
{
    Vec2 object_to_target = getVec2(object->position, target);
    int side            = whichSide(object->direction, object_to_target);

    if (side == LEFT_SIDE)
    {
        (object->control) |= CONTROL_LEFT;
        (object->control) &= ~CONTROL_RIGHT;
    }
    else if (side == RIGHT_SIDE)
    {
        (object->control) |= CONTROL_RIGHT;
        (object->control) &= ~CONTROL_LEFT;
    }
}

void chaseTarget(Object_t* chaser)
{
    Vec2 object_to_target;
    float distance_sq;
    float cross_product;

    #if DEBUG == 1
    char* d;
    #endif

    
    object_to_target = getVec2(chaser->position, chaser->move_target);
    distance_sq      = getVec2LengthSquared(object_to_target);
    cross_product    = crossVec2(object_to_target, chaser->direction);
    
    if (distance_sq <= MIN_CHASE_DISTANCE_SQ)
        (chaser->control) &= ~CONTROL_UP;
    else
        (chaser->control) |= CONTROL_UP;

    // set direction based on cross product

    if (cross_product > TURN_THRESHOLD)
    {
        (chaser->control) |= CONTROL_LEFT;
        (chaser->control) &= ~CONTROL_RIGHT;
    }
    else if (cross_product < -TURN_THRESHOLD)
    {
        (chaser->control) |= CONTROL_RIGHT;
        (chaser->control) &= ~CONTROL_LEFT;
    }
    else
    {
        (chaser->control) &= ~CONTROL_LEFT;
        (chaser->control) &= ~CONTROL_RIGHT;
    }

    #if DEBUG == 1
    d = debug[DEBUG_AICHASE];
    d += sprintf(d, "DISTANCE: %.2f\n", sqrt(distance_sq));
    d += sprintf(d, "ANGLE: %.2lf\n", chaser->angle);
    d += sprintf(d, "CP: %.2f\n", cross_product);
    d += sprintf(d, "DIR-X: %.2f\n", chaser->direction.x);
    d += sprintf(d, "DIR-Y: %.2f", chaser->direction.y);
    #endif
}

void think(Object_t* obj)
{
    if (Game.ObjectsById[obj->target_id] != UINT16_MAX)
    {
        obj->move_target = Game.Objects[Game.ObjectsById[obj->target_id]].position;
        // check to see if target in sight; set mode to chase if yes, and timer to 100 ticks
        if (testFieldOfView(obj->position, obj->direction, obj->move_target) == IN_SIGHT)
        {
            obj->ai_mode = AI_CHASE;
            obj->ai_timer = CHASE_TIMEOUT;
        }
    }
    else
    {
        Game.ObjectsById[obj->target_id] = UINT16_MAX;
        obj->ai_mode = AI_IDLE;
        obj->control = 0; // same as clearing all bits
    }
}

void act(Object_t* obj)
{
    if (obj->ai_mode == AI_IDLE)
    {
        if (obj->ai_timer == 0)
        {
            if (!(obj->control & (CONTROL_LEFT|CONTROL_RIGHT)))
            {
                obj->ai_timer = 5 + rand() % 30;
                if (rand() % 2)
                    (obj->control) |= CONTROL_LEFT;
                else
                    (obj->control) |= CONTROL_RIGHT;
            }
            else
            {
                obj->ai_timer = 10 + rand() % 200;
                obj->control = 0;
            }
        }
        else
            obj->ai_timer--;
    }
    else if (obj->ai_mode == AI_CHASE)
    {
        if (obj->ai_timer > 0)
        {
            chaseTarget(obj);
            if (obj->last_shot + obj->shot_delay < System.ticks);
            {
                shootWeapon(obj);
            }
            obj->ai_timer--;

            if (Game.ObjectsById[obj->target_id] == UINT16_MAX) // if target is deleted
            {
                obj->target_id = UINT16_MAX; // remove chase target
                obj->ai_mode = AI_IDLE;
                obj->control = 0; // same as clearing all bits
            }
        }
        else
        {
            obj->ai_mode = AI_IDLE;
            obj->control = 0; // same as clearing all bits
        }
    }
}

void AILoop()
{
    int i;
    if (!(System.ticks % 10)) // only think every 10 tics
    {
        for (i = 1; i < Game.object_count; i++)
        {
            if (Game.Objects[i].ai_mode != AI_NONE)
            {
                think(&Game.Objects[i]);
                act(&Game.Objects[i]);
            }
        }
    }
    else
    {
        for (i = 1; i < Game.object_count; i++)
            if (Game.Objects[i].ai_mode != AI_NONE)
                act(&Game.Objects[i]);
    }
}
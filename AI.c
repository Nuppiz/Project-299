#include "Common.h"
#include "Movecoll.h"
#include "Structs.h"
#include "Vectors.h"

/* AI functions */

extern GameData_t Game;

int testLineOfSight(Vec2 p, Vec2 target)
{
    Vec2 v = getVec2(p, target);
    float distance = normalizeAndGetLength(&v);
    float los = 0;
    
    for (los = 0.0; los < distance; los += LOS_STEP)
    {        
        //SET_PIXEL((int)p.x - (CAMERA_OFFSET * SQUARE_SIZE), (int)p.y, COLOUR_WHITE);
        
        if (getTileType(p) == WALL)
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

    #if DEBUG == 1
    char* d;
    #endif
    
    if (distance_sq < CHASE_DISTANCE_SQ)
    {
        angle = getVec2Angle2(origin_to_target, direction);
        
        if (angle > VIEW_ANGLE_COS && testLineOfSight(origin, target) == IN_SIGHT)
        {
             return IN_SIGHT;
        }
    }
    #if DEBUG == 1
    d = debug[DEBUG_TESTFOV];
    d += sprintf(d, "DISTANCE: %.2f\n", sqrt(distance_sq));
    #endif

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
        setBit(object->control, CONTROL_LEFT);
        clearBit(object->control, CONTROL_RIGHT);
    }
    else if (side == RIGHT_SIDE)
    {
        setBit(object->control, CONTROL_RIGHT);
        clearBit(object->control, CONTROL_LEFT);
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
        clearBit(chaser->control, CONTROL_UP);
    else
        setBit(chaser->control, CONTROL_UP);

    // set direction based on cross product

    if (cross_product > TURN_THRESHOLD)
    {
        setBit(chaser->control, CONTROL_LEFT);
        clearBit(chaser->control, CONTROL_RIGHT);
    }
    else if (cross_product < -TURN_THRESHOLD)
    {
        setBit(chaser->control, CONTROL_RIGHT);
        clearBit(chaser->control, CONTROL_LEFT);
    }
    else
    {
        clearBit(chaser->control, CONTROL_LEFT);
        clearBit(chaser->control, CONTROL_RIGHT);
    }

    #if DEBUG == 1
    d = debug[DEBUG_AICHASE];
    d += sprintf(d, "DISTANCE: %.2f\n", sqrt(distance_sq));
    d += sprintf(d, "ANGLE: %.2lf\n", chaser->angle);
    d += sprintf(d, "CP: %.2f\n", cross_product);
    d += sprintf(d, "DIR-X: %.2f\n", chaser->direction.x);
    d += sprintf(d, "DIR-Y: %.2f", chaser->direction.x);
    #endif
}

void think(Object_t* obj)
{
    if (Game.ObjectsById[obj->target_id] != NULL)
    {
        obj->move_target = Game.ObjectsById[obj->target_id]->position;
        // check to see if target in sight; set mode to chase if yes, and timer to 100 ticks
        if (testFieldOfView(obj->position, obj->direction, obj->move_target) == IN_SIGHT)
        {
            obj->ai_mode = AI_CHASE;
            obj->ai_timer = CHASE_TIMEOUT;
        }
    }
    else
    {
        obj->ai_mode = AI_IDLE;
    }

    if (obj->ai_mode == AI_CHASE) // to separate "do" function?
    {   // "think" infrequently (every 10 tics), but do all the time ... ?
        if (obj->ai_timer > 0)
        {
            chaseTarget(obj);
            obj->ai_timer--;
        }
        else
        {
            obj->ai_mode = AI_IDLE;
            clearBit(obj->control, CONTROL_UP);
            clearBit(obj->control, CONTROL_LEFT);
            clearBit(obj->control, CONTROL_RIGHT);
        }
    }
}

void AILoop()
{
    int i = 1;
    
    while (i < Game.object_count)
    {
        think(&Game.Objects[i]);
        i++;
    }
}
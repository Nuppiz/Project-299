#include "Common.h"
#include "Movecoll.h"
#include "Structs.h"
#include "Vectors.h"

/* AI functions */

extern uint8_t far screen_buf [];
extern Object_t Objects [];
extern Map_t map1;

int testLineOfSight(Vec2 origin, Vec2 target)
{
    Vec2 origin_to_target = getVec2(origin, target);
    Vec2 p = origin; // test point
    Vec2 direction = normalizeVec2(origin_to_target);
    float distance = getVec2Length(origin_to_target);
    float los;
    
    for (los = 0.0; los < distance; los += LOS_STEP)
    {        
        //SET_PIXEL((int)p.x - (CAMERA_OFFSET * SQUARE_SIZE), (int)p.y, COLOUR_WHITE);
        
        if (getTileType(p) == WALL)
            return OUT_OF_SIGHT;
        
        p.x += (direction.x * LOS_STEP);
        p.y += (direction.y * LOS_STEP);
    }
    return IN_SIGHT;
}

int testFieldOfView(Vec2 origin, Vec2 direction, Vec2 target)
{
    Vec2 origin_to_target = getVec2(origin, target);
    float distance = getVec2Length(origin_to_target);
    float angle;
    
    if (distance < CHASE_DISTANCE)
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
    Vec2 ObjectToTarget = getVec2(object->position, target);
    float distance      = getVec2Length(ObjectToTarget);
    int side            = whichSide(object->direction, ObjectToTarget);

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
    Vec2 ObjectToTarget = getVec2(chaser->position, *chaser->ai_target);
    float distance      = getVec2Length(ObjectToTarget);
    float cross_product = crossVec2(ObjectToTarget, chaser->direction);

    #if DEBUG == 1
    char* d = debug[DEBUG_AICHASE];
    d += sprintf(d, "DISTANCE: %.2f\n", distance);
    d += sprintf(d, "ANGLE: %.2lf\n", chaser->angle);
    d += sprintf(d, "CP: %.2f\n", cross_product);
    d += sprintf(d, "DIR-X: %.2f\n", chaser->direction.x);
    d += sprintf(d, "DIR-Y: %.2f", chaser->direction.x);
    #endif
    
    if (distance <= MIN_CHASE_DISTANCE)
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
}

void think(Object_t* obj)
{
    // check to see if target in sight; set mode to chase if yes, and timer to 100 ticks
    if (testFieldOfView(obj->position, obj->direction, *obj->ai_target) == IN_SIGHT)
    {
        obj->ai_mode = CHASE_TARGET;
        obj->ai_timer = CHASE_TIMEOUT;
    }

    if (obj->ai_mode == CHASE_TARGET)
    {
        if (obj->ai_timer > 0)
        {
            chaseTarget(obj);
            obj->ai_timer--;
        }
        else
        {
            obj->ai_mode = IDLE;
            clearBit(obj->control, CONTROL_UP);
            clearBit(obj->control, CONTROL_LEFT);
            clearBit(obj->control, CONTROL_RIGHT);
        }
    }
}

void AILoop()
{
    int i = 1;
    
    while (i < NUM_OBJECTS)
    {
        think(&Objects[i]);
        i++;
    }
}
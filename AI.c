#include "Common.h"
#include "Defines.h"
#include "Enums.h"
#include "Movecoll.h"
#include "Structs.h"
#include "Vectors.h"

/* AI functions */

extern uint8_t far screen_buf [];
extern Object object_array [];

int testLineOfSight(Vec2 origin, Vec2 target)
{
    Vec2 origin_to_target = getVec2(origin, target);
    Vec2 p = origin; // test point
    Vec2 direction = normalizeVec2(origin_to_target);
    float distance = getVec2Length(origin_to_target);
    float los;
    
    for (los = 0.0; los < distance; los += LOS_STEP)
    {        
        SET_PIXEL((int)p.x, (int)p.y, COLOUR_WHITE);
        
        if (tileDetectColor(p) == WALL)
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
        angle = getVec2Angle(origin_to_target, direction);
        
        if (angle > VIEW_ANGLE_COS && testLineOfSight(origin, target) == IN_SIGHT)
        {
             return IN_SIGHT;
        }
    }
    return OUT_OF_SIGHT;
}

void chaseTarget(Object* chaser)
{
    Vec2 ObjectToTarget = getVec2(chaser->position, *chaser->ai_target);
    float distance = getVec2Length(ObjectToTarget);
    chaser->direction = normalizeVec2(ObjectToTarget);    
    chaser->magnitude = getVec2Length(chaser->velocity);
    
    if (chaser->magnitude <= MAX_SPEED)
    {
        chaser->velocity.x += chaser->direction.x * ACCELERATION_RATE;
        chaser->velocity.y += chaser->direction.y * ACCELERATION_RATE;
        
        if (distance <= CHASE_THRESHOLD)
        {
            chaser->velocity.x = 0;
            chaser->velocity.y = 0;
        }
    }
}

void think(Object* obj)
{
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
        }
    }
    else if (testFieldOfView(obj->position, obj->direction, *obj->ai_target) == IN_SIGHT)
    {
        obj->ai_mode = CHASE_TARGET;
        obj->ai_timer = 100;
    }
}

void AILoop()
{
    int i = 1;
    
    while (i < Num_Objects)
    {
        think(&object_array[i]);
        i++;
    }
}
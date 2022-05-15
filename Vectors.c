#include "Common.h"
#include "Defines.h"
#include "Structs.h"

/* Vector calculations */

Vec2 getVec2(Vec2 p0, Vec2 p1)
{
    Vec2 v;
    v.x = p1.x - p0.x;
    v.y = p1.y - p0.y;
    
    return v;
}

float dotVec2(Vec2 v1, Vec2 v2)
{
    // dot product is the result of two vectors combined into a single number
    float dot_product = (v1.x * v2.x) + (v1.y * v2.y);
    
    return dot_product;
}

float getVec2Length(Vec2 v)
{
    float Vec2Length = sqrt((v.x * v.x) + (v.y * v.y));
    
    return Vec2Length;
}

float getVec2Angle(Vec2 v1, Vec2 v2)
{
    /*to calculate the angle between two vectors, we first multiply the directional vector
    magnitudes with each other...
    then divide the dot product with that...
    and take arc cosine from the end result, this will give us the angle*/
    
    float vector_angle = dotVec2(v1, v2) / (getVec2Length(v1) * getVec2Length(v2));
    
    return vector_angle;
}

Vec2 normalizeVec2(Vec2 v)
{
    Vec2 normalizedVec;
    
    float vec_length = getVec2Length(v);
    
    normalizedVec.x = v.x / vec_length;
    normalizedVec.y = v.y / vec_length;
    
    return normalizedVec;
}
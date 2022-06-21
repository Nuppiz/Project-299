#ifndef VECTORS_H
#define VECTORS_H

Vec2 getVec2(Vec2 p0, Vec2 p1);
float crossVec2(Vec2 v1, Vec2 v2);
float getVec2Length(Vec2 v);
float getVec2LengthSquared(Vec2 v);
float getVec2Angle2(Vec2 v1, Vec2 v2);
Vec2 normalizeVec2(Vec2 v);
float normalizeAndGetLength(Vec2* v);
Vec2 getDirVec2(double radians);

#endif/* VECTORS_H */

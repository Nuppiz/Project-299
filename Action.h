#ifndef ACTION_H
#define ACTION_H

int checkForHit(Vec2 projectile, Vec2 target, int radius);
void shootWeapon(Object_t* source);
void useTile(Vec2 pos, Vec2 dir);

#endif /* ACTION_H */

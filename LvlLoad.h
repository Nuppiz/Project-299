#ifndef LVLLOAD_H
#define LVLLOAD_H

#include "Common.h"

void freeAllEntities();
void levelLoader(char* level_name, uint8_t load_type);
void saveLevelState();
void levelTransition(char* prevlevelname, char* newlevelname, uint8_t player_death);

#endif /* LVLLOAD_H */

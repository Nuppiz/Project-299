#ifndef LVLLOAD_H
#define LVLLOAD_H

#include "Common.h"

void freeAllEntities();
void levelLoader(char* level_name, uint8_t load_type);
void saveGameState();
void saveLevelState(char* levelname);
void loadGameState();
void loadLevelState(char* savename);
void levelTransition(char* prevlevelname, char* newlevelname);
void loadAfterDeath(char* currentlevel);
void quickSave(char* levelname);
void quickLoad(char* levelname);

#endif /* LVLLOAD_H */

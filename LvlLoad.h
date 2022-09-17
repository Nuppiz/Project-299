#ifndef LVLLOAD_H
#define LVLLOAD_H

#include "Common.h"

void freeAllEntities();
void levelLoader(char* level_name, uint8_t load_type);
void saveGameState(char* foldername);
void saveLevelState(char* foldername, char* levelname);
void loadGameState(char* foldername);
void loadLevelState(char* foldername, char* savename);
void levelTransition(char* prevlevelname, char* newlevelname);
void loadAfterDeath(char* currentlevel);
void quickSave(char* levelname);
void quickLoad();

#endif /* LVLLOAD_H */

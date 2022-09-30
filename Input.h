#ifndef INPUT_H
#define INPUT_H

#include "Common.h"
#include "Structs.h"

void processKeyEvents(uint8_t text_input, TextInput_t* destination);
void clearKeys();
void gameInput();
void titleInput();
void menuInput();
void pauseInput();
void ingameMenuInput();

#endif/* INPUT_H */
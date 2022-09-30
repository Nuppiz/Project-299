#ifndef TEXT_H
#define TEXT_H

void loadFont();
void drawSymbol(int x, int y, int i, uint8_t color);
int  drawText(int x, int y, char* string, uint8_t color);
int  drawTextClipped(int x, int y, char* string, uint8_t color);
void drawMenuText();
void resetInput(TextInput_t* input);
void getCharacter(KeyEvent_t* event, char* destination_str);
int handleTextInput(KeyEvent_t* event, TextInput_t* destination);

#endif/* TEXT */

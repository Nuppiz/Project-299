#ifndef TEXT_H
#define TEXT_H

void loadFont();
void drawSymbol(int x, int y, int i, uint8_t color);
void drawText(int x, int y, char* string, uint8_t color);

#endif /* TEXT_H */

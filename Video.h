#ifndef VIDEO_H
#define VIDEO_H

#include "Structs.h"

extern uint8_t *VGA;
extern uint8_t far screen_buf[];
extern unsigned int visible_page;
extern unsigned int non_visible_page;

int checkForVGA();
void setVideoMode(uint8_t mode);
void loadPalette(char* filename, Palette_t* pal);
void setPalette_VGA(Palette_t* pal);
void setUnchainedMode();
void render();
void renderWithoutClear();
void renderUnchained(uint8_t color);

#endif /* VIDEO_H */

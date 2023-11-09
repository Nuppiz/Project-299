#include "Common.h"
#include "Structs.h"

/* Video mode and palette settings */

uint8_t *VGA=(uint8_t *)0xA0000000L;        /* this points to video memory. */
uint8_t far screen_buf [64000];             // Double screen buffer
Palette_t NewPalette;

unsigned int visible_page = 0;
unsigned int non_visible_page = SCREEN_SIZE / 4;

int checkForVGA()
{
    // uses the VGA/MCGA specific VIDEO_INT function 1A to determine the video adapter
    // this function is absent in older video systems, so it should return an error anyway
    // finally, the regs.h.bl check also filters out MCGA (the game resolution is screwed up in MCGA)

    union REGS regs;

    regs.x.ax = 0x1A00;
	int86(VIDEO_INT, &regs, &regs);
    
    if (regs.h.al == 0x1A && regs.h.bl > 6 && regs.h.bl < 9)
        return TRUE;
    else
        return FALSE;
}

void setVideoMode(uint8_t mode)
{
    union REGS regs;

    regs.h.ah = SET_MODE;
    regs.h.al = mode;
    int86(VIDEO_INT, &regs, &regs);
}

// loads the palette from a 256-colour bitmap file
void loadPalette(char* filename, Palette_t* pal)
{
    FILE *fp;
    int i;
	int x;
    
    // Open the file
    fp = fopen(filename, "rb");
    
    // skip forward and read the number of colours
    fseek(fp, 0x0036, SEEK_SET);
    
    // load palette
    for (i = 0; i < 256; i++)
    {
        uint8_t r,g,b;
        // VGA palette values only go up to 6 bits, (2 ^ 6 == 64 different shades)
        // while bitmap palettes go up to 8 bits, (2 ^ 8 == 256 different shades)
        // right shift (>>) the bits by two places so an 8-bit value becomes a 6-bit
        // this divides the shade value (0-255) by four, giving a value between 0-63
  
        pal->colors[i].b = fgetc(fp);
        pal->colors[i].g = fgetc(fp);
        pal->colors[i].r = fgetc(fp);

        fgetc(fp); // discarded  value
    }

    fclose(fp);
}

void setPalette_VGA(Palette_t* pal)
{
    unsigned i;
    outportb(PALETTE_WRITE, 0);

    for (i = 0; i < 256; i++)
    {
       outportb(PALETTE_DATA, (pal->colors[i].r>>2));
       outportb(PALETTE_DATA, (pal->colors[i].g>>2));
       outportb(PALETTE_DATA, (pal->colors[i].b>>2));
    }
}

void setUnchainedMode()
{
    uint16_t i;
    uint32_t *ptr=(uint32_t *)VGA;            /* used for faster screen clearing */

    outp(SC_INDEX, MEMORY_MODE);       /* turn off chain-4 mode */
    outp(SC_DATA, 0x06);

    outp(SC_INDEX, ALL_PLANES);        /* set map mask to all 4 planes */

    for(i = 0; i < 0x4000; i++)               /* clear all 256K of memory */
        *ptr++ = 0;

    outp(CRTC_INDEX, UNDERLINE_LOCATION);/* turn off long mode */
    outp(CRTC_DATA, 0x00);

    outp(CRTC_INDEX, MODE_CONTROL);      /* turn on byte mode */
    outp(CRTC_DATA, 0xe3);
}

void clearScreen(uint8_t color)
{
    outpw(SC_INDEX, ALL_PLANES);
    memset(&VGA[non_visible_page], color, SCREEN_SIZE / 4);
}

void pageFlip(unsigned int* page1, unsigned int* page2)
{
    unsigned int temp;
    unsigned int high_address, low_address;

    temp = *page1;
    *page1 = *page2;
    *page2 = temp;

    high_address = HIGH_ADDRESS | (*page1 & 0xFF00);
    low_address = LOW_ADDRESS | (*page1 << 8);

    while( inp(INPUT_STATUS_1) & DISPLAY_ENABLE );
    outport( CRTC_INDEX, high_address );
    outport( CRTC_INDEX, low_address );
    while( !(inp(INPUT_STATUS_1) & VRETRACE ) );
}

void render()
{     
    // copy off-screen buffer to VGA memory
    memcpy(VGA, screen_buf, SCREEN_SIZE);

    // clear off-screen buffer so the screen updates properly
    _fmemset(screen_buf, 0, SCREEN_SIZE);
}

void renderWithoutClear()
{     
    // copy off-screen buffer to VGA memory, don't clear buffer
    memcpy(VGA, screen_buf, SCREEN_SIZE);
}

void renderUnchained(uint8_t color)
{
    pageFlip(&visible_page, &non_visible_page);
    clearScreen(color);
}
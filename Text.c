#include "Common.h"

/* Text drawing functions */

extern uint8_t far screen_buf [];
uint8_t alphabet [4240]; // Array to hold the typeface graphics

void loadFont()
{
    FILE* file_ptr;
    file_ptr = fopen("FONT.7UP", "rb");
    fread(alphabet, 1, 4240, file_ptr);
    fclose(file_ptr);
}

void loadFontNew()
{
    FILE* file_ptr;
    file_ptr = fopen("FONTD.7UP", "rb");
    fseek(file_ptr, 8, SEEK_SET); // skip header info
    fread(alphabet, 1, 4240, file_ptr);
    fclose(file_ptr);
}

void drawSymbol(int x, int y, int symbol_index, uint8_t color)
{
    uint8_t index_x = 0;
    uint8_t index_y = 0;
    symbol_index = symbol_index * CHARACTER_SIZE; // pixel index of the symbol in the bitmap file

    for (index_y=0;index_y<TILE_HEIGHT;index_y++)
    {
        for (index_x=0;index_x<TILE_WIDTH;index_x++)
        {
            if (alphabet[symbol_index] != TRANSPARENT_COLOR)
            {
                SET_PIXEL(x, y, alphabet[symbol_index] + color);
                symbol_index++;
                x++;
            }
            else
            {
                symbol_index++;
                x++;
            }
        }
        index_x = 0;
        x = x - TILE_WIDTH;
        y++;
    }
    index_y = 0;
    symbol_index = 0;
}

int drawText(int x, int y, char* string, uint8_t color)
{
    int i = 0;
    int newlines = 0;
    int start_x = x;
    char c;
    
    while ((c = string[i++]) != 0)
    {
        if (c == '\n')
        {
            x = start_x;
            y += 10;
            newlines++;
            continue;
        }
        drawSymbol(x, y, c - 32, color);
        x += 10;
    }

    return newlines;
}

void drawDebug()
{
    int i;
    int y = 0;

    for (i = 0; i < NUM_DEBUG; i++)
    {
        if (debug[i][0] != '\0')
            y += (drawText(0, y, debug[i], COLOUR_WHITE) + 1) * 10;
    }
}
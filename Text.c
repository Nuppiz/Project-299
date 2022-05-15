#include "Common.h"
#include "Defines.h"

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

void drawSymbol(int x, int y, int symbol_index, uint8_t color)
{
    uint8_t index_x = 0;
    uint8_t index_y = 0;
    symbol_index = symbol_index * CHARACTER_SIZE; // pixel index of the symbol in the bitmap file

    for (index_y=0;index_y<TILE_HEIGHT;index_y++)
    {
        for (index_x=0;index_x<TILE_WIDTH;index_x++)
        {
            if (alphabet[symbol_index] != 13)
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

void drawText(int x, int y, char* string, uint8_t color)
{
    int i = 0;
    char c;
    
    while (string[i] != 0)
    {
        c = string[i];
        drawSymbol(x, y, c - 32, color);
        x = x + 10;
        i++;
    }
}
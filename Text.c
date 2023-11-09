#include "Common.h"
#include "Structs.h"
#include "Keyb.h"
#include "Video.h"

#define FONT_FILE_SIZE 7128

/* Text input, output and drawing functions */

extern Menu_t* current_menu;
extern Keyboard_t Keyboard;
uint8_t alphabet [FONT_FILE_SIZE]; // Array to hold the typeface graphics

void loadFont()
{
    FILE* file_ptr;
    file_ptr = fopen("FONT.7UP", "rb");
    fread(alphabet, 1, FONT_FILE_SIZE, file_ptr);
    fclose(file_ptr);
}

void loadFontNew()
{
    FILE* file_ptr;
    file_ptr = fopen("FONTD.7UP", "rb");
    fseek(file_ptr, 8, SEEK_SET); // skip header info
    fread(alphabet, 1, FONT_FILE_SIZE, file_ptr);
    fclose(file_ptr);
}

void drawSymbol(int x, int y, int symbol_index, uint8_t color)
{
    uint8_t index_x = 0;
    uint8_t index_y = 0;
    symbol_index = symbol_index * CHARACTER_SIZE; // pixel index of the symbol in the bitmap file

    for (index_y = 0; index_y < CHARACTER_HEIGHT; index_y++)
    {
        for (index_x = 0; index_x < CHARACTER_WIDTH; index_x++)
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
        x = x - CHARACTER_WIDTH;
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
        drawSymbol(x, y, c - 24, color);
        x += 10;
    }

    return newlines;
}

void drawSymbolVGA(int x, int y, int symbol_index, uint8_t color)
{
    uint8_t index_x = 0;
    uint8_t index_y = 0;
    symbol_index = symbol_index * CHARACTER_SIZE; // pixel index of the symbol in the bitmap file

    for (index_y = 0; index_y < CHARACTER_HEIGHT; index_y++)
    {
        for (index_x = 0; index_x < CHARACTER_WIDTH; index_x++)
        {
            if (alphabet[symbol_index] != TRANSPARENT_COLOR)
            {
                SET_PIXEL_VGA(x, y, alphabet[symbol_index] + color);
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
        x = x - CHARACTER_WIDTH;
        y++;
    }
    index_y = 0;
    symbol_index = 0;
}

int drawTextVGA(int x, int y, char* string, uint8_t color)
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
        drawSymbolVGA(x, y, c - 24, color);
        x += 10;
    }

    return newlines;
}

int drawTextClipped(int x, int y, char* string, uint8_t color)
{
    int i = 0;
    int newlines = 0;
    int start_x = x;
    char c;

    if (x < 0 || x > SCREEN_WIDTH - 10 || y < 0 || y > SCREEN_HEIGHT - 10)
        return 0;

    while ((c = string[i++]) != 0)
    {
        if (c == '\n')
        {
            x = start_x;
            y += 10;
            newlines++;
            
            if (y > SCREEN_HEIGHT - 10)
                return 0;

            continue;
        }
        if (x > SCREEN_WIDTH-10)
        {
            continue;
        }
        drawSymbol(x, y, c - 24, color);
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
        #if DEBUG == 1
        if (debug[i][0] != '\0')
            y += (drawText(0, y, debug[i], COLOUR_WHITE) + 1) * 10;
        #endif
    }
}

void drawMenuText()
{
    int i;
    int y = current_menu->start_y;

    for (i = 0; i < current_menu->num_selections; i++)
    {
        drawText(current_menu->cursor_x + 20, y, current_menu->options[i].text, COLOUR_WHITE);
        y += current_menu->cursor_spacing;
    }
    drawText(current_menu->cursor_x, current_menu->cursor_y, "->", COLOUR_WHITE);
}

void resetInput(TextInput_t* input)
{
    input->buffer[0] = '\0';
    input->length = 0;
    input->cursor = 0;
    input->start = 0;
}

void cursorLeft(TextInput_t* text)
{
    if (text->cursor > text->start)
        text->cursor--;
}

void cursorRight(TextInput_t* text)
{
    if (text->cursor < text->length - 1)
        text->cursor++;
}

void writeChar(TextInput_t* text, char c)
{
    if (text->length < text->capacity - 1)
    {
        if (text->cursor < text->length)
        {
            int i;
            for (i = text->length; i > text->cursor; i--)
                text->buffer[i] = text->buffer[i - 1];
        }
        text->buffer[text->cursor++] = c;
        text->buffer[++text->length] = '\0';
    }
}

void eraseChar(TextInput_t* text)
{
    if (text->cursor > text->start)
    {
        if (text->cursor < text->length)
        {
            int i;
            for (i = text->cursor; i < text->length; i++)
                text->buffer[i - 1] = text->buffer[i];
        }
        text->cursor--;
        text->length--;
        text->buffer[text->length] = '\0';
    }
}

uint8_t keyToAscii(uint8_t keycode)
{
    uint8_t c;

    if (keycode < 128)
    {
        if ((KEY_IS_PRESSED(KEY_LSHIFT) | KEY_IS_PRESSED(KEY_RSHIFT)))
            c = ascii_shifted[keycode];
        else
            c = ascii[keycode];
    }
    else
        c = ascii_special[keycode & 0x7F];
    // return a character if the keycode corresponds to a valid ASCII code
    if ((c >= 32 && c < 128) || c == '/')
        return c;
    
    return 0;
}

void getCharacter(KeyEvent_t* event, char* destination_str)
{
    char c;

    if (event->type == KEY_HIT_FLAG)
    {
        if ((c = keyToAscii(event->keycode)) != 0)
        {
            /*if (c >= 97 && c < 123) // if current typeface doesn't have non-caps letters, convert them to caps
                c -= 32;*/
            strncat(destination_str, &c, 1);
        }
    }
}

int handleTextInput(KeyEvent_t* event, TextInput_t* destination)
{   
    char c;

    if (event->type == KEY_HIT_FLAG)
    {
        switch (event->keycode)
        {
        case KEY_LEFT:
            cursorLeft(destination);
            break;
        case KEY_RIGHT:
            cursorRight(destination);
            break;
        case KEY_BACKSPACE:
            eraseChar(destination);
            break;
        default:
            if ((c = keyToAscii(event->keycode)) != 0)
            {
                /*if (c >= 97 && c < 123) // if current typeface doesn't have non-caps letters, onvert them to caps
                    c -= 32;*/
                writeChar(destination, c);
                break;
            }
        }

    }
    return TRUE;
}
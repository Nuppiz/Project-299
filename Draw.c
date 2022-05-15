#include "Common.h"
#include "Defines.h"
#include "Enums.h"
#include "Structs.h"

/* Graphics drawing functions */

extern uint8_t far screen_buf [];
extern Object object_array [];
extern uint8_t grid_array [];
extern uint8_t texture_array [];
extern Texture Textures [];

void drawSprite(int x, int y, Texture* texture)
{
    int pix_x = x;
    int pix_y = y;
    int index_x = 0;
    int index_y = 0;
    int i = 0;

    //x += offset.x;

    if (texture->transparent == TRUE)
    {
        for (index_y=0;index_y<texture->height;index_y++)
        {
            for (index_x=0;index_x<texture->width;index_x++)
            {
                if (texture->pixels[i] != TRANSPARENT_COLOR)
                {
                    if (pix_x < SCREEN_WIDTH && pix_y < SCREEN_HEIGHT)
                    {
                        SET_PIXEL(pix_x, pix_y, texture->pixels[i]);
                        i++;
                        pix_x++;
                    }
                }
                else
                {
                    i++;
                    pix_x++;
                }
            }
            index_x = 0;
            pix_x = x;
            pix_y++;
        }
    }
    else
    {
        for (index_y=0;index_y<texture->height;index_y++)
        {
            for (index_x=0;index_x<texture->width;index_x++)
            {
                if (pix_x < SCREEN_WIDTH && pix_y < SCREEN_HEIGHT)
                {
                    SET_PIXEL(pix_x, pix_y, texture->pixels[i]);
                    i++;
                    pix_x++;
                }
            }
            index_x = 0;
            pix_x = x;
            pix_y++;
        }
    }
}

void drawCircle(Vec2* position, int radius, uint8_t color)
{
    int offset_x;
    int offset_y;

    int camera_offset = 2;
    
    int center_x = position->x + camera_offset * SQUARE_SIZE;
    int center_y = position->y;

    offset_y = 0;
    do
    {
        offset_x = sqrt((float)radius*radius - (float)offset_y*offset_y);
        SET_PIXEL(center_x + offset_x, center_y + offset_y, color); // lower right octant
        SET_PIXEL(center_x - offset_x, center_y + offset_y, color); // lower left octant
        SET_PIXEL(center_x + offset_x, center_y - offset_y, color); // upper right octant
        SET_PIXEL(center_x - offset_x, center_y - offset_y, color); // upper left octant
        SET_PIXEL(center_x + offset_y, center_y + offset_x, color); // bottom right octant
        SET_PIXEL(center_x - offset_y, center_y + offset_x, color); // bottom left octant
        SET_PIXEL(center_x + offset_y, center_y - offset_x, color); // top right octant
        SET_PIXEL(center_x - offset_y, center_y - offset_x, color); // top left octant

        offset_y++;
    }
    while (offset_y < offset_x);
}

void drawSquareColor(int x, int y, uint8_t colour)
{
    int index_y = 0;

    for (index_y=0;index_y<SQUARE_SIZE;index_y++)
    {
        //memsetting the entire row is faster than drawing it pixel by pixel
        memset(screen_buf + (y * SCREEN_WIDTH + x), colour, SQUARE_SIZE);
        y++;
    }
}

void drawDot(Object* obj)
{
    int offset_y = 0;
    int offset_x = 0;
    float dot_radians;

    int camera_offset = 2;
    
    // calculate angle
    dot_radians = atan2(obj->direction.y, obj->direction.x);
    
    // directional dot's offsets from the center of the circle
    offset_y = sin(dot_radians) * (obj->radius + 2);
    offset_x = cos(dot_radians) * (obj->radius + 2) + camera_offset * SQUARE_SIZE;
    // center of the circle has to be cast into int, otherwise the draw function doesn't work
    SET_PIXEL((int)obj->position.x + offset_x, (int)obj->position.y + offset_y, COLOUR_WHITE);
}

void drawMap()
{
    int camera_offset = 2;
    int i = camera_offset; // square drawing "index" from array
    int start_index = camera_offset;
    int x_pixel;
    int y_pixel;
    int num_cols;
    int num_rows = 0; // number of "rows" traversed in the array
    
    // draw 160 20x20 squares (maximum that can fit on the 320x200 screen)
    for (y_pixel = 0; y_pixel < SCREEN_HEIGHT; y_pixel += SQUARE_SIZE)
    {
        for(x_pixel = 0, num_cols = 0; x_pixel < SCREEN_WIDTH && num_cols < SQUARE_COLUMNS; x_pixel += SQUARE_SIZE, num_cols++)
        {
            //drawSquareColor(x, y, grid_array[a]); // old color-based drawing
            drawSprite(x_pixel, y_pixel, &Textures[texture_array[i]]);
            i++;
        }
        num_rows++;
        i = start_index + NUM_COLUMNS * num_rows;
    }
}

void drawStuff()
{
    int i = 0; // object array "index"

    drawMap();
    
    /* change player square to a lovely peach colour
    drawSquare(object_array[0].grid_loc.x * SQUARE_SIZE, object_array[0].grid_loc.y * SQUARE_SIZE, COLOUR_PEACH);*/
    
    while (i < Num_Objects)
    {
        // draw all circles in their current locations
        drawCircle(&object_array[i].position, object_array[i].radius, object_array[i].color);
        drawDot(&object_array[i]);
        i++;
    }
}
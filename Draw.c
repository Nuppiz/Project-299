#include "Common.h"
#include "Defines.h"
#include "Enums.h"
#include "Structs.h"

/* Graphics drawing functions */

extern uint8_t far screen_buf [];
extern Object object_array [];
extern Texture Textures [];
extern Map map1;
extern Vec2 camera_offset;

int boundaryCheck(int x, int y)
{
    if (x < SCREEN_WIDTH && x >= 0 && y < SCREEN_HEIGHT && y >= 0)
        return TRUE;
    else
        return FALSE;
}

int boundaryCheckRadius(int x, int y, int radius)
{
    if (x < SCREEN_WIDTH - radius && x >= 0 + radius && y < SCREEN_HEIGHT - radius && y >= 0 + radius)
        return FULLY_IN;
    else if (x - radius > SCREEN_WIDTH || x + radius < 0 ||
            y - radius > SCREEN_HEIGHT || y + radius < 0)
        return FULLY_OUT;
    else
        return PARTIAL;
}

int boundaryCheck_X(int x)
{
    if (x < SCREEN_WIDTH && x >= 0)
        return TRUE;
    else
        return FALSE;
}

int boundaryCheck_Y(int y)
{
    if (y < SCREEN_HEIGHT && y >= 0)
        return TRUE;
    else
        return FALSE;
}

void drawSprite(int x, int y, Texture* texture)
{
    int pix_x = x;
    int pix_y = y;
    int index_x;
    int index_y;
    int i = 0;

    if (texture->transparent == TRUE)
    {
        for (index_y = 0; index_y < texture->height; index_y++)
        {
            for (index_x = 0; index_x < texture->width; index_x++)
            {
                if (texture->pixels[i] != TRANSPARENT_COLOR)
                {
                    if (pix_x < SCREEN_WIDTH && pix_y < SCREEN_HEIGHT)
                    {
                        SET_PIXEL(pix_x, pix_y, texture->pixels[i]);
                        i++;
                        pix_x++;
                    }
                    else
                    {
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
            pix_x = x;
            pix_y++;
        }
    }
    else
    {
        for (index_y = 0; index_y < texture->height; index_y++)
        {
            for (index_x = 0; index_x < texture->width; index_x++)
            {
                if (pix_x < SCREEN_WIDTH && pix_x >= 0 && pix_y < SCREEN_HEIGHT && pix_y >= 0)
                {
                    SET_PIXEL(pix_x, pix_y, texture->pixels[i]);
                    i++;
                    pix_x++;
                }
                else
                {
                    i++;
                    pix_x++;
                }
            }
            pix_x = x;
            pix_y++;
        }
    }
}

/*void drawSpritePartial(int x, int y, int start_x, int start_y, Texture* texture)
{
    int pix_x = x;
    int pix_y = y;
    int index_x;
    int index_y;
    int i = start_x;

    if (start_x < 0)
        i = 0;

    if (texture->transparent == TRUE)
    {
        for (index_y = 0; index_y < texture->height; index_y++)
        {
            for (index_x = 0; index_x < texture->width; index_x++)
            {
                if (texture->pixels[i] != TRANSPARENT_COLOR)
                {
                    if (pix_x < SCREEN_WIDTH && pix_y < SCREEN_HEIGHT)
                    {
                        SET_PIXEL(pix_x, pix_y, texture->pixels[i]);
                        i++;
                        pix_x++;
                    }
                    else
                    {
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
            i += start_x;
            pix_x = x;
            pix_y++;
        }
    }
    else
    {
        for (index_y = 0; index_y < texture->height; index_y++)
        {
            for (index_x = 0; index_x < texture->width; index_x++)
            {
                if (pix_x < SCREEN_WIDTH && pix_y < SCREEN_HEIGHT)
                {
                    SET_PIXEL(pix_x, pix_y, texture->pixels[i]);
                    i++;
                    pix_x++;
                }
                else
                {
                    i++;
                    pix_x++;
                }
            }
            pix_x = x;
            pix_y++;
        }
    }
}*/

void drawOctants(int center_x, int offset_x, int center_y, int offset_y, uint8_t color)
{
    SET_PIXEL(center_x + offset_x, center_y + offset_y, color); // lower right octant
    SET_PIXEL(center_x - offset_x, center_y + offset_y, color); // lower left octant
    SET_PIXEL(center_x + offset_x, center_y - offset_y, color); // upper right octant
    SET_PIXEL(center_x - offset_x, center_y - offset_y, color); // upper left octant
    SET_PIXEL(center_x + offset_y, center_y + offset_x, color); // bottom right octant
    SET_PIXEL(center_x - offset_y, center_y + offset_x, color); // bottom left octant
    SET_PIXEL(center_x + offset_y, center_y - offset_x, color); // top right octant
    SET_PIXEL(center_x - offset_y, center_y - offset_x, color); // top left octant   
}

void drawCircle(Vec2* position, int radius, uint8_t color)
{
    int offset_x;
    int offset_y;
    
    int center_x = position->x - (int)camera_offset.x;
    int center_y = position->y - (int)camera_offset.y;

    offset_y = 0;

    do
    {
        offset_x = sqrt((float)radius*radius - (float)offset_y*offset_y);
        if (boundaryCheckRadius(center_x, center_y, radius == FULLY_IN))
        {
            drawOctants(center_x, offset_x, center_y, offset_y, color);
        }
        offset_y++;
    }
    while (offset_y < offset_x);
}

void drawSquareColor(int x, int y, uint8_t colour)
{
    int index_y = 0;

    for (index_y = 0; index_y < SQUARE_SIZE; index_y++)
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
    int pos_x = obj->position.x;
    int pos_y = obj->position.y;
    float dot_radians;
    
    // calculate angle
    dot_radians = atan2(obj->direction.y, obj->direction.x);
    
    // directional dot's offsets from the center of the circle
    offset_y = sin(dot_radians) * (obj->radius + 2) - (int)camera_offset.y;
    offset_x = cos(dot_radians) * (obj->radius + 2) - (int)camera_offset.x;
    if (boundaryCheck_X(pos_x + offset_x) == TRUE && boundaryCheck_Y(pos_y + offset_y) == TRUE)
    {
        SET_PIXEL(pos_x + offset_x, pos_y + offset_y, COLOUR_WHITE);
    }
}

void drawRectangle(int x, int y, int w, int h, uint8_t color)
{
    int index_x = 0;
    int index_y = 0;

    for (index_y = 0; index_y < h;index_y++)
    {
        for (index_x = 0; index_x < w;index_x++)
        {
            SET_PIXEL(x, y, color);
            x++;
        }
        index_x = 0;
        x -= w;
        y++;
    }
    index_y = 0;
}

void drawMap(Map* map)
{
    int xi = (int)camera_offset.x / SQUARE_SIZE; // first column in the array to be drawn
    int yi = (int)camera_offset.y / SQUARE_SIZE; // first row in the array to be drawn
    int i = xi; // square drawing "index" from array
    int start_index = i; // first index of the array that is drawn on screen
    int x_pixel; // x-coordinate of the currently drawn pixel
    int y_pixel; // y-coordinate of the currently drawn pixel
    int num_rows; // number of "rows" traversed in the array
    int num_cols; // number of "columns" traversed in the array
    int max_cols = map->width - xi - 1; // max columns to draw
    int max_rows = map->height; // max rows to draw
    char X_str [20];
    char Y_str [20];
    char XI_str [20];
    char YI_str [20];
    char I_str [20];
    sprintf(X_str, "X OFF: %f", camera_offset.x);
    sprintf(Y_str, "Y OFF: %f", camera_offset.y);
    sprintf(XI_str, "X-I: %d", xi);
    sprintf(YI_str, "Y-I: %d", yi);
    sprintf(I_str, "I: %d", i);

    // run loops until maximum number of squares is drawn and the edges of the screen have been reached
    for (y_pixel = 0 - camera_offset.y, num_rows = 0; y_pixel < SCREEN_HEIGHT && num_rows < max_rows; y_pixel += SQUARE_SIZE)
    {
        if (camera_offset.x > 0)
        {
            for (x_pixel = 0 - (camera_offset.x - xi * SQUARE_SIZE), num_cols = 0; x_pixel < SCREEN_WIDTH && num_cols <= max_cols; x_pixel += SQUARE_SIZE, num_cols++)
            {
                drawSprite(x_pixel, y_pixel, &Textures[map->textures[i]]);
                i++;
            }
        }
        else
        {
            for (x_pixel = 0 - (camera_offset.x - start_index * SQUARE_SIZE); x_pixel < SCREEN_WIDTH; x_pixel += SQUARE_SIZE)
            {
                // eliminate unnecessary drawing on the left of the screen
                if (x_pixel >= abs(xi) * SQUARE_SIZE)
                    drawSprite(x_pixel, y_pixel, &Textures[map->textures[i]]);
                i++;
            }
        }
        num_rows++;
        i = start_index + (map->width * num_rows); // jump in the texture array to the next "row"
    }
    drawText(0, 0, X_str, COLOUR_WHITE);
    drawText(0, 10, Y_str, COLOUR_WHITE);
    drawText(0, 20, XI_str, COLOUR_WHITE);
    drawText(0, 30, YI_str, COLOUR_WHITE);
    drawText(0, 40, I_str, COLOUR_WHITE);
}

void drawStuff()
{
    int i = 0; // object array "index"

    drawMap(&map1);
    
    /* change player square to a lovely peach colour
    drawSquare(object_array[0].grid_loc.x * SQUARE_SIZE, object_array[0].grid_loc.y * SQUARE_SIZE, COLOUR_PEACH);*/
    
    while (i < Num_Objects)
    {
        // draw all circles in their current locations
        //drawCircle(&object_array[i].position, object_array[i].radius, object_array[i].color);
        drawDot(&object_array[i]);
        i++;
    }
}
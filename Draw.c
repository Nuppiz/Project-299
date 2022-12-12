#include "Common.h"
#include "Structs.h"
#include "Text.h"
#include "Action.h"
#include "Video.h"
#include "Vectors.h"
#include "Loadgfx.h"
#include "Keyb.h"

/* Graphics drawing functions */

extern System_t System;
extern GameData_t Game;

extern uint8_t far screen_buf[];
extern Texture_array ObjectTextures;
extern Texture_array TileTextures;
extern Anim_array Animations;
extern Tile_t TileSet[];
extern Item_t* Items;
extern Keyboard_t Keyboard;
extern Weapon_t Weapons[];

// test subjects, del later
extern Sprite_t Rocket;
extern Sprite_t Explosion;

Vec2 camera_offset;

Particle_t Particles[MAX_PARTICLES] = {0};
int particle_read = 0;
int particle_write = 0;

Corpse_t Corpses[MAX_CORPSES] = {0};
int corpse_read = 0;
int corpse_write = 0;

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

void drawTexture(int x, int y, Texture_t* texture)
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
            memcpy(&screen_buf[pix_y * SCREEN_WIDTH + pix_x],
                   &texture->pixels[(index_y + index_y) * texture->width],
                   texture->width);
        }
    }
}

void drawTextureClipped(int x, int y, Texture_t* texture)
{
    int start_x;
    int start_y;
    int end_x;
    int end_y;
    int index_x;
    int index_y;
    int clip_x;
    int clip_y;
    int x_offset = 0;
    int y_offset = 0;

    int sprite_w = texture->width;
    int sprite_h = texture->height;

    start_x = x;
    start_y = y;

    end_x = start_x + sprite_w - 1;
    end_y = start_y + sprite_h - 1;

    if (start_x >= SCREEN_WIDTH || start_y >= SCREEN_HEIGHT || start_x <= -sprite_w || start_y <= -sprite_h)
        return;
    
    if (start_x < 0)
    {   
        x_offset = abs(start_x);
        start_x = 0;
    }
    
    else if (end_x >= SCREEN_WIDTH)
        end_x = SCREEN_WIDTH - 1;

    if (start_y < 0)
    {
        y_offset = abs(start_y);
        start_y = 0;
    }
    
    else if (end_y >= SCREEN_HEIGHT)
        end_y = SCREEN_HEIGHT - 1;

    clip_x = end_x - start_x + 1;
    clip_y = end_y - start_y + 1;

    for (index_y = 0; index_y < clip_y; index_y++)
    {
        memcpy(&screen_buf[(start_y + index_y) * SCREEN_WIDTH + start_x], &texture->pixels[(index_y + y_offset) * texture->width + x_offset], clip_x);
    }
}

void drawTexturePartial(int x, int y, Texture_t* texture)
{
    int start_x;
    int start_y;
    int end_x;
    int end_y;
    int index_x;
    int index_y;
    int clip_x;
    int clip_y;
    int x_offset = 0;
    int y_offset = 0;

    int sprite_w = texture->width;
    int sprite_h = texture->height;

    x -= texture->offset_x;
    y -= texture->offset_y;

    start_x = x;
    start_y = y;

    end_x = start_x + sprite_w - 1;
    end_y = start_y + sprite_h - 1;

    if (start_x >= SCREEN_WIDTH || start_y >= SCREEN_HEIGHT || start_x <= -sprite_w || start_y <= -sprite_h)
        return;
    
    if (start_x < 0)
    {   
        x_offset = abs(start_x);
        start_x = 0;
    }
    
    else if (end_x >= SCREEN_WIDTH)
        end_x = SCREEN_WIDTH - 1;

    if (start_y < 0)
    {
        y_offset = abs(start_y);
        start_y = 0;
    }
    
    else if (end_y >= SCREEN_HEIGHT)
        end_y = SCREEN_HEIGHT - 1;

    clip_x = end_x - start_x + 1;
    clip_y = end_y - start_y + 1;

    if (texture->transparent == TRUE)
    {
        for (index_y = 0; index_y < clip_y; index_y++)
        {
            for (index_x = 0; index_x < clip_x; index_x++)
            {
                if (texture->pixels[(index_y + y_offset) * texture->width + (x_offset + index_x)] != TRANSPARENT_COLOR)
                    SET_PIXEL(start_x + index_x, start_y + index_y, texture->pixels[(index_y + y_offset) * texture->width + (x_offset + index_x)]);    
            }
        }
    }
    else
    {
        for (index_y = 0; index_y < clip_y; index_y++)
        {
            for (index_x = 0; index_x < clip_x; index_x++)
            {
                SET_PIXEL(start_x + index_x, start_y + index_y, texture->pixels[(index_y + y_offset) * texture->width + (x_offset + index_x)]);    
            }
        }
    }
}

float rotateShearX(Vec2 source, double angle)
{
    float new_loc;

    new_loc = source.x - tan(angle / 2) * source.y;

    new_loc = (int)new_loc;

    return new_loc;
}

float rotateShearY(Vec2 source, double angle)
{
    float new_loc;

    new_loc = source.y + sin(angle) * source.x;

    new_loc = (int)new_loc;

    return new_loc;
}

void drawTextureRotated(int x, int y, double angle, Texture_t* source, uint8_t bgcolor)
{
    Texture_t rotated;
    Vec2 sheared;
    float width_scale = 1.0;
    float height_scale = 1.0;
    int w;
    int h;
    int w_half;
    int h_half;
    int i = 0;
    float rot_i;
    int rotated_size;
    uint8_t mirror_flip = FALSE;

    if (angle > RAD_270)
        angle -= RAD_360;

    else if (angle < -RAD_270)
        angle += RAD_360;

    if (angle >= RAD_90)
    {
        angle -= RAD_180;
        mirror_flip = TRUE;
    }

    else if (angle <= -RAD_90)
    {
        angle += RAD_180;
        mirror_flip = TRUE;
    }

    if (source->width != source->height)
    {
        if (source->width < source->height)
            width_scale = source->height / source->width;

        else if (source->height < source->width)
            height_scale = source->width / source->height;

        rotated.width = abs(source->height * sin(angle)) + abs(source->width * cos(angle)) + (7 * height_scale);
        rotated.height = abs(source->width * sin(angle)) + abs(source->height * cos(angle)) + (7 * width_scale);
    }

    else
        rotated.width = abs(source->height * sin(angle)) + abs(source->width * cos(angle)) + 5;
        rotated.height = abs(source->width * sin(angle)) + abs(source->height * cos(angle)) + 5;

    if (source->transparent == TRUE)
        rotated.transparent = TRUE;

    rotated.offset_x = (rotated.width - source->width) / 2;
    rotated.offset_y = (rotated.height - source->height) / 2;

    w_half = source->width / 2;
    h_half = source->height / 2;

    rotated_size = rotated.width * rotated.height;
    rotated.pixels = malloc(rotated_size);
    memset(rotated.pixels, bgcolor, rotated_size);

    if (mirror_flip == TRUE)
    {
        for (h = -h_half; h < h_half; h++)
        {
            for (w = -w_half; w < w_half; w++)
            {
                sheared.x = w;
                sheared.y = h;
                sheared.x = rotateShearX(sheared, angle);
                sheared.y = rotateShearY(sheared, angle);
                sheared.x = rotateShearX(sheared, angle);
                rot_i = ((rotated.height - ((int)sheared.y + rotated.offset_y + h_half))) * rotated.width + (rotated.width - (sheared.x + rotated.offset_x + w_half));
                rotated.pixels[(int)rot_i] = source->pixels[i];
                i++;
            }
        }
    }
    else
    {
        for (h = -h_half; h < h_half; h++)
        {
            for (w = -w_half; w < w_half; w++)
            {
                sheared.x = w;
                sheared.y = h;
                sheared.x = rotateShearX(sheared, angle);
                sheared.y = rotateShearY(sheared, angle);
                sheared.x = rotateShearX(sheared, angle);
                rot_i = ((int)sheared.y + rotated.offset_y + h_half) * rotated.width + (sheared.x + rotated.offset_x + w_half);
                rotated.pixels[(int)rot_i] = source->pixels[i];
                i++;
            }
        }
    }
    drawTexturePartial(x, y, &rotated);

    free(rotated.pixels);
}

Texture_t saveRotatedTexture(double angle, Texture_t* source, uint8_t bgcolor)
{
    Texture_t rotated;
    Vec2 sheared;
    int w;
    int h;
    int w_half;
    int h_half;
    int i = 0;
    float rot_i;
    int rotated_size;
    uint8_t mirror_flip = FALSE;

    if (angle > RAD_270)
        angle -= RAD_360;

    else if (angle < -RAD_270)
        angle += RAD_360;

    if (angle >= RAD_90)
    {
        angle -= RAD_180;
        mirror_flip = TRUE;
    }

    else if (angle <= -RAD_90)
    {
        angle += RAD_180;
        mirror_flip = TRUE;
    }

    if (source->transparent == TRUE)
        rotated.transparent = TRUE;

    rotated.width = abs(source->height * sin(angle)) + abs(source->width * cos(angle)) + 5;
    rotated.height = abs(source->width * sin(angle)) + abs(source->height * cos(angle)) + 5;

    rotated.offset_x = (rotated.width - source->width) / 2;
    rotated.offset_y = (rotated.height - source->height) / 2;

    w_half = source->width / 2;
    h_half = source->height / 2;

    rotated_size = rotated.width * rotated.height;
    rotated.pixels = malloc(rotated_size);
    memset(rotated.pixels, bgcolor, rotated_size);

    if (mirror_flip == TRUE)
    {
        for (h = -h_half; h < h_half; h++)
        {
            for (w = -w_half; w < w_half; w++)
            {
                sheared.x = w;
                sheared.y = h;
                sheared.x = rotateShearX(sheared, angle);
                sheared.y = rotateShearY(sheared, angle);
                sheared.x = rotateShearX(sheared, angle);
                rot_i = ((rotated.height - ((int)sheared.y + rotated.offset_y + h_half))) * rotated.width + (rotated.width - (sheared.x + rotated.offset_x + w_half));
                rotated.pixels[(int)rot_i] = source->pixels[i];
                i++;
            }
        }
    }
    else
    {
        for (h = -h_half; h < h_half; h++)
        {
            for (w = -w_half; w < w_half; w++)
            {
                sheared.x = w;
                sheared.y = h;
                sheared.x = rotateShearX(sheared, angle);
                sheared.y = rotateShearY(sheared, angle);
                sheared.x = rotateShearX(sheared, angle);
                rot_i = ((int)sheared.y + rotated.offset_y + h_half) * rotated.width + (sheared.x + rotated.offset_x + w_half);
                rotated.pixels[(int)rot_i] = source->pixels[i];
                i++;
            }
        }
    }
    return rotated;
}

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

    if (boundaryCheckRadius(center_x, center_y, radius == FULLY_IN))
    {
        do
        {
            offset_x = sqrt((float)radius*radius - (float)offset_y*offset_y);
            drawOctants(center_x, offset_x, center_y, offset_y, color);
            offset_y++;
        }
        while (offset_y < offset_x);
    }
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

void drawRectangleVGA(int x, int y, int w, int h, uint8_t color)
{
    int index_x = 0;
    int index_y = 0;

    for (index_y = 0; index_y < h;index_y++)
    {
        for (index_x = 0; index_x < w;index_x++)
        {
            SET_PIXEL_VGA(x, y, color);
            x++;
        }
        index_x = 0;
        x -= w;
        y++;
    }
    index_y = 0;
}

void drawMap()
{
    int xi = (int)camera_offset.x / SQUARE_SIZE; // first column in the array to be drawn
    //int yi = (int)camera_offset.y / SQUARE_SIZE; // first row in the array to be drawn
    int i = xi; // square drawing "index" from array
    int start_index = i; // first index of the array that is drawn on screen
    int x_pixel; // x-coordinate of the currently drawn pixel
    int y_pixel; // y-coordinate of the currently drawn pixel
    int num_rows; // number of "rows" traversed in the array
    int num_cols; // number of "columns" traversed in the array
    int max_cols = Game.Map.width - xi - 1; // max columns to draw
    int max_rows = Game.Map.height; // max rows to draw
    int a; // array index for Interactives array

    // run loops until maximum number of squares is drawn and the edges of the screen have been reached
    for (y_pixel = 0 - camera_offset.y, num_rows = 0; y_pixel < SCREEN_HEIGHT && num_rows < max_rows; y_pixel += SQUARE_SIZE)
    {
        if (camera_offset.x > 0)
        {
            for (x_pixel = 0 - (camera_offset.x - xi * SQUARE_SIZE), num_cols = 0; x_pixel < SCREEN_WIDTH && num_cols <= max_cols; x_pixel += SQUARE_SIZE, num_cols++)
            {
                drawTextureClipped(x_pixel, y_pixel, &TileTextures.textures[Game.Map.tilemap[i].texture_id]);
                for (a = 0; a < Game.item_count; a++)
                {
                    if (Items[a].state == 1)
                    {                    
                        if (i == Items[a].index && Items[a].type == ITEM_KEY_RED)
                            drawTexturePartial(x_pixel, y_pixel, &ObjectTextures.textures[TEX_KEY]);
                    }
                }
                i++;
            }
        }
        else
        {
            for (x_pixel = 0 - (camera_offset.x - start_index * SQUARE_SIZE); x_pixel < SCREEN_WIDTH; x_pixel += SQUARE_SIZE)
            {
                // eliminate unnecessary drawing on the left of the screen
                if (x_pixel >= abs(xi) * SQUARE_SIZE)
                {
                    drawTextureClipped(x_pixel, y_pixel, &TileTextures.textures[Game.Map.tilemap[i].texture_id]);
                    for (a = 0; a < Game.item_count; a++)
                    {
                        if (Items[a].state == 1)
                        {                    
                            if (i == Items[a].index && Items[a].type == ITEM_KEY_RED)
                                drawTexturePartial(x_pixel, y_pixel, &ObjectTextures.textures[TEX_KEY]);
                        }
                    }
                }
                i++;
            }
        }
        num_rows++;
        i = start_index + (Game.Map.width * num_rows); // jump in the texture array to the next "row"
    }
}

void testColors()
{
    int i, x;
    int y = 100;

    for (i = 0, x = 31; i < 256; i++, x++)
        SET_PIXEL(x, y, i);
}

void calcCameraOffset()
{
    Vec2 pos;
    float angle;

    int cam_min_x = SCREEN_WIDTH/2;
    int cam_max_x = Game.Map.width*SQUARE_SIZE - SCREEN_WIDTH/2;
    int cam_min_y = SCREEN_HEIGHT/2;
    int cam_max_y = Game.Map.height*SQUARE_SIZE - SCREEN_HEIGHT/2;

    angle = atan2(PLAYER_ACTOR.direction.y, PLAYER_ACTOR.direction.x);
    pos.x = PLAYER_ACTOR.position.x + cos(angle) * LOOK_DISTANCE;
    pos.y = PLAYER_ACTOR.position.y + sin(angle) * LOOK_DISTANCE;

    if (pos.x < cam_min_x)
        pos.x = cam_min_x;
    else if (pos.x > cam_max_x)
        pos.x = cam_max_x;

    if (pos.y < cam_min_y)
        pos.y = cam_min_y;
    else if (pos.y > cam_max_y)
        pos.y = cam_max_y;

    camera_offset.x = pos.x - (SCREEN_WIDTH / 2);// - SQUARE_SIZE / 2;
    camera_offset.y = pos.y - (SCREEN_HEIGHT / 2);// - SQUARE_SIZE / 2;
}

void drawDot(Actor_t* actor)
{
    int offset_y;
    int offset_x;
    int pos_x;
    int pos_y;
    float dot_radians;
    int i;
    
    // calculate angle
    dot_radians = atan2(actor->direction.y, actor->direction.x);
    
    // directional dot's offsets from the center of the actor
    offset_y = sin(dot_radians) * DOT_DISTANCE - (int)camera_offset.y;
    offset_x = cos(dot_radians) * DOT_DISTANCE - (int)camera_offset.x;
    pos_x = actor->position.x + offset_x;
    pos_y = actor->position.y + offset_y;

    if (boundaryCheck_X(pos_x) == TRUE && boundaryCheck_Y(pos_y) == TRUE)
    {
        SET_PIXEL(pos_x, pos_y, COLOUR_WHITE);
    }
}

void increaseParticleRead()
{
    particle_read++;

    if (particle_read == MAX_PARTICLES)
        particle_read = 0;
}

void increaseParticleWrite()
{
    particle_write++;

    if (particle_write == MAX_PARTICLES)
        particle_write = 0;

    if (particle_write == particle_read)
        increaseParticleRead();
}

void decrementParticleWrite()
{
    particle_write--;

    if (particle_write < 0)
        particle_write = MAX_PARTICLES - 1;
}

void spawnParticle(Vec2 pos, Vec2 vel, uint8_t color, int8_t life)
{
    Particles[particle_write].pos.x = pos.x;
    Particles[particle_write].pos.y = pos.y;
    Particles[particle_write].vel.x = vel.x;
    Particles[particle_write].vel.y = vel.y;
    Particles[particle_write].color = color;
    Particles[particle_write].life = life;

    increaseParticleWrite();
}

void sprayParticles(Vec2 pos, Vec2 dir, float speed, float randomness, uint8_t num_particles, uint8_t start_color, uint8_t color_variance, int8_t life)
{
    Vec2 random_vec, vel;
    float random_scale;
    uint8_t color;
    int8_t random_life;
    int i;

    for (i = 0; i < num_particles; i++)
    {
        random_vec = getDirVec2(degToRad(rand() % 360));
        random_scale = (float)(rand() % 100) / 100.0;
        random_vec.x *= speed * random_scale * randomness;
        random_vec.y *= speed * random_scale * randomness;

        vel.x = dir.x * speed + random_vec.x;
        vel.y = dir.y * speed + random_vec.y;
        color = start_color + rand() % color_variance;
        random_life = (life / 2) + life * random_scale;
        spawnParticle(pos, vel, color, random_life);
    }
}

void particleFx(Vec2 pos, Vec2 dir, uint8_t fx_type)
{
    uint8_t start_color, color_variance, num_particles;
    int8_t life;
    float speed, randomness;

    switch (fx_type)
    {
    case FX_BLOOD: start_color = 176, color_variance = 15, num_particles = 20, 
    randomness = 0.8, speed = 1, life = 20; break;
    case FX_SPARKS: start_color = 160, color_variance = 7, num_particles = 15, 
    randomness = 0.5, speed = -5, life = 10; break;
    case FX_DIRT: start_color = 64, color_variance = 15, num_particles = 8, 
    randomness = 0.3, speed = 3, life = 10; break;
    case FX_WATERGUN: start_color = 196, color_variance = 5, num_particles = 15, 
    randomness = 0.1, speed = 10, life = 10; break;
    
    default: break;
    }
    if (boundaryCheck(((int)(pos.x - camera_offset.x)), ((int)(pos.y - camera_offset.y))) == TRUE)
        sprayParticles(pos, dir, speed, randomness, num_particles, start_color, color_variance, life);
}

void deleteParticle(int index)
{
    int last_particle = (particle_write == 0) ? MAX_PARTICLES - 1 : particle_write - 1;

    if (last_particle != particle_read)
        Particles[index] = Particles[last_particle];

    decrementParticleWrite();
}

void drawParticle(Vec2* pos, Vec2 vel, uint8_t color)
{
    pos->x += vel.x * System.ticks_per_frame;
    pos->y += vel.y * System.ticks_per_frame;
    if (boundaryCheck(((int)(pos->x - camera_offset.x)), ((int)(pos->y - camera_offset.y))) == TRUE)
        SET_PIXEL(((int)(pos->x - camera_offset.x)), ((int)(pos->y - camera_offset.y)), color);
}

void particleArrayManager()
{
    int i = particle_read;

    while (i != particle_write)
    {
        Particles[i].life -= System.ticks_per_frame;
        if (Particles[i].life <= 0)
        {
            deleteParticle(i);
            continue;
        }    
        drawParticle(&Particles[i].pos, Particles[i].vel, Particles[i].color);
        i++;
        if (i == MAX_PARTICLES)
        {
            i = 0;
        }
    }
}

void increaseCorpseRead()
{
    corpse_read++;

    if (corpse_read == MAX_CORPSES)
        corpse_read = 0;
}

void increaseCorpseWrite()
{
    corpse_write++;

    if (corpse_write == MAX_CORPSES)
        corpse_write = 0;

    if (corpse_write == corpse_read)
        increaseCorpseRead();
}

void decrementCorpseWrite()
{
    corpse_write--;

    if (corpse_write < 0)
        corpse_write = MAX_CORPSES - 1;
}

void spawnCorpse(Vec2 pos, double angle, int8_t life)
{
    Corpses[corpse_write].pos.x = pos.x;
    Corpses[corpse_write].pos.y = pos.y;
    Corpses[corpse_write].life = life;
    Corpses[corpse_write].sprite = saveRotatedTexture(angle, &ObjectTextures.textures[TEX_CORPSE], TRANSPARENT_COLOR);

    increaseCorpseWrite();
}

void deleteCorpse(int index)
{
    int last_corpse = (corpse_write == 0) ? MAX_CORPSES - 1 : corpse_write - 1;

    if (last_corpse != corpse_read)
        Corpses[index] = Corpses[last_corpse];

    decrementCorpseWrite();
}

void corpseArrayManager()
{
    int i = corpse_read;

    while (i != corpse_write)
    {
        if (Corpses[i].life != -1)
        {
            Corpses[i].life -= System.ticks_per_frame;
            if (Corpses[i].life <= 0)
            {
                deleteCorpse(i);
                continue;
            }
        }
        if (boundaryCheck(((int)(Corpses[i].pos.x - camera_offset.x)), ((int)(Corpses[i].pos.y - camera_offset.y))) == TRUE)
            drawTexturePartial(Corpses[i].pos.x - camera_offset.x, Corpses[i].pos.y - camera_offset.y, &Corpses[i].sprite);
        i++;
        if (i == MAX_CORPSES)
        {
            i = 0;
        }
    }
}

void emptyCorpseArray()
{
    int i;
    memset(Corpses, 0, sizeof(Corpse_t) * MAX_CORPSES);
    corpse_read = 0;
    corpse_write = 0;
}

// test function, delete/flesh out later
void drawAnim()
{
    static int frame_counter = 0;
    static int animation_counter = 0;
    static ticks_t last_frame_drawn;

    Texture_t* texture = &ObjectTextures.textures[Animations.anims[animation_counter].frame_ids[frame_counter]];

    drawTexture(110, 100, texture);

    if (last_frame_drawn + 10 < System.ticks)
    {
        last_frame_drawn = System.ticks;
        frame_counter++;
        if (frame_counter >= Animations.anims[animation_counter].num_frames)
        {
            frame_counter = 0;
            animation_counter++;
            if (animation_counter >= Animations.anim_count)
                animation_counter = 1;
        }
    }
}

void testDrawAnimFromName(int x, int y, char* name)
{
    static int frame_counter = 0;
    static ticks_t last_frame_drawn;
    int i;

    if ((i = findAnim(name)) != 0)
    {
        Texture_t* texture = &ObjectTextures.textures[Animations.anims[i].frame_ids[frame_counter]];

        drawTexture(x, y, texture);

        if (last_frame_drawn + 5 < System.ticks)
        {
            last_frame_drawn = System.ticks;
            frame_counter++;
            if (frame_counter >= Animations.anims[i].num_frames)
            {
                frame_counter = 0;
            }
        }
    }
    else
        drawTextureClipped(x, y, &ObjectTextures.textures[Animations.anims[TEX_ERROR].frame_ids[TEX_ERROR]]);
}

void drawAnimFromSprite(int x, int y, double angle, Sprite_t* sprite)
{
    Texture_t* texture;

    if (sprite->last_frame + sprite->frame_interval < System.ticks)
    {
        sprite->last_frame = System.ticks;
        sprite->frame++;
        if (sprite->frame >= Animations.anims[sprite->anim_id].num_frames)
        {
            sprite->frame = 0;
        }
    }

    texture = &ObjectTextures.textures[Animations.anims[sprite->anim_id].frame_ids[sprite->frame]];

    if (angle != 0.0)
        drawTextureRotated(x, y, angle, texture, TRANSPARENT_COLOR);
    else
        drawTexture(x, y, texture);
}

void drawSprite(int x, int y, double angle, Sprite_t* sprite)
{
    if (sprite->flags == SPRITE_IS_STATIC)
    {
        Texture_t* texture = &ObjectTextures.textures[Animations.anims[sprite->anim_id].frame_ids[0]];

        if (angle != 0.0)
            drawTextureRotated(x, y, angle, texture, TRANSPARENT_COLOR);
        else
            drawTexture(x, y, texture);
    }
    else
        drawAnimFromSprite(x, y, angle, sprite);
}

void setActorAnim(Actor_t* actor, int new_anim_type)
{
    actor->sprite.anim_id = actor->animset->anim_ids[new_anim_type];
    actor->sprite.frame = 0;
}

void drawActors()
{
    int i; // actor array "index"
    int start_x;
    int start_y;
    Texture_t* texture;
    Actor_t* actor;
    char str[8] = {0};

    for (i = 0; i < Game.actor_count; i++)
    {
        actor = &Game.Actors[i];
        texture = &ObjectTextures.textures[actor->texture_id];

        start_x = actor->position.x - camera_offset.x - texture->width / 2;
        start_y = actor->position.y - camera_offset.y - texture->height / 2;
        
        if ((i != 0) && boundaryCheckRadius(start_x, start_y, actor->radius) != FULLY_OUT && texture != NULL)
            drawTextureRotated(start_x, start_y, actor->angle, texture, TRANSPARENT_COLOR);
        else if (boundaryCheckRadius(start_x, start_y, actor->radius) != FULLY_OUT)
            drawSprite(start_x, start_y, PLAYER_ACTOR.angle, &PLAYER_ACTOR.sprite);

        drawDot(actor);

        #if DEBUG == 1
        str[0] = '\0';
        sprintf(str, "%u", actor->id);
        drawTextClipped(start_x, start_y - 10, str, COLOUR_YELLOW);
        #endif
    }
}

void drawDebug();

void drawHealth()
{
    char plr_health[10];

    sprintf(plr_health, "HP: %d", PLAYER_ACTOR.health);
    drawText(250, 190, plr_health, COLOUR_WHITE);
}

void drawStats()
{
    char cur_weapon[30];
    char cur_anim[15];

    sprintf(cur_weapon, "%s", PLAYER_ACTOR.primary_weapon->name);
    sprintf(cur_anim, "ANIM: %d", PLAYER_ACTOR.sprite.anim_id);
    drawText(2, 170, cur_anim, COLOUR_WHITE);
    drawText(2, 190, cur_weapon, COLOUR_WHITE);
}

void menuDraw()
{
    memset(screen_buf, 0, SCREEN_SIZE);
    drawMenuText();
}

void titleDraw()
{
    drawText(93, 96, "PRESS SPACE", COLOUR_BLUE);
}

void testRocketAngle()
{
    static double angle = 0.0;
    drawSprite(10, 80, angle, &Rocket);
    angle += 0.1;
    if (angle >= RAD_360)
        angle = 0;
}

void testSetPlayerAnim()
{
    if (PLAYER_ACTOR.magnitude != 0.0)
    {
        if (!KEY_IS_PRESSED(KEY_SPACEBAR) && PLAYER_ACTOR.sprite.anim_id != PLAYER_ACTOR.animset->anim_ids[ANIM_WALK])
            setActorAnim(&PLAYER_ACTOR, ANIM_WALK);
        //PLAYER_ACTOR.sprite.flags = SPRITE_IS_ANIM;
        else if (KEY_IS_PRESSED(KEY_SPACEBAR))
            if (PLAYER_ACTOR.primary_weapon == &Weapons[WEAPON_FIST] && PLAYER_ACTOR.sprite.anim_id != PLAYER_ACTOR.animset->anim_ids[ANIM_PUNCH_WALK])
                setActorAnim(&PLAYER_ACTOR, ANIM_PUNCH_WALK);
            else if (PLAYER_ACTOR.primary_weapon != &Weapons[WEAPON_FIST] && PLAYER_ACTOR.sprite.anim_id != PLAYER_ACTOR.animset->anim_ids[ANIM_SHOOT])
                setActorAnim(&PLAYER_ACTOR, ANIM_SHOOT);
    }
    else if (PLAYER_ACTOR.magnitude == 0.0)
    {
        if (!KEY_IS_PRESSED(KEY_SPACEBAR))
            setActorAnim(&PLAYER_ACTOR, ANIM_IDLE);
        else
            if (PLAYER_ACTOR.primary_weapon == &Weapons[WEAPON_FIST] && PLAYER_ACTOR.sprite.anim_id != PLAYER_ACTOR.animset->anim_ids[ANIM_PUNCH_STILL])
                setActorAnim(&PLAYER_ACTOR, ANIM_PUNCH_STILL);
            else if (PLAYER_ACTOR.primary_weapon != &Weapons[WEAPON_FIST] && PLAYER_ACTOR.sprite.anim_id != PLAYER_ACTOR.animset->anim_ids[ANIM_SHOOT])
                setActorAnim(&PLAYER_ACTOR, ANIM_SHOOT);
        //PLAYER_ACTOR.sprite.flags = SPRITE_IS_STATIC;
    }
}

void animTestBlock()
{
    drawAnim();
    testDrawAnimFromName(60, 80, "ANIMS/ROCKET.ANI");
    testDrawAnimFromName(60, 100, "ANIMS/EXPLO.ANI");
    testDrawAnimFromName(60, 140, "ANIMS/LOL.ANI");
    testRocketAngle();
    drawSprite(10, 120, 0.0, &Explosion);
}

void gameDraw()
{
    calcCameraOffset();
    drawMap();
    corpseArrayManager();
    testSetPlayerAnim();
    drawActors();
    drawHealth();
    drawStats();
    //animTestBlock();
    particleArrayManager();
    #if DEBUG == 1
    drawDebug();
    #endif
}

void pauseDraw()
{
    drawText(132, 96, "PAUSED", COLOUR_WHITE);
}

void ingameMenuDraw()
{
    //memset(screen_buf, 0, SCREEN_SIZE);
    drawMenuText();
}
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <math.h>
#include <stdint.h>

#define VIDEO_INT           0x10      /* the BIOS video interrupt. */
#define SET_MODE            0x00      /* BIOS func to set the video mode. */
#define VGA_256_COLOR_MODE  0x13      /* use to set 256-color mode. */
#define TEXT_MODE           0x03      /* use to set 80x25 text mode. */

#define SCREEN_WIDTH        320       /* width in pixels of mode 0x13 */
#define SCREEN_HEIGHT       200       /* height in pixels of mode 0x13 */
#define NUM_COLORS          256       /* number of colors in mode 0x13 */

#define SET_PIXEL(x,y,color)      screen_buf[(y)*SCREEN_WIDTH + (x)] = color

#define KB_ARRAY_LENGTH     256
#define KB_QUEUE_LENGTH     256

#define KEY_HIT_FLAG        1
#define KEY_PRESSED_FLAG    2
#define KEY_RELEASED_FLAG   0x80 // 128, MSB
#define KEY_SPECIAL_CODE    0xE0 // 224
#define KEY_IS_PRESSED(k)   (g_Keyboard[k] & KEY_PRESSED_FLAG)
#define KEY_WAS_HIT(k)      (g_Keyboard[k] & KEY_HIT_FLAG)
#define KEY_WAS_RELEASED(k) (g_Keyboard[k] & KEY_RELEASED_FLAG)
#define KEY_WAS_TOGGLED(k)  (g_Keyboard[k] & (KEY_HIT_FLAG|KEY_RELEASED_FLAG))
#define KEY_LEFT            75
#define KEY_RIGHT           77
#define KEY_UP              72 
#define KEY_DOWN            80
#define KEY_F10             68
 
#define X_AXIS              1
#define Y_AXIS              2

#define SQUARE_SIZE         20
#define SCREEN_SIZE         64000
#define SQUARE_ROWS         SCREEN_HEIGHT / SQUARE_SIZE
#define SQUARE_COLUMNS      SCREEN_WIDTH / SQUARE_SIZE
#define degToRad(degree)    ((degree) * (M_PI / 180.0))

#define WALL                7
#define COLOUR_RED          40
#define COLOUR_PEACH        64

#define player              object_array[0]
#define MAX_SPEED           10.0
#define BRAKE_RATE          0.9
#define SPEED_THRESHOLD     0.2
#define TURN_RATE           5

#define CHASE_DISTANCE      75
#define CHASE_DISTANCE_SQ   CHASE_DISTANCE*CHASE_DISTANCE

enum Objects
{
    Circle1,
    Circle2,
    Circle3,
    Num_Objects
};

typedef struct
{
    float x, y;
} Vec2;

typedef struct
{
    int x, y;
} Vec2_int;

typedef struct
{
    Vec2 position;
    Vec2 direction; // direction
    Vec2 velocity; // velocity
    float magnitude; // magnitude of velocity
    int radius; // circle radius
    uint8_t color;
} Object;

// array which holds all objects (circles in this case)
Object object_array[Num_Objects] = {
//    pos_x  pos_y     direction    velocity   magnitude     radius  colour
    {{120.0, 40.0,},  {0.0, 0.0}, {0.0, 0.0}, 0.0,          8,      14},
    {{40.0,  110.0,}, {0.0, 0.0}, {0.0, 0.0}, 0.0,          8,       3},
    {{120.0, 110.0,}, {0.0, 0.0}, {0.0, 0.0}, 0.0,          8,      12}
};

// array which determines the colour of each square on the grid
uint8_t grid_array [] = {7,  7, 7,  7, 7,  7, 7,  7, 7,  7, 7,  7, 7,  7, 7,  7,
                         7, 15, 0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 0,  7,
                         7,  0, 7,  7, 7, 0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 7,
                         7, 15, 7, 15, 0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 0,  7,
                         7,  0, 7, 0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 7,
                         7, 15, 0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 0,  7,
                         7,  0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 7,
                         7, 15, 0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 0,  7,
                         7,  0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 0, 15, 7,
                         7,  7, 7,  7, 7,  7, 7,  7, 7,  7, 7,  7, 7,  7, 7,  7
};

Vec2_int pl_grid; // vector to hold the player's grid location

struct Input
{
    uint8_t kb_array[KB_ARRAY_LENGTH];
    uint8_t kb_queue[KB_QUEUE_LENGTH];
    uint8_t kb_head;
    uint8_t kb_tail;
};

typedef struct Input Input_t;

uint8_t *VGA=(uint8_t *)0xA0000000L;        /* this points to video memory. */
uint8_t far screen_buf [64000];

Input_t Input = {0};
Input_t* g_Input = &Input;
uint8_t* g_Keyboard = Input.kb_array;

static void interrupt (far *old_keyhandler)(void);

int running = 1;
float drag = 1.05;
float thrust = 0.3;
int heading = 0; // current direction in degrees
float radians = 0.0; // current direction in radians, as that's what the math functions use

void set_mode(uint8_t mode)
{
    union REGS regs;

    regs.h.ah = SET_MODE;
    regs.h.al = mode;
    int86(VIDEO_INT, &regs, &regs);
}

// keyboard handling stuff until line 229
void interrupt far keyhandler()
{
    uint8_t key_scan = 0;
    static uint8_t key_scan_last = 0;
    static uint8_t key_special = 0;
    static uint8_t key_special_last = 0;
    // obtain scancode
    asm {
        cli // clear interrupt flag; prevent other external interrupts
        in  al, 0x60
        mov key_scan, al
        in  al, 0x61 // get keyboard status
        mov bl, al
        or  al, 0x80 // MSB set
        out 0x61, al // write (ACK)
        mov al, bl   // MSB clear
        out 0x61, al // write
    }
    // remember the last special key code
    if (key_scan_last == KEY_SPECIAL_CODE)
        key_special = key_scan;
    else if (key_scan != KEY_SPECIAL_CODE)
        key_special = 0;
    // place scancode on queue unless it's the same as the previous one
    if (key_scan != key_scan_last && (key_special != key_special_last || key_special_last == 0))
    {
        g_Input->kb_queue[g_Input->kb_tail] = key_scan;
        if (g_Input->kb_tail+1 != g_Input->kb_head)
            g_Input->kb_tail++;
    }
    key_scan_last = key_scan;
    key_special_last = key_special;
    // PIC 8259 reset
    asm {
        mov al, 0x20
        out 0x20, al
        sti // set interrupt flag; allow external interrupts again
    }
}

void init_keyboard()
{
    uint8_t far *bios_key_state;
    asm cli
    // save address of current keyhandler interrupt function
    old_keyhandler = _dos_getvect(9);
    // caps lock & num lock off
    bios_key_state = MK_FP(0x040, 0x017);
    *bios_key_state &= (~(32|64));
    old_keyhandler(); 
    // replace old keyhandler with new keyhandler function
    _dos_setvect(9, keyhandler);
    asm sti
}

void deinit_keyboard()
{
    // restore old keyhandler
    asm cli
    _dos_setvect(9, old_keyhandler);
    asm sti

    g_Input = NULL;
}

void get_keyboard()
{
    int i;
    uint8_t key_scan;
    uint8_t extended = 0;
    uint8_t released = 0;

    while (g_Input->kb_head != g_Input->kb_tail)
    {
        key_scan = g_Input->kb_queue[g_Input->kb_head++];
        /* handle special keys, but only if a second scancode follows*/
        if (key_scan == KEY_SPECIAL_CODE)
                extended = 128;
        /* remember the release flag before clearing it*/
        released = key_scan & KEY_RELEASED_FLAG;
        key_scan &= ~KEY_RELEASED_FLAG;
        if (released)
        {
            g_Input->kb_array[key_scan+extended] &= KEY_HIT_FLAG;
            g_Input->kb_array[key_scan+extended] |= KEY_RELEASED_FLAG;
        }
        else
            g_Input->kb_array[key_scan+extended] |= (KEY_HIT_FLAG|KEY_PRESSED_FLAG);

        extended = 0;
    }
}

void clear_keys()
{
    int i;
    for (i = 0; i < KB_ARRAY_LENGTH; i++)
        g_Input->kb_array[i] &= KEY_PRESSED_FLAG;
}

void control_ingame()
{
    player.direction.x = cos(radians); // calculate directional x-vector
    player.direction.y = sin(radians); // calculate directional y-vector
    
    if (KEY_IS_PRESSED(KEY_UP) && player.magnitude <= MAX_SPEED)
    {
        player.velocity.x += player.direction.x * thrust;
        player.velocity.y += player.direction.y * thrust;
        // calculate the player's current movement speed
        player.magnitude = sqrt((player.velocity.x * player.velocity.x) + (player.velocity.y * player.velocity.y));
    }
    if (KEY_IS_PRESSED(KEY_DOWN))
    {
        // reduce player velocity
        player.velocity.x *= BRAKE_RATE;
        player.velocity.y *= BRAKE_RATE;
            // if player has slowed down to a crawl, stop them entirely
            if (fabs(player.velocity.x) < SPEED_THRESHOLD)
            {
                player.velocity.x = 0.0;
            }
            if  (fabs(player.velocity.y) < SPEED_THRESHOLD)
            {
                player.velocity.y = 0.0;
            }
    }
    if (KEY_IS_PRESSED(KEY_LEFT))
    {
        heading -= TURN_RATE;
        if (heading < 0)
            heading = 360;
    }
    if (KEY_IS_PRESSED(KEY_RIGHT))
    {
        heading += TURN_RATE;
        if (heading > 360)
            heading = 0;
    }
}

void process_input()
{
    get_keyboard();

    control_ingame();
    
    // F10 always exits, wherever you are
    if (KEY_WAS_HIT(KEY_F10))
        running = 0;

    clear_keys();
}

void draw_circle(Vec2* position, int radius, uint8_t color)
{
    int offset_x;
    int offset_y;
    
    int center_x = position->x;
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

void draw_square(int x, int y, uint8_t colour)
{
    int index_y = 0;

    for (index_y=0;index_y<SQUARE_SIZE;index_y++)
    {
        //memsetting the entire row is faster than drawing it pixel by pixel
        memset(screen_buf + (y * SCREEN_WIDTH + x), colour, SQUARE_SIZE);
        y++;
    }
}

void player_grid_loc() // player circle's location on the grid
{   
    // calculated by dividing the player's x/y location by square size
    pl_grid.x = player.position.x / SQUARE_SIZE;
    pl_grid.y = player.position.y / SQUARE_SIZE;
}

void draw_stuff()
{
    int i = 0; // object array "index"
    int a = 0; // square drawing "index"
    int x;
    int y;
    
    int offset_y;
    int offset_x;
    
    // draw 160 20x20 squares (maximum that can fit on the 320x200 screen)
    while (a < SQUARE_ROWS * SQUARE_COLUMNS)
    {
        for (y = 0; y < SCREEN_HEIGHT; y += SQUARE_SIZE)
        {
            for(x = 0; x < SCREEN_WIDTH; x += SQUARE_SIZE)
            {
                draw_square(x, y, grid_array[a]);
                a++;
            }
        }
    }
    
    player_grid_loc(); // check where the player circle is
    draw_square(pl_grid.x * SQUARE_SIZE, pl_grid.y * SQUARE_SIZE, COLOUR_PEACH); // change that square to a lovely peach colour
    
    while (i < Num_Objects)
    {
        // draw all circles in their current locations
        draw_circle(&object_array[i].position, object_array[i].radius, object_array[i].color);
        i++;
    }
    
    // directional dot's offsets from the center of the circle
    offset_y = (sin(radians)) * player.radius;
    offset_x = (cos(radians)) * player.radius;
    // center of the circle has to be cast into int, otherwise the draw function doesn't work
    SET_PIXEL((int)player.position.x + offset_x, (int)player.position.y + offset_y, COLOUR_RED);
    
    // copy off-screen buffer to VGA memory
    memcpy(VGA,screen_buf,SCREEN_SIZE);
    
    // clear off-screen buffer so the screen updates properly
    _fmemset(screen_buf, 0, SCREEN_SIZE);
}

int tile_detect(Vec2 pos)
{
    int object_tile; // tile which the object is on (or attempting to be), i.e. array index number from grid_array
    uint8_t tile_colour;
    
    // calculate current grid position
    pos.x /= SQUARE_SIZE;
    pos.y /= SQUARE_SIZE;
    
    // check which grid_array index it corresponds to
    object_tile = (int)pos.y * SQUARE_COLUMNS + (int)pos.x;
    
    tile_colour = grid_array[object_tile]; // check which colour is at that index
    
    return tile_colour; // return said colour
}

void edge_detect()
{
    int i = 0;
    while (i < Num_Objects)
    {
        if (object_array[i].position.x - object_array[i].radius <= 0) // left edge
        {
            object_array[i].position.x = 0 + object_array[i].radius;
            object_array[i].velocity.x = 0.0;
        }
        
        if (object_array[i].position.y - object_array[i].radius <= 0) // top edge
        {
            object_array[i].position.y = 0 + object_array[i].radius;
            object_array[i].velocity.y = 0.0;
        }
        
        if (object_array[i].position.x + object_array[i].radius >= (SCREEN_WIDTH - 1)) // right edge
        {
            object_array[i].position.x = (SCREEN_WIDTH - 1) - object_array[i].radius;
            object_array[i].velocity.x = 0.0;
        }
        
        if (object_array[i].position.y + object_array[i].radius >= (SCREEN_HEIGHT - 1)) // bottom edge
        {
            object_array[i].position.y = (SCREEN_HEIGHT - 1) - object_array[i].radius;
            object_array[i].velocity.y = 0.0;
        }
        i++;
    }
}

void move_circle(Object* obj, Vec2 movement)
{
    // collision box around the object
    Vec2 test_point_a;
    Vec2 test_point_b;
    
    if (movement.x > 0) // if moving to the right
    {
        obj->position.x += movement.x; // temporarily move the object
        // test_point_a = top right corner
        test_point_a.x = obj->position.x + obj->radius;
        test_point_a.y = obj->position.y - obj->radius;
        // test_point_b = bottom right corner
        test_point_b.x = obj->position.x + obj->radius;
        test_point_b.y = obj->position.y + obj->radius;
        
        // if the movement would result in the object moving inside of a wall...
        if (tile_detect(test_point_a) == WALL || tile_detect(test_point_b) == WALL)
        {
            // ...cancel movement
            obj->position.x -= movement.x;
        }
    }
    else if (movement.x < 0) // if moving to the left
    {
        obj->position.x += movement.x;
        // test_point_a = top left corner
        test_point_a.x = obj->position.x - obj->radius;
        test_point_a.y = obj->position.y - obj->radius;
        // test_point_b = bottom left corner
        test_point_b.x = obj->position.x - obj->radius;
        test_point_b.y = obj->position.y + obj->radius;
        
        if (tile_detect(test_point_a) == WALL || tile_detect(test_point_b) == WALL)
        {
            obj->position.x -= movement.x;
        }
    }

    if (movement.y < 0) // if moving towards the top
    {
        obj->position.y += movement.y;
        // test_point_a = top right corner
        test_point_a.x = obj->position.x + obj->radius;
        test_point_a.y = obj->position.y - obj->radius;
        // test_point_ba = top left corner
        test_point_b.x = obj->position.x - obj->radius;
        test_point_b.y = obj->position.y - obj->radius;
        
        if (tile_detect(test_point_a) == WALL || tile_detect(test_point_b) == WALL)
        {
            obj->position.y -= movement.y;
        }
    }
    else if (movement.y > 0) // if moving towards the bottom
    {
        obj->position.y += movement.y;
        // test_point_a = bottom right corner
        test_point_a.x = obj->position.x + obj->radius;
        test_point_a.y = obj->position.y + obj->radius;
        // test_point_b = bottom left corner
        test_point_b.x = obj->position.x - obj->radius;
        test_point_b.y = obj->position.y + obj->radius;
        
        if (tile_detect(test_point_a) == WALL || tile_detect(test_point_b) == WALL)
        {
            obj->position.y -= movement.y;
        }
    }
}

void calculate_movement()
{
    int i = 0;
    
    // iterate through the object array
    while (i < Num_Objects)
    {
        move_circle(&object_array[i], object_array[i].velocity); // check each circle for wall collisions
        // reduce object velocity with aerial drag
        object_array[i].velocity.y /= drag;
        object_array[i].velocity.x /= drag;
        i++;
    }
}

void chase(Object* object_a, Object* object_b)
{
    float ai_radians;
    
    float distance_x = object_a->position.x - object_b->position.x; // x-distance between the two objects
    float distance_y = object_a->position.y - object_b->position.y; // y-distance between the two objects
    // simplified total distance calculation (without sqrt)
    float distance_squared = distance_x * distance_x + distance_y * distance_y;
    
    // calculate angle between the two objects - atan2 allows for both positive and negative angles
    ai_radians = atan2(distance_y, distance_x);
    
    // calculated the directional vector of the "chasing" object
    object_b->direction.x = cos(ai_radians);
    object_b->direction.y = sin(ai_radians);
    
    // if close enough, initiate "chase"
    if(distance_squared < CHASE_DISTANCE_SQ)
    {
        object_b->velocity.x += object_b->direction.x / 2;
        object_b->velocity.y += object_b->direction.y / 2;
    }
    
    // otherwise stop gradually
    else
    {
        object_b->direction.x = 0;
        object_b->direction.y = 0;
    }
}

void collision_detect(Object* object_a, Object* object_b)
{
    float distance_x;
    float distance_y;
    float distance;
    float collision_depth;
    int tile_colour;
    Vec2 u; // how much each object moves in case of a collision
    
    distance_x = object_a->position.x - object_b->position.x;  // x-distance between the two objects
    distance_y = object_a->position.y - object_b->position.y;  // y-distance between the two objects
    
    // actual distance between the two objects (hypotenuse of the x and y distances)
    distance = sqrt((distance_x * distance_x) + (distance_y * distance_y));
    
    // if distance is less than combined radiuses
    if (distance < (object_a->radius + object_b->radius))
    {
        // calculate how much the objects are "inside" each other
        collision_depth = (object_a->radius + object_b->radius) - distance;
        
        //each object is moved for half of that
        u.x = (distance_x/distance) * (collision_depth/2);
        u.y = (distance_y/distance) * (collision_depth/2);
        
        // first object gets the values as is...
        move_circle(object_a, u);
        
        // ...and for the second object they are flipped
        u.x = -u.x;
        u.y = -u.y;
        move_circle(object_b, u);
    }
}

void collision()
{
    int x;
    int i;
    
    // iterate through each object pair to see if they collide
    for (i = 0; i < Num_Objects; i++)
    {
        for (x = i; x < Num_Objects-1; x++)
        {
            collision_detect(&object_array[i], &object_array[x+1]);
        }
    }
    
    // also check that none of the objects are going beyond the screen boundaries
    edge_detect();
}

void quit()
{   
    deinit_keyboard();
    set_mode(TEXT_MODE);
}

void main()
{    
    set_mode(VGA_256_COLOR_MODE);
    init_keyboard();
    
    while (running == 1)
    {
        process_input();
        radians = degToRad(heading);
        chase(&object_array[0], &object_array[2]);
        chase(&object_array[2], &object_array[1]);
        calculate_movement();
        collision();
        draw_stuff();
        delay(50);
    }
    quit();
}
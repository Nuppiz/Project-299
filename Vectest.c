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
#define degToRad(degree)    ((degree) * M_PI / 180.0)

#define RAD_360             (2*M_PI)
#define RAD_270             (4.172)
#define RAD_180             M_PI
#define RAD_120             (M_PI/1.5)
#define RAD_105             (M_PI-1.3)
#define RAD_90              (M_PI/2)
#define RAD_60              (M_PI/3)
#define RAD_45              (M_PI/4)
#define RAD_30              (M_PI/6)
#define RAD_15              (M_PI/12)

#define WALL                7
#define COLOUR_RED          40
#define COLOUR_PEACH        64

#define player              object_array[0]
#define MAX_SPEED           10.0
#define ACCELERATION_RATE   2.0
#define BRAKE_RATE          0.9
#define SPEED_THRESHOLD     0.2
#define TURN_RATE           5

#define CHASE_DISTANCE      75
#define CHASE_DISTANCE_SQ   CHASE_DISTANCE*CHASE_DISTANCE
#define CHASE_TIMEOUT       100

#define TILE_WIDTH          8
#define TILE_HEIGHT         8
#define CHARACTER_SIZE      72

#define IDLE                0
#define CHASE_TARGET        1

uint8_t alphabet [4240];

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
    Vec2_int grid_loc; // location on the grid
    Vec2 direction; // direction
    Vec2 velocity; // velocity
    float magnitude; // magnitude of velocity
    int radius; // circle radius
    uint8_t color;
    uint8_t ai_mode;
    int ai_timer;
    Vec2* ai_target;
} Object;

// array which holds all objects (circles in this case)
Object object_array[Num_Objects] = {
//    pos_x  pos_y    grid_x grid_y    direction    velocity    magnitude radius  colour  ai_mode       ai_timer  ai_target
    {{28.0, 40.0,},  {1,     2,},     {1.0, 1.0},  {0.0, 0.0}, 0.0,      8,      14,     IDLE,         0,        &object_array[2].position},
    {{40.0,  110.0,}, {2,     5,},     {1.0, 1.0},  {0.0, 0.0}, 0.0,      8,       3,     IDLE, 100,      &object_array[2].position},
    {{270.0, 160.0,},  {13,     9,},     {1.0, 1.0},  {0.0, 0.0}, 0.0,      8,      12,     IDLE, 100,      &player.position}
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

void load_font()
{
    FILE* file_ptr;
    file_ptr = fopen("FONT.7UP", "rb");
    fread(alphabet, 1, 4240, file_ptr);
    fclose(file_ptr);
}

void draw_text(int x, int y, int i, uint8_t color)
{
    uint8_t index_x = 0;
    uint8_t index_y = 0;
    i = i * CHARACTER_SIZE;

    for (index_y=0;index_y<TILE_HEIGHT;index_y++)
    {
        for (index_x=0;index_x<TILE_WIDTH;index_x++)
        {
            if (alphabet[i] != 13)
            {
                SET_PIXEL(x, y, alphabet[i] + color);
                i++;
                x++;
            }
            else
            {
                i++;
                x++;
            }
        }
        index_x = 0;
        x = x - TILE_WIDTH;
        y++;
    }
    index_y = 0;
    i= 0;
}

void render_text(int x, int y, char* string, uint8_t color)
{
    int i = 0;
    char c;
    
    while (string[i] != 0)
    {
        c = string[i];
        draw_text(x, y, c - 32, color);
        x = x + 10;
        i++;
    }
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

void draw_dot(Object* obj)
{
    int offset_y = 0;
    int offset_x = 0;
    float dot_radians;
    
    // calculate angle
    dot_radians = atan2(obj->direction.y, obj->direction.x);
    
    // directional dot's offsets from the center of the circle
    offset_y = sin(dot_radians) * obj->radius;
    offset_x = cos(dot_radians) * obj->radius;
    // center of the circle has to be cast into int, otherwise the draw function doesn't work
    SET_PIXEL((int)obj->position.x + offset_x, (int)obj->position.y + offset_y, obj->color + 64);
}

void check_grid_loc(Object* obj) // circle's location on the grid
{   
    // calculated by dividing the circle's x/y location by square size
    obj->grid_loc.x = obj->position.x / SQUARE_SIZE;
    obj->grid_loc.y = obj->position.y / SQUARE_SIZE;
}

void draw_stuff()
{
    int i = 0; // object array "index"
    int a = 0; // square drawing "index"
    int x;
    int y;
    
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
    
    // change player square to a lovely peach colour
    draw_square(object_array[0].grid_loc.x * SQUARE_SIZE, object_array[0].grid_loc.y * SQUARE_SIZE, COLOUR_PEACH);
    
    while (i < Num_Objects)
    {
        // draw all circles in their current locations
        draw_circle(&object_array[i].position, object_array[i].radius, object_array[i].color);
        draw_dot(&object_array[i]);
        i++;
    }
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
    
    check_grid_loc(obj);
    
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
            // ...cancel movement and set velocity to 0
            obj->position.x = (obj->grid_loc.x + 1) * SQUARE_SIZE - obj->radius - 1;
            obj->velocity.x = 0.0;
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
            obj->position.x = obj->grid_loc.x * SQUARE_SIZE + obj->radius;
            obj->velocity.x = 0.0;
        }
    }

    if (movement.y < 0) // if moving towards the top
    {
        obj->position.y += movement.y;
        // test_point_a = top right corner
        test_point_a.x = obj->position.x + obj->radius;
        test_point_a.y = obj->position.y - obj->radius;
        // test_point_b = top left corner
        test_point_b.x = obj->position.x - obj->radius;
        test_point_b.y = obj->position.y - obj->radius;
        
        if (tile_detect(test_point_a) == WALL || tile_detect(test_point_b) == WALL)
        {
            obj->position.y = obj->grid_loc.y * SQUARE_SIZE + obj->radius;
            obj->velocity.y = 0.0;
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
            obj->position.y = (obj->grid_loc.y + 1) * SQUARE_SIZE - obj->radius - 1;
            obj->velocity.y = 0.0;
        }
    }
    check_grid_loc(obj);
}

void calculate_movements()
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

Vec2 getVec2(Vec2 p0, Vec2 p1)
{
    Vec2 v;
    v.x = p1.x - p0.x;
    v.y = p1.y - p0.y;
    
    return v;
}

int dotVec2(Vec2 v1, Vec2 v2)
{
    // dot product is the result of two vectors combined into a single number
    float dot_product = (v1.x * v2.x) + (v1.y * v2.y);
    char ve_str[64];
    char dp_str[24];
    sprintf(ve_str, "DV: %.2f %.2f %.2f %.2f", v1.x, v1.y, v2.x, v2.y);
    render_text(0, 0, ve_str, 0);
    sprintf(dp_str, "DP: %f", dot_product);
    render_text(0, 10, dp_str, 0);
    
    return dot_product;
}

int getVec2Length(Vec2 v)
{
    float Vec2Length = sqrt((v.x * v.x) + (v.y * v.y));
    char vl_str[24];
    sprintf(vl_str, "VL: %f", Vec2Length);
    render_text(0, 20, vl_str, 40);
    
    return Vec2Length;
}

int getVec2Angle(Vec2 v1, Vec2 v2)
{
    /*to calculate the angle between two vectors, we first multiply the directional vector
    magnitudes with each other...
    then divide the dot product with that...
    and take arc cosine from the end result, this will give us the angle*/
    
    char cos_str[24];
    char va_str[24];
    float dot_product = dotVec2(v1, v2);
    double cos = dot_product / getVec2Length(v1);
    float vector_angle = acos(cos);
    
    sprintf(cos_str, "COS: %f", cos); 
    sprintf(va_str, "VA: %f", vector_angle);
    render_text(0, 30, cos_str, 40);
    render_text(0, 40, va_str, 40);
    
    return vector_angle;
}

int testFieldOfView(Vec2 origin, Vec2 direction, Vec2* target)
{
    Vec2 origin_to_target = getVec2(*target, origin);
    float distance = getVec2Length(origin_to_target);
    float angle;
    
    if (distance < CHASE_DISTANCE)
    {
        angle = getVec2Angle(origin_to_target, direction);

        if (angle > RAD_120)
        {
             return 1;
        }
    }
    return 0;
}

Vec2 normalizeVec2(Vec2 v)
{
    Vec2 normalizedVec;
    
    float vec_length = getVec2Length(v);
    
    normalizedVec.x = v.x / vec_length;
    normalizedVec.y = v.y / vec_length;
    
    return normalizedVec;
}

void chaseTarget(Object* chaser)
{
    Vec2 ObjectToTarget = getVec2(chaser->position, *chaser->ai_target);
    chaser->direction = normalizeVec2(ObjectToTarget);    
    chaser->magnitude = getVec2Length(chaser->velocity);
    
    if (chaser->magnitude <= MAX_SPEED)
    {
        chaser->velocity.x += chaser->direction.x * ACCELERATION_RATE;
        chaser->velocity.y += chaser->direction.y * ACCELERATION_RATE;
    }
}

void think(Object* obj)
{
    if (obj->ai_mode == CHASE_TARGET)
    {
        if (obj->ai_timer > 0)
        {
            chaseTarget(obj);
            obj->ai_timer--;
        }
        else
        {
           obj->ai_mode = IDLE;
        }
    }
    else if (testFieldOfView(obj->position, obj->direction, obj->ai_target) == 1)
    {
        obj->ai_mode = CHASE_TARGET;
        obj->ai_timer = 100;
    }
}

void ai_loop()
{
    int i = 2;
    
    while (i < Num_Objects)
    {
        think(&object_array[i]);
        i++;
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

void render()
{     
    // copy off-screen buffer to VGA memory
    memcpy(VGA,screen_buf,SCREEN_SIZE);

    // clear off-screen buffer so the screen updates properly
    _fmemset(screen_buf, 0, SCREEN_SIZE);
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
    load_font();
    
    while (running == 1)
    {
        process_input();
        radians = degToRad(heading);
        ai_loop();
        calculate_movements();
        collision();
        render();
        draw_stuff();
        delay(50);
    }
    quit();
}
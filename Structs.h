#ifndef STRUCTS_H
#define STRUCTS_H

#include "Common.h"

typedef struct
{
    int    running;
    int    paused;      // stop ticks if in menu/paused
    time_t time;        // global timer
    time_t seconds;     // second timer (time/1000)
    ticks_t ticks;       // total game ticks
    time_t frames;      // total frames drawn
    time_t tick_interval;   // target time interval of logic cycles
    time_t frame_interval;  // target time interval between draws
    int    tick_rate;   // target rate of logic cycles
    int    frame_rate;  // target frame rate
    int    fps;         // actual measured fps
    float  fps_avg;     // average fps (frames/seconds)
    int    ticks_per_frame; // number of ticks per frame, varies based on frame rate
} System_t;

typedef struct 
{
    time_t last_midas_time; // used for synchronising the MIDAS Sound System
    time_t last_time; // Used for accumulating seconds & FPS calculation
    ticks_t last_tick; // Tracks time elapsed since last tick started
    time_t last_frame; // Tracks time elapsed since last draw started
    time_t accumulator; // Incremented by frame draw duration, decremented by ticks
    int frame_count; // Counts frames in a second so far; used by debug
    ticks_t last_env_damage; // timer for environmental damage
    ticks_t last_sfx; // timer for sound effects
} Timer_t;

typedef struct
{
    uint8_t keycode;
    uint8_t type;
    time_t time;
} KeyEvent_t;

typedef struct
{
    KeyEvent_t queue[KB_QUEUE_LENGTH];
    uint8_t keystates[KB_ARRAY_LENGTH];
    uint8_t queue_head;
    uint8_t queue_tail;
} Keyboard_t;

typedef struct
{
    uint8_t keycode;
    uint8_t state;
    ticks_t time;
} InputEvent_t;

typedef struct
{
    InputEvent_t EventQueue[KB_QUEUE_LENGTH];
    uint8_t KeyStates[KB_ARRAY_LENGTH]; // TO-DO: split into multiple arrays
    uint8_t queue_head;
    uint8_t queue_tail;
} Input_t;

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
    char* filename;
    uint8_t* pixels;
    uint16_t width, height;
    uint16_t transparent;
    uint8_t material_type;
    int offset_x, offset_y;
} Texture_t;

typedef struct
{
    Texture_t* textures;
    int texture_count;
} Texture_array;

typedef struct
{
    char* name;
    int* frame_ids;
    int num_frames;
    time_t frame_interval;
} Anim_t;

typedef struct
{
    Anim_t* anims;
    int anim_count;
} Anim_array;

typedef struct
{
    int anim_ids[NUM_ACTORANIMS];
} AnimSet_t;

typedef struct
{
    uint8_t flags;
    int anim_id;
    int frame;
    time_t last_frame;
    time_t frame_interval;
} Sprite_t;

typedef struct
{
    uint8_t draw_type; // run once or until a "kill" command
    uint8_t move_type; // static or moving
    uint8_t status; // active/inactive
    Vec2 pos;
    Vec2 vel;
    double angle;
    int frame;
    Sprite_t* sprite;
} TempSprite_t;

typedef struct
{
    id_t id; // id number in the weapons array
    char name[20];
    uint16_t range; // absolute maximum range for projectiles/hitting the target (if melee)
    uint16_t damage; // base damage per projectile/hit/etc.
    uint8_t num_projectiles; // number of projectiles per shot (e.g. a shotgun fires multiple pellets)
    int8_t projectile_speed; // if -1, instant (hitscan)
    uint8_t projectile_spread; // maximum of how much each projectile diverts from the targeted point (i.e. accuracy)
    ticks_t shot_delay; // delay between each shot/swing/whatever
    int8_t ammo_type; // if -1, infinite ammo/melee
    uint8_t sound_id; // id number to the enum table that contains sound effects
    int16_t sprite_id; // id number for projectile sprite (if not hitscan), -1 for none
    int8_t effect_id; // effect to spawn on hit (explosion, poison etc.), -1 for no effect
} Weapon_t;

typedef struct
{
    uint8_t sound_id; // id number to the enum table that contains sound effects
    uint16_t damage; // damage at the center of the effect
    uint16_t radius; // radius for explosion / poison splash etc.
    id_t sprite_id; // animation graphic
} WeaponEffect_t;

typedef struct
{
    id_t source_id; // id number of the shooter (for infighting etc.)
    Vec2 origin;
    Vec2 position;
    Vec2 velocity;
    double angle;
    Vec2_int grid_loc;
    uint16_t max_range;
    uint16_t damage;
    int sprite_id; // id in the TempSprites array
    int8_t effect_id; // effect to spawn on "death"
    uint8_t state;
} Projectile_t;

typedef struct
{
    char* name;
    float walk_speed;
    float run_speed;
    float turn_rate;
    int radius; // collision box radius
    int health;
    id_t primary_weapon_id;
    id_t secondary_weapon_id;
    AnimSet_t animset;
} ActorTemplate_t;

typedef struct
{
    id_t     id;
    flags_t  flags;
    Vec2     position;
    Vec2_int grid_loc;  // location on the grid
    Vec2     direction; // direction
    double   angle;     // current direction in radians
    Vec2     velocity;  // velocity
    float    magnitude; // magnitude of velocity
    int      radius;    // collision box/circle radius
    
    flags_t control;
    uint8_t ai_mode;
    ticks_t ai_timer;
    id_t    target_id_primary; // index number of the AI's primary target in the ActorsById array
    id_t    target_id_secondary; // index number of the AI's secondary target in the ActorsById array
    Vec2    move_target;

    uint8_t color;
    id_t texture_id;
    int health;
    id_t trigger_on_death; // entity ID to trigger on death
    ticks_t last_shot; // last shot taken
    int primary_weapon_id;
    int secondary_weapon_id;
    Sprite_t sprite;
    AnimSet_t* animset;
} Actor_t;

typedef struct
{
    uint8_t r,g,b;
} Color_t;

typedef struct
{
    Color_t colors[256];
} Palette_t;

typedef struct {
    uint8_t texture_id : 7;
    uint8_t obstacle : 1;
    uint8_t block_bullets : 1;
    uint8_t is_entity : 1;
    uint8_t entity_value : 5; // id in Entities array
} Tile_t;

typedef struct
{
    int level_num; // current level number
    int width; // width in tiles
    int height; // height in tiles
    Tile_t* tilemap;
} Map_t;

typedef struct {
    uint8_t x, y, id;
    uint8_t state : 1;
    uint8_t type : 7;
    union u_data {
        struct t_door {uint8_t locked : 1; uint8_t key : 7;} door;
        struct t_button {uint8_t target;} button;
        struct t_spawner {double angle; ticks_t last_spawn_time; ticks_t spawn_time_interval;
        int8_t max_actors; uint8_t num_actors; uint16_t spawn_type: 5;
        id_t trigger_on_death: 5; uint16_t toggleable : 1; uint16_t only_once : 1;} spawner;
        struct t_trigger {ticks_t last_trigger_time; ticks_t trigger_interval; int8_t target_ids[4]; uint8_t only_once;} trigger;
        struct t_counter {uint16_t value : 5; uint16_t max_value : 5; uint16_t target_id : 5; uint16_t only_once : 1;} counter;
        struct t_portal {char level_name[30]; int x; int y; double angle;} portal;
    } data;
} Entity_t;

typedef struct {
    int index; // tilemap index
    uint8_t state : 1; // enabled/disabled
    uint8_t type : 7; // type of item from Items enum
} Item_t;

typedef struct
{
    Map_t Map;
    char current_level_name[15];
    Actor_t* Actors;
    id_t* ActorsById;
    id_t actor_count;
    id_t actor_capacity;
    id_t id_capacity;
    uint8_t item_count;
    uint8_t item_capacity;
    id_t player_id;
} GameData_t;

typedef void (*fnp)(); // function pointer

typedef struct {
    fnp init;
    fnp input;
    fnp update;
    fnp draw;
    fnp exit;
    flags_t flags;
} State_t;

typedef struct
{
    char* text;
    fnp action;
} Option_t;

typedef struct
{
    //char* bgfilename;
    int num_selections;
    int cursor_loc;
    int start_y;
    int cursor_y;
    int cursor_x;
    int cursor_spacing;
    Option_t* options;
} Menu_t;

typedef struct {
    Vec2 pos;
    Vec2 vel;
    int8_t life;
    uint8_t color;
} Particle_t;

struct SFX_file {
    char* filename;
    uint8_t looping;
};

typedef struct {
    Vec2 pos;
    int life;
    Texture_t sprite;
} Corpse_t;

typedef struct
{
    char* buffer;
    int capacity;
    int length;
    int cursor;
    int start;
} TextInput_t;

typedef struct
{
    char** list;
    int count;
} StringList_t;

#endif/* STRUCTS_H */
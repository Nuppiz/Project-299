#ifndef ENUMS
#define ENUMS

enum TextureCount
{
    DUDE1,
    DUDE2,
    DUDE3,
    CAR,
    NUM_TEXTURES
};

enum TileCount
{
    DIRT,
    GRASS,
    GRAVEL,
    WATER,
    FLOOR_BATH,
    FLOOR_WOOD,
    WALL_BRICKS,
    WALL_C_HOR,
    WALL_C_VER,
    WALL_C_LL,
    WALL_C_LR,
    WALL_C_UL,
    WALL_C_UR,
    WALL_C_INTER,
    WALL_C_T_DOWN,
    WALL_C_T_LEFT,
    WALL_C_T_RIGHT,
    WALL_C_T_UP,
    PATH_D_HOR,
    PATH_D_VER,
    PATH_D_UR,
    PATH_D_UL,
    PATH_D_DR,
    PATH_D_DL,
    PATH_D_INTER,
    PATH_D_T_DOWN,
    PATH_D_T_LEFT,
    PATH_D_T_RIGHT,
    PATH_D_T_UP,
    BA_TOILETD,
    BA_TOILETL,
    BA_TOILETR,
    BA_TOILETU,
    NUM_TILES
};

enum DebugIndex
{
    DEBUG_FPS,
    DEBUG_INPUT,
    DEBUG_VELOCITY,
    DEBUG_AICHASE,
    DEBUG_TESTFOV,
    DEBUG_CLOCK,
    DEBUG_SHOOT,
    NUM_DEBUG
};

enum AiMode
{
    AI_NONE,
    AI_IDLE,
    AI_CHASE
};

enum StateIndex
{
    STATE_TITLE,
    STATE_INGAME,
    STATE_PAUSE,
    NUM_STATES
};

#endif/* ENUMS */

#ifndef ENUMS
#define ENUMS

enum ParticleEffects
{
    FX_BLOOD,
    FX_DIRT,
    FX_SPARKS,
    FX_WATERGUN
};

enum TextureMaterials
{
    MAT_DEFAULT,
    MAT_GRASS
};

enum EntityTypes
{
    ENT_DOOR,
    ENT_BUTTON
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

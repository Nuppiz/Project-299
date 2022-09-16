#ifndef ENUMS_H
#define ENUMS_H

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
    ENT_BUTTON,
    ENT_SPAWNER,
    ENT_TRIGGER,
    ENT_COUNTER,
    ENT_PORTAL,
    ENT_DELETED,
    NUM_ENTITYTYPES
};

enum EntityUseModes
{
    USE_DIRECTLY,
    USE_VIA_BUTTON
};

enum InteractiveTile
{
    NONE,
    TILE_KEY_RED,
    TILE_KEY_BLUE,
    TILE_KEY_YELLOW,
    TILE_SPIKES,
    NUM_INTERACTIVE_TILES
};

enum SoundEffects
{
    SOUND_SHOOT,
    SOUND_SWITCH,
    SOUND_LOCKED,
    SOUND_ITEM,
    SOUND_DOOR_O,
    SOUND_DOOR_C,
    SOUND_HURT,
    SOUND_HURT_E,
    SOUND_DEATH,
    SOUND_DEATH_E,
    SOUND_PORTAL,
    NUM_SFX
};

enum BaseTextures
{
    TEX_ERROR,
    TEX_CORPSE,
    TEX_KEY
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
    DEBUG_ENTITIES,
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
    STATE_MENU,
    STATE_INGAME,
    STATE_PAUSE,
    NUM_STATES
};

#endif/* ENUMS_H */

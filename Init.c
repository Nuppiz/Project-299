#include "Common.h"
#include "Structs.h"
#include "LvlLoad.h"
#include "Loadgfx.h"
#include "Sound.h"

extern System_t System;
extern State_t States[];
extern GameData_t Game;

#if DEBUG == 1
char debug[NUM_DEBUG][DEBUG_STR_LEN];
#endif

static void interrupt (far *old_Timer_ISR)(void);
static void interrupt (far *midas_Timer_ISR)(void);

unsigned short setTimerBxHookBx;
unsigned char recomputeMidasTickRate = 0;
unsigned int midasTickRate = 1000;

void setTimerBxHook()
{
    // compute the expected tick rate (as 8.8 fix) with given bx
    asm {
        pushf
        cmp setTimerBxHookBx, bx
        jz skipRecompute
        mov setTimerBxHookBx, bx
        inc recomputeMidasTickRate
    }
skipRecompute:
    asm {
        popf
    }
}

void interrupt far Timer(void)
{
    static long last_clock_time = 0;
    static long last_midas_time = 0;

    asm pushf;
    asm cli;

    ++System.time;

    if (recomputeMidasTickRate)
    {
        midasTickRate = 1000UL / (1193100UL / setTimerBxHookBx);
        last_midas_time = System.time;
        recomputeMidasTickRate = 0;
    }

    if (last_midas_time + midasTickRate < System.time)
    {
        last_midas_time = System.time;
        midas_Timer_ISR();
    }

    // keeps the PC clock ticking in the background
    if (last_clock_time + 55 < System.time)
    {
        last_clock_time = System.time;
        old_Timer_ISR();
    } 
    else
    {
        outportb(PIC2_COMMAND, PIC_EOI);
        outportb(PIC1_COMMAND, PIC_EOI);
    }

    asm popf;
}

void interrupt far stubISR(void) {

}

void setTimer(uint16_t new_count)
{
    outportb(CONTROL_8253, CONTROL_WORD);
    outportb(COUNTER_0, LOW_BYTE(new_count));
    outportb(COUNTER_0, HIGH_BYTE(new_count));
}

#if DEBUG == 1
void initDebug()
{
    int i;
    for (i = 0; i > NUM_DEBUG; i++)
        debug[i][0] = '\0';
}
#endif

void initSystem()
{
    System.running    = 1;
    System.time       = 0;
    System.seconds    = 0;
    System.ticks      = 0;
    System.frames     = 0;
    System.tick_interval  = 1000/TICK_RATE;
    System.frame_interval = 1000/FRAME_RATE;
    System.tick_rate  = TICK_RATE;
    System.frame_rate = FRAME_RATE;
    System.fps        = 0;
    System.fps_avg    = 0;
}

void soundInit()
{
    printf("Initializing sounds...\n");
    patchMidasSetTimer(&setTimerBxHook);
    asm cli;
    old_Timer_ISR = _dos_getvect(TIME_KEEPER_INT);
    // provide stub ISR to MIDAS so that it doesn't do anything
    _dos_setvect(TIME_KEEPER_INT, &stubISR);
    initSounds();
	printf("OK\n");
}

void timerInit()
{
    printf("Initializing timer...\n");
    midas_Timer_ISR = _dos_getvect(TIME_KEEPER_INT);
    _dos_setvect(TIME_KEEPER_INT, Timer);
    setTimer(TIMER_1000HZ);
    asm sti;
	printf("OK\n");
}

void gfxInit()
{
    extern Palette_t NewPalette;

	printf("Initializing graphics...\n");
    setVideoMode(VGA_256_COLOR_MODE);
	printf("Video mode OK\n");
    loadPalette("Pal.bmp", &NewPalette);
	printf("Palette loaded\n");
    setPalette_VGA(&NewPalette);
	printf("Palette set\n");
    loadFontNew();
    createErrorTexture();
	printf("Basic textures loaded into memory\n");
}

void otherInit()
{
    initKeyboard();
	printf("Keyboard OK\n");
    initSystem();
	printf("System variables OK\n");
    #if DEBUG == 1
    initDebug();
    printf("Debug OK\n");
    #endif
}

void mainInit()
{
    // sound
    soundInit();
    // timer
    timerInit();
    // gfx
    gfxInit();
    // the rest
    otherInit();
}

void titleInit()
{
    // do nothing atm
}

void gameInit()
{
    Game.object_capacity = 16;
    Game.id_capacity = 16;
    Game.Objects = malloc(Game.object_capacity * sizeof(Object_t));
    Game.ObjectsById = calloc(Game.id_capacity, sizeof(void*));
    Game.Map.level_num = 1;
    levelLoader("LEVELS/tiletest.txt", LOAD_NEW_LEVEL);
	printf("Game variables OK\n");
}

void pauseInit()
{
    States[STATE_INGAME].flags &= ~STATE_ENABLE_UPDATE;
}

void deinitClock()
{
    asm sti;
    midasClose();
    setTimer(TIMER_18HZ);
    _dos_setvect(TIME_KEEPER_INT, old_Timer_ISR);
    asm cli;
}
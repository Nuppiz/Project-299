#include "Common.h"
#include "Patch.h"
#include "Structs.h"
#include "Game.h"
#include "AI.h"
#include "Draw.h"
#include "Input.h"
#include "Keyb.h"
#include "Loadgfx.h"
#include "Movecoll.h"
#include "Sound.h"
#include "Text.h"
#include "Video.h"

System_t System = {0};

#if DEBUG == 1
char debug[NUM_DEBUG][DEBUG_STR_LEN];
#endif

static void interrupt (far *old_Timer_ISR)(void);

unsigned short setTimerBxHookBx;
unsigned char recomputeMidasTickRate = 0;
unsigned int midasTickRate = 1000;
void setTimerBxHook()
{
    // compute the expected tick rate (as 8.8 fix) with given bx
    // assuming "timer" runs at 55 KHz (increased by 55 1000 times a second)
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

void setTimer(uint16_t new_count)
{
    outportb(CONTROL_8253, CONTROL_WORD);
    outportb(COUNTER_0, LOW_BYTE(new_count));
    outportb(COUNTER_0, HIGH_BYTE(new_count));
}

void interrupt far Timer(void)
{
    static long last_clock_time = 0;

    System.time++;

    if (recomputeMidasTickRate) {
        midasTickRate = 55000 / (1193182 / setTimerBxHookBx);
        recomputeMidasTickRate = 0;
    }

    // keeps the PC clock ticking in the background
    if (last_clock_time + midasTickRate < System.time)
    {
        last_clock_time = System.time;
        old_Timer_ISR();
    }
}

int getTimer()
{
    unsigned short divider;
    outportb(CONTROL_8253, 0xD2);
    divider = inportb(COUNTER_0);
    divider |= inportb(COUNTER_0) << 8;

    return divider;
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
    System.tick_time  = 1000/TICK_RATE;
    System.frame_time = 1000/FRAME_RATE;
    System.tick_rate  = TICK_RATE;
    System.frame_rate = FRAME_RATE;
    System.fps        = 0;
    System.fps_avg    = 0;
}

void init()
{
    extern Palette_t NewPalette;
    initSystem();
    patchMidasSetTimer(&setTimerBxHook);
    initSounds();
    //timer
    old_Timer_ISR = _dos_getvect(TIME_KEEPER_INT);
    _dos_setvect(TIME_KEEPER_INT, Timer);
    setTimer(TIMER_1000HZ);

    // gfx
    setVideoMode(VGA_256_COLOR_MODE);
    loadPalette("Pal.bmp", &NewPalette);
    setPalette_VGA(&NewPalette);
    loadFontNew();
    loadAllTextures();
    loadAllTiles();

    // the rest
    initKeyboard();
    initGame();
    #if DEBUG == 1
    initDebug();
    #endif

    playMusic("MUSIC/PELIMUSA.S3M");
    loadSFX("SFX/RIFLE.WAV", "SFX/EXPLOS1.VOC", "SFX/KARJAISU.VOC");
}

void quit()
{
    midasClose();
    setTimer(TIMER_18HZ);
    _dos_setvect(TIME_KEEPER_INT, old_Timer_ISR);
    deinitKeyboard();
    setVideoMode(TEXT_MODE);
}

void updateStats()
{
    sprintf(debug[DEBUG_FPS], "TIME: %ld MINS, %ld SECS\nTICKS: %ld, FRAMES: %ld\nFPS: %d, AVERAGE: %.2f",
        System.seconds/60, System.seconds%60, System.ticks, System.frames, System.fps, System.fps_avg);
}

void gameLoop()
{
    time_t last_time   = 0; // Used for accumulating seconds & FPS calculation
    time_t last_tick   = 0; // Tracks time elapsed since last tick started
    time_t last_frame  = 0; // Tracks time elapsed since last draw started
    time_t accumulator = 0; // Incremented by frame draw duration, decremented by ticks
    int frame_count    = 0; // Counts frames in a second so far; used by debug

    while (System.running == 1)
    {  
        if (last_tick + System.tick_time < System.time) // tick
        {
            do
            {
                sprintf(debug[DEBUG_CLOCK], "TIMER: %d", getTimer());
                last_tick = System.time;

                input();  
                AILoop(); 
                physics();

                accumulator -= System.tick_time;
                System.ticks++;
            }
            while (accumulator >= System.tick_time);
        }

        if (last_frame + System.frame_time < System.time) // frame
        {
            last_frame = System.time;

            draw();

            System.frames++;
            frame_count++;
            accumulator += System.time - last_frame;

            #if DEBUG == 1
            updateStats();
            #endif
        }
           
        #if DEBUG == 1
        if (last_time + 1000 < System.time) // FPS calculation; optional for debugging
        {
            last_time += 1000;
            System.seconds++;
            System.fps_avg = (float)System.frames/System.seconds;
            System.fps = frame_count;
            frame_count = 0;
        }
        #endif
    }
}

void main()
{
    init();
    gameLoop();
    quit();
}
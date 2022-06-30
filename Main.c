#include "Common.h"
#include "Structs.h"
#include "Game.h"
#include "AI.h"
#include "Draw.h"
#include "Input.h"
#include "Keyb.h"
#include "Loadgfx.h"
#include "Movecoll.h"
#include "Patch.h"
#include "Sound.h"
#include "Text.h"
#include "Video.h"

System_t System = {0};

#if DEBUG == 1
char debug[NUM_DEBUG][DEBUG_STR_LEN];
#endif

static void interrupt (far *old_Timer_ISR)(void);
static void interrupt (far *midas_Timer_ISR)(void);

unsigned short setTimerBxHookBx;
unsigned char recomputeMidasTickRate = 0;
unsigned int midasTickRate = 1000;

extern Palette_t NewPalette;

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
    System.tick_time  = 1000/TICK_RATE;
    System.frame_time = 1000/FRAME_RATE;
    System.tick_rate  = TICK_RATE;
    System.frame_rate = FRAME_RATE;
    System.fps        = 0;
    System.fps_avg    = 0;
}

void init()
{
	printf("Initializing sounds...");
    patchMidasSetTimer(&setTimerBxHook);
    asm cli;
    old_Timer_ISR = _dos_getvect(TIME_KEEPER_INT);
    // provide stub ISR to MIDAS so that it doesn't do anything
    _dos_setvect(TIME_KEEPER_INT, &stubISR);
    initSounds();
	printf("OK\n");
    //timer
	printf("Initializing timer...");
    midas_Timer_ISR = _dos_getvect(TIME_KEEPER_INT);
    _dos_setvect(TIME_KEEPER_INT, Timer);
    setTimer(TIMER_1000HZ);
    asm sti;
	printf("OK\n");

    // gfx
	printf("Initializing graphics...");
    setVideoMode(VGA_256_COLOR_MODE);
	printf("Video mode OK\n");
    loadPalette("Pal.bmp", &NewPalette);
	printf("Palette loaded\n");
    setPalette_VGA(&NewPalette);
	printf("Palette set\n");
    loadFontNew();
    loadAllTextures();
    loadAllTiles();
	printf("Graphics loaded into memory\n");

    // the rest
    initKeyboard();
	printf("Keyboard OK\n");
    initSystem();
	printf("System variables OK\n");
    initGame();
	printf("Game variables OK\n");
    #if DEBUG == 1
    initDebug();
    #endif
    
    loadSFX("SFX/RIFLE.WAV", "SFX/EXPLOS1.VOC", "SFX/KARJAISU.VOC");
}

void quit()
{
    asm sti;
    midasClose();
    setTimer(TIMER_18HZ);
    _dos_setvect(TIME_KEEPER_INT, old_Timer_ISR);
    deinitKeyboard();
    asm cli;
    setVideoMode(TEXT_MODE);
}

void updateStats()
{
    sprintf(debug[DEBUG_FPS], "TIME: %ld MINS, %ld SECS\nTICKS: %ld, FRAMES: %ld\nFPS: %d, AVERAGE: %.2f",
        System.seconds/60, System.seconds%60, System.ticks, System.frames, System.fps, System.fps_avg);
    //sprintf(debug[DEBUG_FPS], "BX=%u RC=%u T=%lu", setTimerBxHookBx, recomputeMidasTickRate, System.time);
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
	printf("Init OK\n");
    gameLoop();
    quit();
}
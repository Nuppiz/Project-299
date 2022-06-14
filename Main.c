#include "Common.h"
#include "Structs.h"
#include "AI.h"
#include "Defines.h"
#include "Draw.h"
#include "Enums.h"
#include "Input.h"
#include "Loadgfx.h"
#include "Movecoll.h"
#include "Text.h"
#include "Video.h"
#include "Game.h"

System_t System;
#if DEBUG == 1
extern char debug[8][64];
#endif

static void interrupt (far *old_Timer_ISR)(void);

void interrupt far Timer(void)
{
    static long last_time = 0;

    System.time++;

    // keeps the PC clock ticking in the background
    if (last_time + 182 < System.time)
    {
        last_time = System.time;
        old_Timer_ISR();
    }
}

void setTimer(uint16_t new_count)
{
    outportb(CONTROL_8253, CONTROL_WORD);
    outportb(COUNTER_0, LOW_BYTE(new_count));
    outportb(COUNTER_0, HIGH_BYTE(new_count));
}

void initSystem()
{
    System.running    = 1;
    System.time       = 0;
    System.seconds    = 0;
    System.ticks      = 0;
    System.tick_rate  = TICK_RATE; // not used yet
    System.frames     = 0;
    System.frame_rate = FRAME_RATE; // target FPS
    System.frame_time = 1000/FRAME_RATE;
    System.fps        = 0; // measured FPS
    System.fps_avg    = 0;
}

void initDebug()
{
    int i;

    for (i = 0; i > 8; i++)
    {
        debug[i][0] = '\0';
    }
}

void init()
{
    extern Palette_t NewPalette;

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
    setTextures();
    
    // the rest
    initKeyboard();
    initSystem();
    initGame();
    #if DEBUG == 1
    initDebug();
    #endif
}

void quit()
{   
    setTimer(TIMER_18HZ);
    _dos_setvect(TIME_KEEPER_INT, old_Timer_ISR);
    deinitKeyboard();
    setVideoMode(TEXT_MODE);
}

void gameLoop()
{
    time_t last_time  = 0;
    time_t last_frame = 0;
    int frame_count = 0; // Counts frames in a second so far

    while (System.running == 1)
    {  
        if (last_frame + System.frame_time < System.time) // one game "tick"
        {
            last_frame = System.time;

            input();   // input and physics should be tied to a tick rate, separate from frame rate, so that
            AILoop();  // if a computer renders the scene too slowly, the game simulation will not slow down
            physics(); // this is not done yet, hence System.ticks is commented out
            //System.ticks++;

            draw();
            System.frames++;
            frame_count++;
        }

        #if DEBUG == 1
        if (last_time + 1000 < System.time) // FPS calculation; optional for debugging
        {
            last_time = System.time;

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
#include "Common.h"
#include "Structs.h"
#include "Game.h"
#include "AI.h"
#include "Draw.h"
#include "Init.h"
#include "Input.h"
#include "Keyb.h"
#include "Loadgfx.h"
#include "Movecoll.h"
#include "Patch.h"
#include "Sound.h"
#include "Text.h"
#include "Video.h"
#include "State.h"
#include "Exit.h"

System_t System = {0};
Timer_t Timers = {0};
extern State_t* Stack[NUM_STATES];
extern state_count;
extern stack_top;

void quit()
{
    //gameExit();
    deinitClock();
    deinitKeyboard();
    setVideoMode(TEXT_MODE);
}

void updateStats()
{
    #if DEBUG == 1
    sprintf(debug[DEBUG_FPS], "TIME: %ld MINS, %ld SECS\nTICKS: %ld, FRAMES: %ld\nFPS: %d, AVERAGE: %.2f",
        System.seconds/60, System.seconds%60, System.ticks, System.frames, System.fps, System.fps_avg);
    //sprintf(debug[DEBUG_FPS], "BX=%u RC=%u T=%lu", setTimerBxHookBx, recomputeMidasTickRate, System.time);
    #endif
}

void loop()
{
    int i;

    while (System.running == 1)
    {  
        if (Timers.last_tick + System.tick_interval < System.time) // tick
        {
            do
            {
                Timers.last_tick = System.time;
                Stack[stack_top]->input(); // only handle input from the state at the top of the stack
                clearKeys();
                for (i = 0; i < state_count; i++)
                {
                    if ((Stack[i]->flags & STATE_ENABLE_UPDATE))
                        Stack[i]->update();
                }

                Timers.accumulator -= System.tick_interval;
                if (System.paused == FALSE)
                {
                    System.ticks++;
                    System.ticks_per_frame++;
                }
            }
            while (Timers.accumulator >= System.tick_interval);
        }

        if (Timers.last_frame + System.frame_interval < System.time) // frame
        {
            Timers.last_frame = System.time;

            for (i = 0; i < state_count; i++)
            {
                if ((Stack[i]->flags & STATE_ENABLE_DRAW))
                    Stack[i]->draw();
            }
            render();

            System.frames++;
            Timers.frame_count++;
            Timers.accumulator += System.time - Timers.last_frame;
            System.ticks_per_frame = 0;

            #if DEBUG == 1
            updateStats();
            #endif
        }

           
        #if DEBUG == 1
        if (Timers.last_time + 1000 < System.time) // FPS calculation; optional for debugging
        {
            Timers.last_time += 1000;
            System.seconds++;
            System.fps_avg = (float)System.frames/System.seconds;
            System.fps = Timers.frame_count;
            Timers.frame_count = 0;
        }
        #endif
    }
}

void main()
{
    mainInit();
    pushState(STATE_TITLE);
    loop();
    quit();
}
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

System_t System = {0};
extern State_t* Stack[NUM_STATES];
extern state_count;
extern stack_top;

void quit()
{
    deinitClock();
    deinitKeyboard();
    setVideoMode(TEXT_MODE);
}

void updateStats()
{
    sprintf(debug[DEBUG_FPS], "TIME: %ld MINS, %ld SECS\nTICKS: %ld, FRAMES: %ld\nFPS: %d, AVERAGE: %.2f",
        System.seconds/60, System.seconds%60, System.ticks, System.frames, System.fps, System.fps_avg);
    //sprintf(debug[DEBUG_FPS], "BX=%u RC=%u T=%lu", setTimerBxHookBx, recomputeMidasTickRate, System.time);
}

void loop()
{
    time_t last_time   = 0; // Used for accumulating seconds & FPS calculation
    time_t last_tick   = 0; // Tracks time elapsed since last tick started
    time_t last_frame  = 0; // Tracks time elapsed since last draw started
    time_t accumulator = 0; // Incremented by frame draw duration, decremented by ticks
    int frame_count    = 0; // Counts frames in a second so far; used by debug
    int i;

    while (System.running == 1)
    {  
        if (last_tick + System.tick_interval < System.time) // tick
        {
            do
            {
                last_tick = System.time;
                Stack[stack_top]->input(); // only handle input from the state at the top of the stack
                clearKeys();
                for (i = 0; i < state_count; i++)
                {
                    if ((Stack[i]->flags & STATE_ENABLE_UPDATE))
                        Stack[i]->update();
                }

                accumulator -= System.tick_interval;
                System.ticks++;
            }
            while (accumulator >= System.tick_interval);
        }

        if (last_frame + System.frame_interval < System.time) // frame
        {
            last_frame = System.time;

            for (i = 0; i < state_count; i++)
            {
                if ((Stack[i]->flags & STATE_ENABLE_DRAW))
                    Stack[i]->draw();
            }
            render();

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
    mainInit();
    pushState(STATE_TITLE);
    loop();
    quit();
}
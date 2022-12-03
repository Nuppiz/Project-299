#include "Common.h"
#include "Structs.h"
#include "Filech.h"
#include "LvlLoad.h"
#include "Loadgfx.h"
#include "Sound.h"
#include "Menu.h"
#include "Mouse.h"
#include "Game.h"

extern System_t System;
extern Timer_t Timers;
extern State_t States[];
extern GameData_t Game;
extern Menu_t* current_menu;
extern Menu_t mainmenu;
extern Menu_t ingamemenu;
Weapon_t Weapons[NUM_WEAPONS];

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

    asm pushf;
    asm cli;

    ++System.time;

    if (recomputeMidasTickRate)
    {
        midasTickRate = 1000UL / (1193100UL / setTimerBxHookBx);
        Timers.last_midas_time = System.time;
        recomputeMidasTickRate = 0;
    }

    if (Timers.last_midas_time + midasTickRate < System.time)
    {
        Timers.last_midas_time = System.time;
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
    System.paused     = 0;
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
    createErrorTextures();
    loadBaseTextures();
    loadAnimsFromList("ANIMS/ANIMS.TXT");
    makeSprites();
    makeAnimset();
	printf("Basic textures loaded into memory\n");
}

void initWeapons()
{
    FILE* weapon_file;
    char c;
    int i = 0;

    char name[20] = {'\0'};
    int range, damage, extra_damage, num_projectiles, projectile_speed, projectile_spread, shot_delay, ammo_type, sound_id, effect_id;

    weapon_file = fopen("WEAPONS.DAT", "r");

    if (weapon_file == NULL)
    {
        fclose(weapon_file);
        setVideoMode(TEXT_MODE);
        printf("Unable to open file: WEAPONS.DAT");
        printf("Please check the file actually exists!\n");
        System.running = 0;
        return;
    }

    do
    {
        if (c != '\n')
        {
            fscanf(weapon_file, " %s %d %d %d %d %d %d %d %d %d",
            &name, &range, &damage, &num_projectiles, &projectile_speed, &projectile_spread, &shot_delay, &ammo_type, &sound_id, &effect_id);

            Weapons[i].id = i;
            strcpy(Weapons[i].name, name);
            Weapons[i].range = range;
            Weapons[i].damage = damage;
            Weapons[i].num_projectiles = num_projectiles;
            Weapons[i].projectile_speed = projectile_speed;
            Weapons[i].projectile_spread = projectile_spread;
            Weapons[i].shot_delay = shot_delay;
            Weapons[i].ammo_type = ammo_type;
            Weapons[i].sound_id = sound_id;
            Weapons[i].effect_id = effect_id;
            i++;
        }
    } while ((c = fgetc(weapon_file)) != EOF);

    fclose(weapon_file);
}

void otherInit()
{
    initKeyboard();
	printf("Keyboard OK\n");
    initMouse();
    printf("Mouse OK\n");
    initSystem();
    if (!checkDirectoryExists("SAVES"))
    {
        createDirectory("SAVES");
    }
    initWeapons();
    initActorTemplates();
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

void menuInit()
{
    current_menu = &mainmenu;
}

void gameInit()
{
    if (!checkDirectoryExists("SAVES/AUTO"))
    {
        createDirectory("SAVES/AUTO");
    }
}

void pauseInit()
{
    System.paused = TRUE;
    States[STATE_INGAME].flags &= ~STATE_ENABLE_UPDATE;
}

void ingameMenuInit()
{
    System.paused = TRUE;
    States[STATE_INGAME].flags &= ~STATE_ENABLE_UPDATE;
    current_menu = &ingamemenu;
}

void deinitClock()
{
    asm sti;
    midasClose();
    setTimer(TIMER_18HZ);
    _dos_setvect(TIME_KEEPER_INT, old_Timer_ISR);
    asm cli;
}
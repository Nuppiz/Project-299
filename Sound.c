#include <alloc.h>
#include "Common.h"
#include "Structs.h"
#include "MIDAS/MIDAS.H"

/* MIDAS Sound System and related functions */

/* number of sound effect channels: */
#define FXCHANNELS 2

/* maximum number of channels in music: */
#define MAXMUSCHANNELS 8

/* sound effect playing rate: */
#define FXRATE 11025

char            *usage =
"Usage:\tEFFECTS\t<module> <effect #1> <effect #2> <looping effect #3>";

unsigned        fxChannel = 0;
ModulePlayer    *MP;

    /* pointers to all Module Players: */
ModulePlayer    *modulePlayers[NUMMPLAYERS] =
    { &mpS3M,
      &mpMOD,
      &mpMTM };

    /* module type strings: */
char            *moduleTypeStr[NUMMPLAYERS] = {
    { "Scream Tracker ]I[" },
    { "Protracker" },
    { "Multitracker"} };

mpModule    *mod;                   /* pointer to current module struct */
unsigned    defAmplify;             /* default amplification */
unsigned    amplification;          /* current, amplification */
unsigned    musicVolume = 64;       /* music master volume */
unsigned    SFXVolume = 64;         /* SFX master volume */
int         error;

struct SFX_file SFX_filenames[NUM_SFX];
ushort SFX_array[NUM_SFX];
uint8_t music_on = FALSE;

/****************************************************************************\
*
* Function:     void Error(char *msg)
*
* Description:  Prints an error message, uninitializes MIDAS and exits to DOS
*
* Input:        char *msg               error message
*
\****************************************************************************/

void Error(char *msg)
{
    printf("Error: %s\n", msg);
    midasClose();
    exit(EXIT_FAILURE);
}





/****************************************************************************\
*
* Function:     unsigned LoadEffect(char *fileName, int looping)
*
* Description:  Loads a raw effect sample that can be used with PlayEffect().
*
* Input:        char *fileName          name of sample file
*               int looping             1 if the sample is looping, 0 if not
*
* Returns:      Instrument handle that can be used with PlayEffect() and
*               FreeEffect().
*
\****************************************************************************/

unsigned LoadEffect(char *fileName, int looping)
{
    ushort      instHandle;             /* sound device instrument handle */
    int         error;
    fileHandle  f;
    long        smpLength;              /* sample length */
    uchar       *smpBuf;                /* sample loading buffer */

    /* open sound effect file: */
    if ( (error = fileOpen(fileName, fileOpenRead, &f)) != OK )
        midasError(error);

    /* get file length: */
    if ( (error = fileGetSize(f, &smpLength)) != OK )
        midasError(error);

    /* check that sample length is not too long: */
    if ( smpLength > SMPMAX )
        midasError(errInvalidInst);

    /* allocate memory for sample loading buffer: */
    if ( (error = memAlloc(smpLength, (void**) &smpBuf)) != OK )
        midasError(error);

    /* load sample: */
    if ( (error = fileRead(f, smpBuf, smpLength)) != OK )
        midasError(error);

    /* close sample file: */
    if ( (error = fileClose(f)) != OK )
        midasError(error);

    /* Add sample to Sound Device list and get instrument handle to
       instHandle: */
    if ( looping )
    {
        error = midasSD->AddInstrument(smpBuf, smp8bit, smpLength, 0,
            smpLength, 64, 1, 1, &instHandle);
    }
    else
    {
        error = midasSD->AddInstrument(smpBuf, smp8bit, smpLength, 0,
            0, 64, 0, 1, &instHandle);
    }

    if ( error != OK )
        midasError(error);

    /* deallocate sample allocation buffer: */
    if ( (error = memFree(smpBuf)) != OK )
        midasError(error);

    /* return instrument handle: */
    return instHandle;
}




/****************************************************************************\
*
* Function:     void FreeEffect(unsigned instHandle)
*
* Description:  Deallocates a sound effect
*
* Input:        unsigned instHandle     effect instrument handle returned by
*                                       LoadEffect()
*
\****************************************************************************/

void FreeEffect(unsigned instHandle)
{
    int         error;

    /* remove instrument from Sound Device list: */
    if ( (error = midasSD->RemInstrument(instHandle)) != OK )
        midasError(error);
}



/****************************************************************************\
*
* Function:     void PlayEffect(ushort instHandle, ulong rate, ushort volume,
*                   short panning)
*
* Description:  Plays a sound effect
*
* Input:        ushort instHandle       effect instrument handle, returned by
*                                           LoadEffect().
*               ulong rate              effect sampling rate, in Hz
*               ushort volume           effect playing volume, 0-64
*               short panning           effect panning (see enum sdPanning in
*                                           SDEVICE.H)
*
\****************************************************************************/

void PlayEffect(ushort instHandle, ulong rate, ushort volume,
    short panning)
{
    int         error;

    /* set effect instrument to current effect channel: */
    if ( (error = midasSD->SetInstrument(fxChannel, instHandle)) != OK )
        midasError(error);

    /* set effect volume: */
    if ( (error = midasSD->SetVolume(fxChannel, volume)) != OK )
        midasError(error);

    /* set effect panning: */
    if ( (error = midasSD->SetPanning(fxChannel, panning)) != OK )
        midasError(error);

    /* start playing effect: */
    if ( (error = midasSD->PlaySound(fxChannel, rate)) != OK )
        midasError(error);

    fxChannel++;                        /* channel for next effect */
    if ( fxChannel >= FXCHANNELS )
        fxChannel = 0;
}




/****************************************************************************\
*
* Function:     mpModule *NewModule(char *fileName)
*
* Description:  Detects the type of a module and starts playing it
*
* Input:        char *fileName          module file name
*
\****************************************************************************/

mpModule *NewModule(char *fileName)
{
    uchar       *header;
    fileHandle  f;
    mpModule    *module;
    int         error, mpNum, recognized;

    /* allocate memory for module header: */
    if ( (error = memAlloc(MPHDRSIZE, (void**) &header)) != OK )
        midasError(error);

    /* open module file: */
    if ( (error = fileOpen(fileName, fileOpenRead, &f)) != OK )
        midasError(error);

    /* read MPHDRSIZE bytes of module header: */
    if ( (error = fileRead(f, header, MPHDRSIZE)) != OK )
        midasError(error);

    if ( (error = fileClose(f)) != OK )
        midasError(error);

    /* Search through all Module Players to find one that recognizes the
       file header: */
    mpNum = 0; MP = NULL;
    while ( (mpNum < NUMMPLAYERS) && (MP == NULL) )
    {
        if ( (error = modulePlayers[mpNum]->Identify(header, &recognized))
            != OK )
            midasError(error);
        if ( recognized )
            MP = modulePlayers[mpNum];
        mpNum++;
    }

    /* deallocate module header: */
    if ( (error = memFree(header)) != OK )
        midasError(error);

    if ( MP == NULL )
        Error("Unknown module format");

    /* load the module file using correct Module Player: */
    module = midasLoadModule(fileName, MP, NULL);

    /* check that the module does not have too many channels: */
    if ( module->numChans > MAXMUSCHANNELS )
        Error("Too many channels in module");

    /* start playing the module: */
    midasPlayModule(module, 0);

    return module;
}




/****************************************************************************\
*
* Function:     void StopModule(mpModule *module)
*
* Description:  Stops playing a module and deallocates it
*
* Input:        mpModule *module        pointer to module structure
*
\****************************************************************************/

void StopModule(mpModule *module)
{
    midasStopModule(module);
    midasFreeModule(module);
}

void loadSFX()
{
    int i;

    for (i = 0; i < NUM_SFX; i++)
    {
        SFX_array[i] = LoadEffect(SFX_filenames[i].filename, SFX_filenames[i].looping);
    }   
}

// effect_id comes from the enum table SoundEffects
void playSFX(int effect_id)
{
    PlayEffect(SFX_array[effect_id], FXRATE, SFXVolume, panMiddle);
}

void changeSFXVolume(int modifier)
{
    if (modifier == VOLUME_UP && SFXVolume < 64)
        SFXVolume += 4;
    else if (modifier == VOLUME_DOWN && SFXVolume > 0)
        SFXVolume -= 4;
}

void stopSFX()
{
    // deallocate memory for sound effects
    int i;
    for (i = 0; i < NUM_SFX; i++)
    {
        FreeEffect(SFX_array[i]);
    }
}

void stopMusic()
{
    music_on = FALSE;
    StopModule(mod);
}

void playMusic(char *music)
{
    music_on = TRUE;
    /* Load module and start playing: */
    mod = NewModule(music);
}

void changeMusicVolume(int modifier)
{
    if (modifier == VOLUME_UP)
    {
        if (musicVolume < 64 )
        {
            musicVolume += 4;
            if ( (error = midasMP->SetMasterVolume(musicVolume))
                != OK )
                midasError(error);
        }

        /* Calculate the amplification value that corresponds to the
            current decrease in volume (in respect to the Sound Device
            default amplification value): */
        amplification = defAmplify * 64L*(MAXMUSCHANNELS+FXCHANNELS) /
            (MAXMUSCHANNELS * musicVolume + FXCHANNELS * 64);
        if ( (error = midasSD->SetAmplification(amplification)) != OK)
            midasError(error);

    }

    else if (modifier == VOLUME_DOWN)
    {
        if (musicVolume > 0 )
            {
                musicVolume -= 4;
                if ( (error = midasMP->SetMasterVolume(musicVolume))
                    != OK )
                    midasError(error);
            }

        /* Calculate the amplification value that corresponds to the
            current decrease in volume (in respect to the Sound Device
            default amplification value): */
        amplification = defAmplify * 64L*(MAXMUSCHANNELS+FXCHANNELS) /
            (MAXMUSCHANNELS * musicVolume + FXCHANNELS * 64);
        if ( (error = midasSD->SetAmplification(amplification)) != OK)
            midasError(error);
    }
}

void generateSFXFileTable()
{
    FILE* SFX_list_file;
    int i = 0;
    int looping;
    char filename[20];

    SFX_list_file = fopen("SFX/SFXLIST.txt", "r");

    if (SFX_list_file == NULL)
    {
        fclose(SFX_list_file);
        setVideoMode(TEXT_MODE);
        printf("Unable to open SFX list file!\n");
        printf("Please check the file actually exists!\n");
        quit();
    }

    do
    {
        fscanf(SFX_list_file, "%s %d", filename, &looping);
        SFX_filenames[i].filename = malloc(strlen(filename) + 1);
        strcpy(SFX_filenames[i].filename, filename);
        SFX_filenames[i].looping = looping;
        i++;
    } while (fgetc(SFX_list_file) != EOF);
    
    fclose(SFX_list_file);
}

void initSounds()
{
    int         i, isConfig;
    /* Check that the configuration file exists: */
    if ( (error = fileExists("MIDAS.CFG", &isConfig)) != OK )
        midasError(error);
    if ( !isConfig )
    {
        puts("Configuration file not found - run SOUNDSET.EXE");
        exit(EXIT_FAILURE);
    }

    midasSetDefaults();                 /* set MIDAS defaults */
    midasLoadConfig("MIDAS.CFG");       /* load configuration */

    midasInit();                        /* initialize MIDAS Sound System */

    /* Open channels for music and sound effects. The first FXCHANNELS
       channels will always be free for playing effects: */
    midasOpenChannels(FXCHANNELS + MAXMUSCHANNELS);

    /* Get Sound Device default amplification value: */
    if ( (error = midasSD->GetAmplification(&defAmplify)) != OK )
        midasError(error);
    amplification = defAmplify;

    /* Initialize array for sound effect filenames */
    printf("Generating SFX file name table...\n");
    generateSFXFileTable();
    printf("OK!\n");
    printf("Loading SFX files into memory...\n");
    loadSFX();
    printf("OK!\n");
    free(SFX_filenames);
}
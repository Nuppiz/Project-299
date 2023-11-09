#include "Common.h"
#include "Structs.h"
#include "Exit.h"
#include "General.h"
#include "Draw.h"

/* Graphics loading functions */

extern System_t System;

uint8_t error_pixels[400] =
{
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,5,5,5,0,5,5,0,0,5,5,0,0,0,0,0,0,0,0,0,
0,5,0,0,0,5,0,5,0,5,0,5,0,0,0,0,0,0,0,0,
0,5,5,5,0,5,5,0,0,5,5,0,0,0,0,0,0,0,0,0,
0,5,0,0,0,5,0,5,0,5,0,5,0,0,0,0,0,0,0,0,
0,5,5,5,0,5,0,5,0,5,0,5,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

Texture_array ObjectTextures = {0};
Texture_array TileTextures = {0};
Anim_array Animations = {0};

Sprite_t Rocket = {SPRITE_IS_ANIM, 0, 0, 0, 3};
Sprite_t Explosion = {SPRITE_IS_ANIM, 0, 0, 0, 2};
Sprite_t DudeSprite = {SPRITE_IS_ANIM, 0, 0, 0, 5};

AnimSet_t DudeAnimSet = {0};

void loadGfx(char* filename, uint8_t* destination, uint16_t data_size)
{
    // load raw graphics data (no dimensions and flags)
    FILE* file_ptr;
    file_ptr = fopen(filename, "rb");
    fread(destination, 1, data_size, file_ptr);
    fclose(file_ptr);
}

int findTexture(char* filename, Texture_array* array)
{
    int i;

    for (i = 0; i < array->texture_count; i++)
    {
        if (strcmp(filename, array->textures[i].filename) == 0)
            return i;
    }
    return 0;
}

int findAnim(char* name)
{
    int i;

    for (i = 0; i < Animations.anim_count; i++)
    {
        if (strcmp(name, Animations.anims[i].name) == 0)
            return i;
    }
    return 0;
}

void createErrorTextures()
{
    TileTextures.textures = malloc(sizeof(Texture_t));
    TileTextures.textures[0].filename = "ERROR.7UP";
    TileTextures.textures[0].width = 20;
    TileTextures.textures[0].height = 20;
    TileTextures.textures[0].pixels = error_pixels;
    TileTextures.texture_count++;
    ObjectTextures.textures = malloc(sizeof(Texture_t));
    ObjectTextures.textures[0].filename = "ERROR.7UP";
    ObjectTextures.textures[0].width = 20;
    ObjectTextures.textures[0].height = 20;
    ObjectTextures.textures[0].pixels = error_pixels;
    ObjectTextures.texture_count++;;
}

int loadTexture(char* filename, Texture_array* array)
{
    int texture_id = 0;
    int filename_length;
    FILE* file_ptr;

    //loop all textures here to check if it was already loaded,
    //return id of that texture if it was found.

    if ((texture_id = findTexture(filename, array)) != 0)
        return texture_id;

    file_ptr = fopen(filename, "rb");
    if (file_ptr == NULL)
    {
        printf("Unable to open file %s!\n", filename);
        return 0;
    }

    array->textures = realloc(array->textures, (array->texture_count + 1) * sizeof(Texture_t));

    ASSERT(array->textures != NULL);

    texture_id = array->texture_count;
    array->texture_count++;

    filename_length = strlen(filename) + 1;
    array->textures[texture_id].filename = malloc(filename_length);
    strcpy(array->textures[texture_id].filename, filename);

    fread(&array->textures[texture_id].width, 2, 1, file_ptr);
    fseek(file_ptr, 2, SEEK_SET);
    fread(&array->textures[texture_id].height, 2, 1, file_ptr);
    fseek(file_ptr, 6, SEEK_SET);
    fread(&array->textures[texture_id].transparent, 2, 1, file_ptr);
	fseek(file_ptr, 8, SEEK_SET);
    array->textures[texture_id].pixels = malloc(array->textures[texture_id].width * array->textures[texture_id].height);
    fread(array->textures[texture_id].pixels, 1, array->textures[texture_id].width * array->textures[texture_id].height, file_ptr);
    array->textures[texture_id].offset_x = 0;
    array->textures[texture_id].offset_y = 0;

    fclose(file_ptr);
    
    return texture_id;
}

void loadTexturesFromList(char* list_filename, Texture_array* array)
{
    FILE* tex_list_file;
    char filename[20];

    tex_list_file = fopen(list_filename, "r");

    if (tex_list_file == NULL)
    {
        char tex_list_error_string[200];
        sprintf(tex_list_error_string, "Unable to open texture list file: %s.\n"
                    "Please check the file actually exists!\n", list_filename);
        fclose(tex_list_file);
        quitError(tex_list_error_string);
    }

    do
    {
        fscanf(tex_list_file, "%s", filename);
        loadTexture(filename, array);
    } while (fgetc(tex_list_file) != EOF);
    
    fclose(tex_list_file);
}

void loadBaseTextures()
{
    loadTexturesFromList("SPRITES/BASETEX.txt", &ObjectTextures);
    loadTexturesFromList("SPRITES/ACTORTEX.txt", &ObjectTextures);
}

int loadAnimation(char* filename)
{
    FILE* anim_file;
    int num_frames = 0;
    int animation_frame = 0;
    int anim_frame_id;
    int anim_id = 0;
    int rotation_index;
    long rotated_frame_size;
    double rotation_angle;
    char c;
    char texture_filename[20];

    Animations.anims = realloc(Animations.anims, (Animations.anim_count + 1) * sizeof(Anim_t));

    anim_file = fopen(filename, "r");

    if ((anim_id = findAnim(filename)) != 0)
        return anim_id;

    if (anim_file == NULL)
    {
        char anim_error_string[200];
        sprintf(anim_error_string, "Unable to open animation file: %s.\n"
                    "Please check the file actually exists!\n", filename);
        fclose(anim_file);
        quitError(anim_error_string);
    }

    anim_id = Animations.anim_count;
    Animations.anim_count++;

    Animations.anims[anim_id].name = malloc(strlen(filename) + 1);
    strcpy(Animations.anims[anim_id].name, filename);

    while ((c = fgetc(anim_file)) != EOF)
    {
        if (c == '\n')
            num_frames++;
    }

    Animations.anims[anim_id].num_frames = num_frames;
    Animations.anims[anim_id].frames = malloc(num_frames * sizeof(AnimFrame_t));

    fseek(anim_file, 0, SEEK_SET);

    for (animation_frame = 0; animation_frame < num_frames; animation_frame++)
    {
        fscanf(anim_file, "%20s", texture_filename);
        anim_frame_id = loadTexture(texture_filename, &ObjectTextures);
        Animations.anims[anim_id].frames[animation_frame].frame_id = anim_frame_id;
        if (anim_id == 1 || anim_id > 3)
        {
            Animations.anims[anim_id].frames[animation_frame].rotations = malloc(NUM_ROTATIONS * sizeof(RotatedTexture_t));
            for (rotation_index = 0, rotation_angle = 0.0; rotation_index < NUM_ROTATIONS; rotation_index++, rotation_angle += RAD_30)
            {
                rotated_frame_size = calculateRotatedTextureSize(rotation_angle, &ObjectTextures.textures[Animations.anims[anim_id].frames[animation_frame].frame_id]);
                Animations.anims[anim_id].frames[animation_frame].rotations[rotation_index].pixels = farmalloc(rotated_frame_size);
                Animations.anims[anim_id].frames[animation_frame].rotations[rotation_index] = 
                saveRotatedTexture(rotation_angle, &ObjectTextures.textures[Animations.anims[anim_id].frames[animation_frame].frame_id], TRANSPARENT_COLOR);
            }
        }
    }

    fclose(anim_file);

    return anim_id;
}

void loadAnimsFromList(char* list_filename)
{
    FILE* anim_list_file;
    char filename[20];

    anim_list_file = fopen(list_filename, "r");

    if (anim_list_file == NULL)
    {
        char animlist_error_string[200];
        sprintf(animlist_error_string, "Unable to open animation list file: %s.\n"
                    "Please check the file actually exists!\n", list_filename);
        fclose(anim_list_file);
        quitError(animlist_error_string);
    }

    do
    {
        fscanf(anim_list_file, "%20s", filename);
        loadAnimation(filename);
    } while (fgetc(anim_list_file) != EOF);
    
    fclose(anim_list_file);
}

// temp
void makeSprites()
{
    Rocket.anim_id = 1;
    Explosion.anim_id  = 3;
}

void makeAnimset()
{
    /*DudeAnimSet.anim_ids[ANIM_IDLE] = loadAnimation("ANIMS/DUDEIDLE.ANI");
    DudeAnimSet.anim_ids[ANIM_WALK] = loadAnimation("ANIMS/DUDEWALK.ANI");
    DudeAnimSet.anim_ids[ANIM_PUNCH_STILL] = loadAnimation("ANIMS/DUDESTP.ANI");
    DudeAnimSet.anim_ids[ANIM_PUNCH_WALK] = loadAnimation("ANIMS/DUDEWALP.ANI");
    DudeAnimSet.anim_ids[ANIM_SHOOT] = loadAnimation("ANIMS/DUDEGUNS.ANI");*/
    DudeSprite.anim_id = 4;
}

void freeAllTextures()
{
    int i;

    for (i = 1; i < TileTextures.texture_count; i++)
    {
        free(TileTextures.textures[i].filename);
        free(TileTextures.textures[i].pixels);
    }
    TileTextures.textures = realloc(TileTextures.textures, sizeof(Texture_t));
    TileTextures.texture_count = 1;

    /*for (i = 1; i < ActorTextures.texture_count; i++)
    {
        free(ActorTextures.textures[i].filename);
        free(ActorTextures.textures[i].pixels);
    }
    ActorTextures.textures = realloc(ActorTextures.textures, sizeof(Texture_t));
    ActorTextures.texture_count = 1;*/
}
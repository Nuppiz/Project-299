#include "Common.h"
#include "Game.h"
#include "Structs.h"
#include "Loadgfx.h"
#include "Filech.h"
#include "Text.h"
#include "Draw.h"
#include "Movecoll.h"

/* Level data and entity loader */

Entity_t Entities[MAX_ENTITIES];
extern System_t System;
extern GameData_t Game;
extern Texture_array TileTextures;
Tile_t TileSet[TILESET_MAX];
Interactive_t* Interactives;

const char* entity_type_strings[NUM_ENTITYTYPES] =
{
    "Door",
    "Button",
    "Spawner",
    "Trigger",
    "Counter",
    "Portal",
};

const char* interactive_type_strings[NUM_INTERACTIVE_TILES] =
{
    "None",
    "Key_Red",
    "Key_Blue",
    "Key_Yellow",
    "Spikes",
};

void loadTileset(char* filename)
{
    FILE* tileset;
    char c, symbol, tex_id;
    char name[50];

    tileset = fopen(filename, "rb");

    if (tileset == NULL)
        tileset = fopen(DEFAULT_TILESET, "rb");

    while ((c = fgetc(tileset)) != EOF)
    {
        if (c == '$')
        {
            symbol = fgetc(tileset) - 32;
            fscanf(tileset, "%s", name);
            tex_id = loadTexture(name, &TileTextures);
            TileSet[symbol].texture_id = tex_id;

            while((c = fgetc(tileset)) != '\n' && c != EOF)
            {
                switch (c)
                {
                case 'D': TileTextures.textures[tex_id].material_type = MAT_DEFAULT; continue;
                case 'G': TileTextures.textures[tex_id].material_type = MAT_GRASS; continue;
                case 'O': TileSet[symbol].obstacle = 1; continue;
                case 'B': TileSet[symbol].block_bullets = 1; continue;
                
                default:  continue;
                }
            }
        }
    }
    fclose(tileset);
}

int entityTypeCheck(char* entity_name)
{
    int entity_type_index;

    for (entity_type_index = 0; entity_type_index < NUM_ENTITYTYPES; entity_type_index++)
    {
        if (strcmp(entity_name, entity_type_strings[entity_type_index]) == 0)
            return entity_type_index;
    }
    return RETURN_ERROR;
}

int interactiveTypeCheck(char* interactive_name)
{
    int interactive_type_index;

    for (interactive_type_index = 0; interactive_type_index < NUM_INTERACTIVE_TILES; interactive_type_index++)
    {
        if (strcmp(interactive_name, interactive_type_strings[interactive_type_index]) == 0)
            return interactive_type_index;
    }
    return RETURN_ERROR;
}

void entityLoader(FILE* level_file, int entity_id, int entity_type)
{
    // because it would look messy to put all of this into the main loader function

    int ent_x, ent_y, state, tilemap_location, only_once; double angle; // common variables
    int locked, key; // door variables
    int target; // switch/button variables
    time_t last_spawn_time; int spawn_interval, toggleable, max_objects, num_objects, spawn_type, trigger_on_death; // spawner variables
    time_t last_trigger_time; int trigger_interval; int target_ids[4]; // trigger variables
    int max_value, target_id; // counter variables
    char level_name[20]; int portal_x, portal_y; // portal variables

    Entity_t* ent; // shorter name of the current entity for more compact code

    if (entity_type != ENT_COUNTER)
        fscanf(level_file, "%d %d %d", &ent_x, &ent_y, &state);

    tilemap_location = ent_y * Game.Map.width + ent_x;

    Game.Map.tilemap[tilemap_location].is_entity = 1;
    Game.Map.tilemap[tilemap_location].entity_value = entity_id;

    ent = &Entities[entity_id];

    ent->type = entity_type;
    if (entity_type != ENT_COUNTER)
    {
        ent->x = ent_x;
        ent->y = ent_y;
        ent->id = entity_id;
        ent->state = state;
    }

    switch(entity_type)
    {
    case ENT_DOOR: fscanf(level_file, "%d %d", &locked, &key),
                ent->data.door.locked = locked,
                ent->data.door.key = key;
                break;
    case ENT_BUTTON: fscanf(level_file, "%d", &target),
                ent->data.button.target = target;
                break;
    case ENT_SPAWNER: fscanf(level_file, "%d %lf %d %d %d %d %d", 
                &spawn_type, &angle, &trigger_on_death, &max_objects, &spawn_interval, &toggleable, &only_once),
                ent->data.spawner.last_spawn_time = 0,
                ent->data.spawner.num_objects = 0,
                ent->data.spawner.spawn_type = spawn_type,
                ent->data.spawner.angle = angle,
                ent->data.spawner.trigger_on_death = trigger_on_death,
                ent->data.spawner.max_objects = max_objects,
                ent->data.spawner.spawn_time_interval = spawn_interval / System.tick_interval,
                ent->data.spawner.toggleable = toggleable,
                ent->data.spawner.only_once = only_once;
                break;
    case ENT_TRIGGER: fscanf(level_file, "%d %d %d %d %d %d", &trigger_interval,
                &target_ids[0], &target_ids[1], &target_ids[2], &target_ids[3], &only_once),
                ent->data.trigger.last_trigger_time = 0,
                ent->data.trigger.trigger_interval = trigger_interval / System.tick_interval,
                ent->data.trigger.target_ids[0] = target_ids[0],
                ent->data.trigger.target_ids[1] = target_ids[1],
                ent->data.trigger.target_ids[2] = target_ids[2],
                ent->data.trigger.target_ids[3] = target_ids[3],
                ent->data.trigger.only_once = only_once;
                break;
    case ENT_COUNTER: fscanf(level_file, "%d %d %d", &max_value, &target_id, &only_once),
                ent->data.counter.value = 0,
                ent->data.counter.max_value = max_value,
                ent->data.counter.target_id = target_id,
                ent->data.counter.only_once = only_once;
                break;
    case ENT_PORTAL: fscanf(level_file, "%s %d %d %lf", level_name, &portal_x, &portal_y, &angle),
                strcpy(ent->data.portal.level_name, level_name),
                ent->data.portal.x = portal_x,
                ent->data.portal.y = portal_y,
                ent->data.portal.angle = angle;
                break;
    }
}

void freeAllEntities()
{
    int i, tilemap_loc;
    for (i = 0; i < MAX_ENTITIES; i++)
    {
        tilemap_loc = Entities[i].y * Game.Map.width + Entities[i].x;
        Game.Map.tilemap[tilemap_loc].is_entity = 0;
        Game.Map.tilemap[tilemap_loc].entity_value = 0;
        memset(&Entities[i], 0, sizeof(Entity_t));
    }
}

void levelLoader(char* level_name, uint8_t load_type)
{
    // general variables
    FILE* level_file;
    char buffer[100];
    char c;
    int i, levelname_length;

    char level_path[30] = LEVEL_PATH; // default level path
    char temp_level[30]; // temporary storage for the level name, as otherwise the filename pointers go bonkers

    // tileset variables
    int tileset_found = FALSE;
    char tileset_file[20] = DEFAULT_TILESET;
    char texture_filename[20];

    // actor variables
    int x, y;
    double angle;
    int radius, control, ai_mode, ai_timer, health, trigger_on_death;
    id_t ai_target, texture_id;

    // entity variables
    char entity_name[20];
    int entity_id, entity_type;

    // interactive tile variables
    char interactive_name[20];
    int interactive_type, tilemap_location, state;

    strcat(level_path, level_name);

    level_file = fopen(level_path, "r");

    strcpy(temp_level, level_name);
    
    if (level_file == NULL)
    {
        fclose(level_file);
        setVideoMode(TEXT_MODE);
        printf("Unable to open file: %s", level_name);
        printf("Please check the file actually exists!\n");
        quit();
    }

    if (Entities != NULL || TileTextures.textures != NULL || Game.Objects != NULL)
    {
        freeAllEntities();
        freeAllTextures();
        freeGameData();
        emptyCorpseArray();
        if (Interactives != NULL)
        {
            free(Interactives);
            Game.interactive_capacity = 0;
        }
    }

    strcpy(Game.current_level_name, temp_level);

    if (load_type == LOAD_NEW_LEVEL)
    {
        initGameData(OBJ_DEFAULT_CAPACITY, OBJ_DEFAULT_CAPACITY);
        Game.interactive_capacity = 16;
        Interactives = malloc(Game.interactive_capacity * sizeof(Interactive_t));
        memset(Interactives, 0, Game.interactive_capacity * sizeof(Interactive_t));
    }

    while ((c = fgetc(level_file)) != EOF)
    {
        if (c == '$')
        {
            fscanf(level_file, "%s", buffer);
            if (strcmp(buffer, "tileset") == 0)
            {
                fscanf(level_file, "%s", tileset_file);
                tileset_found = TRUE;
                loadTileset(tileset_file);
            }
            else if (strcmp(buffer, "leveldim") == 0)
            {
                fscanf(level_file, "%d %d", &Game.Map.width, &Game.Map.height);
                Game.Map.tilemap = malloc((Game.Map.width * Game.Map.height) * sizeof(Tile_t));
            }
            else if (strcmp(buffer, "tilemap") == 0)
            {
                i = 0;
                while ((c = fgetc(level_file)) != EOF && i < Game.Map.width * Game.Map.height)
                {
                    if (c != '\n')
                    {
                        Game.Map.tilemap[i] = TileSet[c - 32];
                        i++;
                    }
                }
                memset(TileSet, 0, TILESET_MAX * sizeof(Tile_t));
            }
            else if (strcmp(buffer, "player") == 0 && load_type != LOAD_SAVED_LEVEL)
            {
                fscanf(level_file, "%d %d %lf %d %d %s",
                    &x, &y, &angle, &radius, &control, texture_filename);
                Game.player_id = createObject((float)x, (float)y, angle, radius, control, 0, 0, 0, 999, -1, 20, texture_filename);
            }
            else if (strcmp(buffer, "dude") == 0 && load_type != LOAD_SAVED_LEVEL)
            {
                fscanf(level_file, "%d %d %lf %d %d %d %d %u %d %d %s",
                    &x, &y, &angle, &radius, &control, &ai_mode, &ai_timer, &ai_target, &health, &trigger_on_death, texture_filename);
                createObject((float)x, (float)y, angle, radius, control, ai_mode, ai_timer, ai_target, health, trigger_on_death, 20, texture_filename);
            }
            else if (strcmp(buffer, "entity") == 0 && load_type != LOAD_SAVED_LEVEL)
            {
                fscanf(level_file, "%d %s", &entity_id, entity_name);
                entity_type = entityTypeCheck(entity_name);
                if (entity_type == RETURN_ERROR)
                {
                    // replace later with just exit to main menu
                    fclose(level_file);
                    setVideoMode(TEXT_MODE);
                    printf("Level load error: invalid entity type.\n");
                    printf("Please check the level file!\n");
                    quit();
                }
                entityLoader(level_file, entity_id, entity_type);
            }
            else if (strcmp(buffer, "interactive") == 0 && load_type != LOAD_SAVED_LEVEL)
            {
                fscanf(level_file, "%s %d %d", interactive_name, &tilemap_location, &state);
                interactive_type = interactiveTypeCheck(interactive_name);
                if (interactive_type == RETURN_ERROR)
                {
                    // replace later with just exit to main menu
                    fclose(level_file);
                    setVideoMode(TEXT_MODE);
                    printf("Level load error: invalid interactive tile type.\n");
                    printf("Please check the level file!\n");
                    quit();
                }
                Interactives[Game.interactive_count].index = tilemap_location;
                Interactives[Game.interactive_count].state = state;
                Interactives[Game.interactive_count].type = interactive_type;
                Game.interactive_count++;
                if (Game.interactive_count >= Game.interactive_capacity)
                {
                    Game.interactive_capacity += 8;
                    Interactives = realloc(Interactives, Game.interactive_capacity* sizeof(Interactive_t));
                }
            }
        }
    }
    if (tileset_found == FALSE)
    {
        loadTileset(tileset_file);
    }
    fclose(level_file);
}

void setEntityTilemap()
{
    int i, tilemap_location;

    for (i = 0; i < MAX_ENTITIES; i++)
    {
        if (Entities[i].type != ENT_COUNTER)
        {
            tilemap_location = Entities[i].y * Game.Map.width + Entities[i].x;

            Game.Map.tilemap[tilemap_location].is_entity = 1;
            Game.Map.tilemap[tilemap_location].entity_value = Entities[i].id;
            if (Entities[i].type == ENT_DOOR || Entities[i].type == ENT_BUTTON)
            {
                if (Entities[i].state == 1)
                {
                    Game.Map.tilemap[tilemap_location].texture_id++;
                    if (Entities[i].type == ENT_DOOR)
                    {
                        Game.Map.tilemap[tilemap_location].obstacle = 0;
                        Game.Map.tilemap[tilemap_location].block_bullets = 0;
                    }
                }
            }
        }
    }
}

void saveGameState()
{
    FILE* save_file;
    char savefilename[30] = "SAVES/CURRENT/CURSTATE.SAV";
    save_file = fopen(savefilename, "wb");
    if (!save_file)
    {
        perror("fopen");
        delay(60000);
    }
    fwrite(&PlayerObject.health, 2, 1, save_file);
    fwrite(&PlayerObject.position.x, 8, 1, save_file);
    fwrite(&PlayerObject.position.y, 8, 1, save_file);
    fclose(save_file);
}

void saveLevelState(char* levelname)
{
    FILE* save_file;
    char savefilepath[50] = "SAVES/CURRENT/";

    strcat(levelname, ".SAV");
    strcat(savefilepath, levelname);
    save_file = fopen(savefilepath, "wb");
    if (!save_file)
    {
        printf("%s", levelname);
        perror("fopen");
        delay(60000);
    }
    fwrite(&Game, sizeof(GameData_t), 1, save_file);
    fwrite(Game.Objects, sizeof(Object_t), Game.object_capacity, save_file);
    fwrite(Game.ObjectsById, sizeof(id_t), Game.id_capacity, save_file);
    fwrite(Entities, sizeof(Entity_t), MAX_ENTITIES, save_file);
    fwrite(Interactives, sizeof(Interactive_t), Game.interactive_capacity, save_file);
    fclose(save_file);
}

void loadGameState()
{
    FILE* state_file;
    int player_hp;
    Vec2 player_loc;

    if (checkFileExists("SAVES/CURRENT/CURSTATE.SAV"))
    {
        state_file = fopen("SAVES/CURRENT/CURSTATE.SAV", "rb");
        fread(&player_hp, 2, 1, state_file);
        if (player_hp > 0) // avoid infinite death loop
            PlayerObject.health = player_hp;
        fread(&player_loc.x, 8, 1, state_file);
        fread(&player_loc.y, 8, 1, state_file);
        PlayerObject.position.x = player_loc.x;
        PlayerObject.position.y = player_loc.y;
        updateGridLoc(&PlayerObject);
        // just in case the player's location in the save file IS on a portal (shouldn't be if level is made correctly)
        if (checkForPortal(PlayerObject.grid_loc) == TRUE)
        {
            PlayerObject.position = moveFromPortal(PlayerObject.position);
            updateGridLoc(&PlayerObject);
        }
        fclose(state_file);
    }
}

void loadLevelState(char* savename)
{
    FILE* save_file;
    char savefilepath[50] = "SAVES/CURRENT/";
    int i;

    strcat(savefilepath, savename);
    save_file = fopen(savefilepath, "rb");
    if (!save_file)
    {
        printf("%s", savename);
        perror("fopen");
        delay(60000);
    }
    fseek(save_file, 0x30, SEEK_SET);
    fread(&Game.object_count, 2, 1, save_file);
    fseek(save_file, 0x32, SEEK_SET);
    fread(&Game.object_capacity, 2, 1, save_file);
    fseek(save_file, 0x34, SEEK_SET);
    fread(&Game.id_capacity, 2, 1, save_file);
    initGameData(Game.object_capacity, Game.id_capacity);
    fseek(save_file, 0x36, SEEK_SET);
    fread(&Game.interactive_count, 1, 1, save_file);
    fread(&Game.interactive_capacity, 1, 1, save_file);
    Interactives = malloc(Game.interactive_capacity * sizeof(Interactive_t));
    fseek(save_file, 0x38, SEEK_SET);
    fread(&Game.player_id, 2, 1, save_file);
    fseek(save_file, 0x3A, SEEK_SET);
    fread(Game.Objects, sizeof(Object_t), Game.object_capacity, save_file);
    fread(Game.ObjectsById, sizeof(id_t), Game.id_capacity, save_file);
    fread(Entities, sizeof(Entity_t), MAX_ENTITIES, save_file);
    fread(Interactives, sizeof(Interactive_t), Game.interactive_capacity, save_file);
    fclose(save_file);
    setEntityTilemap();
}

void levelTransition(char* prevlevelname, char* newlevelname)
{
    char prevsavename[30] = {'\0'};
    char newsavename[30] = {'\0'};
    char loadsavename[30] = {'\0'};
    char savepath[45] = "SAVES/CURRENT/";

    // create save file names
    strncpy(prevsavename, prevlevelname, (strlen(prevlevelname) - 4)); // drop the level filename ending
    strncpy(newsavename, newlevelname, (strlen(newlevelname) - 4)); // drop the level filename ending
    strcpy(loadsavename, newsavename); // copy the name of the new level
    strcat(loadsavename, ".SAV"); // add save filename ending
    strcat(savepath, loadsavename); // construct folder path

    // save the level we're exiting and stats
    saveLevelState(prevsavename);
    saveGameState();

    if (checkFileExists(savepath)) // if savefile exists, load save
    {
        levelLoader(newlevelname, LOAD_SAVED_LEVEL);
        loadLevelState(loadsavename);
    }
    else // else load everything from level file
        levelLoader(newlevelname, LOAD_NEW_LEVEL);

    // save current status of the level we're entering
    saveLevelState(newsavename);

    // load stats
    loadGameState();
}

void loadAfterDeath(char* currentlevel)
{
    char loadsavename[30] = {'\0'};
    char savepath[45] = "SAVES/CURRENT/";

    strncpy(loadsavename, currentlevel, (strlen(currentlevel) - 4)); // drop the level filename ending
    strcat(loadsavename, ".SAV"); // add save filename ending
    strcat(savepath, loadsavename); // construct folder path

    if (checkFileExists(savepath)) // if savefile exists, load save
    {
        levelLoader(currentlevel, LOAD_SAVED_LEVEL);
        loadLevelState(loadsavename);
        loadGameState();
    }
    else // else load everything from level file
        levelLoader(currentlevel, LOAD_NEW_LEVEL);
}

void quickSave(char* levelname)
{
    char savename[30] = {'\0'};
    strncpy(savename, levelname, (strlen(levelname) - 4)); // drop the level filename ending
    saveLevelState(savename);
    saveGameState();
}

void quickLoad(char* levelname)
{
    char loadname[30] = {'\0'};
    char savepath[45] = "SAVES/CURRENT/";
    strncpy(loadname, levelname, (strlen(levelname) - 4)); // drop the level filename ending
    strcat(loadname, ".SAV"); // add save filename ending
    strcat(savepath, loadname); // construct folder path

    if (checkFileExists(savepath)) // if savefile exists, load save, else do nothing
    {
        levelLoader(levelname, LOAD_SAVED_LEVEL);
        loadLevelState(loadname);
        loadGameState();
    }
}
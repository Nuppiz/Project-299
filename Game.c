#include "Common.h"
#include "LvlLoad.h"
#include "Structs.h"
#include "Loadgfx.h"
#include "Vectors.h"

#define ACTOR_CHUNK_SIZE 16

/* Game data and actor array functions */

GameData_t Game = {0};
extern Texture_array ObjectTextures;
extern System_t System;
extern Weapon_t Weapons[];
extern Sprite_t DudeSprite;
extern AnimSet_t DudeAnimSet;
ActorTemplate_t ActorTemplates[NUM_ACTORTEMPLATES];
int actortemplate_count = 1;

id_t getNewId()
{
    id_t id;
    // to do; optimize with a cached "free list" to avoid looping through really long id lists
    // fixed bug where only every other number was used
    // start id count from 1 so 0 is never used; assign 0 as "no id" if needed
    for (id = 1; id < Game.id_capacity; id++)
        if (Game.ActorsById[id] == UINT16_MAX)
            return id;
    // no free IDs found; allocate more
    Game.id_capacity += ACTOR_CHUNK_SIZE;
    Game.ActorsById = realloc(Game.ActorsById, Game.id_capacity * sizeof(id_t));
    memset(&Game.ActorsById[Game.id_capacity - ACTOR_CHUNK_SIZE], UINT8_MAX, ACTOR_CHUNK_SIZE * sizeof(id_t)); // set new ids to -1
    // to do later: ensure successful allocation
    return id;
}

void initActorTemplates()
{
    ActorTemplates[ACT_ERROR].name = malloc(10);
    ActorTemplates[ACT_ERROR].name = "ERROR.ACT";
    ActorTemplates[ACT_ERROR].walk_speed = 1.0; 
    ActorTemplates[ACT_ERROR].run_speed = 2.0;
    ActorTemplates[ACT_ERROR].turn_rate = 0.05 ;
    ActorTemplates[ACT_ERROR].radius = 5;
    ActorTemplates[ACT_ERROR].health = 50;
    ActorTemplates[ACT_ERROR].primary_weapon_id = 0; 
    ActorTemplates[ACT_ERROR].secondary_weapon_id = 0;
}

int loadActorTemplate(char* filename)
{
    // file/traversing variables
    FILE* actortemplate_file;
    char c;
    char buffer[100];
    int actortemplate_id = actortemplate_count;
    // stat variables
    float walk_speed, run_speed, turn_rate;
    int radius, health;
    id_t primary_weapon_id, secondary_weapon_id;
    // animation variables
    char anim_name[20];
    char anim_filename[30];
    int animset_enum;

    actortemplate_file = fopen(filename, "r");

    if (actortemplate_file == NULL)
    {
        fclose(actortemplate_file);
        setVideoMode(TEXT_MODE);
        printf("Unable to open actor template file!\n");
        printf("Please check the file actually exists!\n");
        System.running = 0;
        return 0;
    }

    ActorTemplates[actortemplate_id].name = malloc(strlen(filename) + 1);
    strcpy(ActorTemplates[actortemplate_id].name, filename);

    do
    {
        if (c == '$')
        {
            fscanf(actortemplate_file, "%s", buffer);
            if (strcmp(buffer, "walk_speed") == 0)
            {
                fscanf(actortemplate_file, "%f", &walk_speed);
            }
            else if (strcmp(buffer, "run_speed") == 0)
            {
                fscanf(actortemplate_file, "%f", &run_speed);
            }
            else if (strcmp(buffer, "turn_rate") == 0)
            {
                fscanf(actortemplate_file, "%f", &turn_rate);
            }
            else if (strcmp(buffer, "radius") == 0)
            {
                fscanf(actortemplate_file, "%d", &radius);
            }
            else if (strcmp(buffer, "health") == 0)
            {
                fscanf(actortemplate_file, "%d", &health);
            }
            else if (strcmp(buffer, "primary_weapon_id") == 0)
            {
                fscanf(actortemplate_file, "%d", &primary_weapon_id);
            }
            else if (strcmp(buffer, "secondary_weapon_id") == 0)
            {
                fscanf(actortemplate_file, "%d", &secondary_weapon_id);
            }
            else if (strcmp(buffer, "anim") == 0)
            {
                fscanf(actortemplate_file, "%s", anim_name);
                if ((animset_enum = actorAnimTypeCheck(anim_name)) != RETURN_ERROR)
                {
                    fscanf(actortemplate_file, "%s", anim_filename);
                    ActorTemplates[actortemplate_id].animset.anim_ids[animset_enum] = loadAnimation(anim_filename);
                }
                else
                {
                    fclose(actortemplate_file);
                    setVideoMode(TEXT_MODE);
                    printf("Error loading animset!\n");
                    System.running = 0;
                    return 0;
                }
            }
        }
    } while ((c = fgetc(actortemplate_file)) != EOF);

    fclose(actortemplate_file);

    ActorTemplates[actortemplate_id].walk_speed = walk_speed;
    ActorTemplates[actortemplate_id].run_speed = run_speed;
    ActorTemplates[actortemplate_id].turn_rate = turn_rate;
    ActorTemplates[actortemplate_id].radius = radius;
    ActorTemplates[actortemplate_id].health = health;
    ActorTemplates[actortemplate_id].primary_weapon_id = primary_weapon_id;
    ActorTemplates[actortemplate_id].secondary_weapon_id = secondary_weapon_id;

    actortemplate_count++;
    return actortemplate_id;
}

id_t createActorFromTemplate(char* template_name, float x, float y, double angle, uint8_t control, uint8_t ai_mode, int ai_timer, id_t ai_target, id_t trigger_on_death)
{
	Vec2 direction; // temporary container for direction value
    id_t id = getNewId();
    int actortemplate_id = loadActorTemplate(template_name);
	direction = getDirVec2(angle);

    if (Game.actor_count >= Game.actor_capacity)
    {
        Game.actor_capacity += ACTOR_CHUNK_SIZE;
        Game.Actors = realloc(Game.Actors, Game.actor_capacity * sizeof(Actor_t));
        // to do later: ensure successful allocation
    }

    Game.ActorsById[id] = Game.actor_count;

    memset(&Game.Actors[Game.actor_count], 0, sizeof(Actor_t));

    Game.Actors[Game.actor_count].id = id;
    Game.Actors[Game.actor_count].position.x = x;
    Game.Actors[Game.actor_count].position.y = y;
    Game.Actors[Game.actor_count].angle = angle;
    Game.Actors[Game.actor_count].radius = ActorTemplates[actortemplate_id].radius;
    Game.Actors[Game.actor_count].control = control;
    Game.Actors[Game.actor_count].ai_mode = ai_mode;
    Game.Actors[Game.actor_count].ai_timer = ai_timer;
    Game.Actors[Game.actor_count].target_id_primary = ai_target;
    Game.Actors[Game.actor_count].target_id_secondary = UINT16_MAX;
    Game.Actors[Game.actor_count].health = ActorTemplates[actortemplate_id].health;
    Game.Actors[Game.actor_count].trigger_on_death = trigger_on_death;
    Game.Actors[Game.actor_count].primary_weapon = &Weapons[ActorTemplates[actortemplate_id].primary_weapon_id];
	Game.Actors[Game.actor_count].direction.x = direction.x;
	Game.Actors[Game.actor_count].direction.y = direction.y;
    Game.Actors[Game.actor_count].last_shot = System.ticks;
    Game.Actors[Game.actor_count].animset = &ActorTemplates[actortemplate_id].animset;
    ASSERT(Game.Actors[Game.actor_count].animset != NULL);

    Game.actor_count++;
    
    return id;
}

id_t createActor(float x, float y, double angle, int radius, uint8_t control, uint8_t ai_mode, int ai_timer, id_t ai_target, int health, id_t trigger_on_death, id_t primary_weapon, char* texture_name)
{
	Vec2 direction; // temporary container for direction value
    id_t id = getNewId();
	direction = getDirVec2(angle);

    if (Game.actor_count >= Game.actor_capacity)
    {
        Game.actor_capacity += ACTOR_CHUNK_SIZE;
        Game.Actors = realloc(Game.Actors, Game.actor_capacity * sizeof(Actor_t));
        // to do later: ensure successful allocation
    }

    Game.ActorsById[id] = Game.actor_count;

    memset(&Game.Actors[Game.actor_count], 0, sizeof(Actor_t));

    Game.Actors[Game.actor_count].id = id;
    Game.Actors[Game.actor_count].position.x = x;
    Game.Actors[Game.actor_count].position.y = y;
    Game.Actors[Game.actor_count].angle = angle;
    Game.Actors[Game.actor_count].radius = radius;
    Game.Actors[Game.actor_count].control = control;
    Game.Actors[Game.actor_count].ai_mode = ai_mode;
    Game.Actors[Game.actor_count].ai_timer = ai_timer;
    Game.Actors[Game.actor_count].target_id_primary = ai_target;
    Game.Actors[Game.actor_count].target_id_secondary = UINT16_MAX;
    Game.Actors[Game.actor_count].health = health;
    Game.Actors[Game.actor_count].trigger_on_death = trigger_on_death;
    Game.Actors[Game.actor_count].primary_weapon = &Weapons[primary_weapon];
    Game.Actors[Game.actor_count].texture_id = loadTexture(texture_name, &ObjectTextures);
	Game.Actors[Game.actor_count].direction.x = direction.x;
	Game.Actors[Game.actor_count].direction.y = direction.y;
    Game.Actors[Game.actor_count].last_shot = System.ticks;
    Game.Actors[Game.actor_count].animset = NULL;

    Game.actor_count++;
    
    return id;
}

void deleteActor(id_t id)
{
    if (Game.ActorsById[id] != Game.actor_count - 1)
    {
        id_t deleted_object_index;
        id_t replacing_object_index;
        id_t replacing_object_id;

        deleted_object_index    = Game.ActorsById[id];
        replacing_object_index  = Game.actor_count - 1;
        replacing_object_id     = Game.Actors[Game.actor_count-1].id;

        Game.Actors[deleted_object_index]    = Game.Actors[replacing_object_index];
        Game.ActorsById[replacing_object_id] = deleted_object_index;
    }
    
    Game.actor_count--;

    // set value in ID-to-actor map to UINT16_MAX so it can be reused
    Game.ActorsById[id] = UINT16_MAX;

    if (Game.actor_count < Game.actor_capacity - ACTOR_CHUNK_SIZE)
    {
        Game.actor_capacity -= ACTOR_CHUNK_SIZE;
        Game.Actors = realloc(Game.Actors, Game.actor_capacity * sizeof(Actor_t));
    }
}

void deleteLastActor()
{
    if (Game.actor_count > 0)
        deleteActor(Game.actor_count - 1);
}

void initGameData(id_t actor_capacity, id_t id_capacity)
{
    Game.Actors = calloc(actor_capacity, sizeof(Actor_t));
    Game.ActorsById = calloc(id_capacity, sizeof(id_t));
    memset(Game.ActorsById, UINT16_MAX, id_capacity * sizeof(id_t));
}

void freeGameData()
{
    memset(Game.Map.tilemap, 0, Game.Map.width * Game.Map.height);
    free(Game.Map.tilemap);
    memset(Game.Actors, 0, Game.actor_capacity * sizeof(Actor_t));
    free(Game.Actors);
    Game.actor_count = 0;
    memset(Game.ActorsById, 0, Game.id_capacity * sizeof(id_t));
    free(Game.ActorsById);
    Game.id_capacity = 0;
    memset(Game.current_level_name, 0, strlen(Game.current_level_name) * sizeof(char));
}

void testInitPlayerAnim()
{
    PLAYER_ACTOR.sprite = DudeSprite;
    //PLAYER_ACTOR.animset = &DudeAnimSet;
}
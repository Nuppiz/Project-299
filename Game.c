#include "Common.h"
#include "Game.h"
#include "LvlLoad.h"
#include "Structs.h"
#include "Loadgfx.h"
#include "Vectors.h"
#include "General.h"

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

char* actor_variable_strings[] = 
{
    "walk_speed",
    "run_speed",
    "turn_rate",
    "radius",
    "health",
    "primary_weapon_id",
    "secondary_weapon_id",
    "anim"
};

char* actor_anim_strings[NUM_ACTORANIMS] =
{
    "idle",
    "walk",
    "melee_stationary",
    "melee_moving",
    "shoot",
};

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
    ActorTemplates[ACT_DEFAULT].name = malloc(12);
    ActorTemplates[ACT_DEFAULT].name = "DEFAULT.ACT";
    ActorTemplates[ACT_DEFAULT].walk_speed = 1.0; 
    ActorTemplates[ACT_DEFAULT].run_speed = 2.0;
    ActorTemplates[ACT_DEFAULT].turn_rate = 0.05 ;
    ActorTemplates[ACT_DEFAULT].radius = 5;
    ActorTemplates[ACT_DEFAULT].health = 50;
    ActorTemplates[ACT_DEFAULT].primary_weapon_id = 0; 
    ActorTemplates[ACT_DEFAULT].secondary_weapon_id = 0;
}

int findActorTemplate(char* name)
{
    int i;

    for (i = 0; i < actortemplate_count; i++)
    {
        if (strcmp(name, ActorTemplates[i].name) == 0)
            return i;
    }
    return 0;
}

int loadActorTemplate(char* filename)
{
    FILE* act_file;
    char c;
    char buffer[100];
    int actortemplate_id;
    ActorTemplate_t* actortemplate;

    act_file = fopen(filename, "r");

    if (act_file == NULL)
    {
        fclose(act_file);
        quitError("Unable to open actor template file!\n"
                  "Please check the file actually exists!\n");
        return 0;
    }

    if ((actortemplate_id = findActorTemplate(filename)) != 0)
        return actortemplate_id;

    actortemplate_id = actortemplate_count;
    actortemplate = &ActorTemplates[actortemplate_id];

    *actortemplate = ActorTemplates[ACT_DEFAULT];
    actortemplate->name = malloc(strlen(filename) + 1);
    strcpy(actortemplate->name, filename);

    do
    {
        int actor_var_id;

        if (c == '$')
        {
            fscanf(act_file, "%s", buffer);
            actor_var_id = searchStringList(buffer, actor_variable_strings, NUM_ACTOR_VARIABLE_STRINGS);

            switch (actor_var_id)
            {
                case ACT_VAR_WALK_SPEED:  fscanf(act_file, "%f", &actortemplate->walk_speed);           break;
                case ACT_VAR_RUN_SPEED:   fscanf(act_file, "%f", &actortemplate->run_speed);            break;
                case ACT_VAR_TURN_RATE:   fscanf(act_file, "%f", &actortemplate->turn_rate);            break;
                case ACT_VAR_RADIUS:      fscanf(act_file, "%d", &actortemplate->radius);               break;
                case ACT_VAR_HEALTH:      fscanf(act_file, "%d", &actortemplate->health);               break;
                case ACT_VAR_WEAPON1_ID:  fscanf(act_file, "%d", &actortemplate->primary_weapon_id);    break;
                case ACT_VAR_WEAPON2_ID:  fscanf(act_file, "%d", &actortemplate->secondary_weapon_id);  break;
                case ACT_VAR_ANIM:
                {
                    int actoranim_type_index;
                    char anim_name[20];
                    char anim_filename[30];

                    fscanf(act_file, "%s", anim_name);
                    actoranim_type_index = searchStringList(anim_name, actor_anim_strings, NUM_ACTORANIMS);

                    if (actoranim_type_index != RETURN_ERROR)
                    {
                        fscanf(act_file, "%s", anim_filename);
                        actortemplate->animset.anim_ids[actoranim_type_index] = loadAnimation(anim_filename);
                    }
                    else
                    {
                        fclose(act_file);
                        quitError("Error loading animset!\n");
                        return 0;
                    }
                }
            }
        }
    } while ((c = fgetc(act_file)) != EOF);

    fclose(act_file);
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
#include "Common.h"
#include "Game.h"
#include "Structs.h"
#include "Loadgfx.h"

/* Level data loader */

extern GameData_t Game;
Tile TileSet[100];

void loadTileset(char* filename)
{
    char c, symbol, tex_id;
    char name[50];

    while ((c = fgetc(filename)) != EOF)
    {
        if (c == '$')
        {
            symbol = fgetc(filename) - 32;
            fscanf(filename, "%s", name);
            tex_id = loadTileGfx(name);
            TileSet[symbol].texture_id = tex_id;

            while((c = fgetc(filename)) != '\n' && c != EOF)
            {
                switch (c)
                {
                case 'D': TileSet[symbol].material_type = 0; continue;
                case 'G': TileSet[symbol].material_type = 1; continue;
                case 'O': TileSet[symbol].obstacle = 1; continue;
                case 'P': TileSet[symbol].obstacle = 0; continue;
                case 'B': TileSet[symbol].block_bullets = 1; continue;
                case 'T': TileSet[symbol].block_bullets = 0; continue;
                
                default:  continue;
                }
            }
        }
    }
}

void loadMap()
{
    FILE* level_file;
    FILE* tileset;
    char buffer[100];
    char tileset_file[13];
    char c;
    int i = 0;
 
    int x, y;
    double angle;
    int radius, control, ai_mode, ai_timer;
    id_t ai_target, sprite_id;

    level_file = fopen("tiletest.txt", "rb");
    
    if (level_file == NULL)
    {
        setVideoMode(TEXT_MODE);
        printf("Unable to open file: tiletest.txt");
        printf("Please check the file actually exists!\n");
        quit();
    }

    while ((c = fgetc(level_file)) != EOF)
    {
        if (c == '$')
        {
            fscanf(level_file, "%s", buffer);
            if (strcmp(buffer, "tileset") == 0)
            {
                fscanf(level_file, "%s", tileset_file);
                tileset = fopen(tileset_file, "rb");
                if (tileset_file == NULL)
                {
                    setVideoMode(TEXT_MODE);
                    printf("Unable to open file: %s\n", tileset_file);
                    printf("Please check the file actually exists!\n");
                    quit();
                }

                loadTileset(tileset_file);
            }
            else if (strcmp(buffer, "leveldim") == 0)
            {
                fscanf(level_file, "%d %d", &Game.Map.width, &Game.Map.height);
                Game.Map.collision = malloc((Game.Map.width * Game.Map.height) * sizeof(uint8_t));
                Game.Map.tiles = malloc((Game.Map.width * Game.Map.height) * sizeof(uint8_t));
            }
            else if (strcmp(buffer, "player") == 0)
            {
                fscanf(level_file, "%d %d %lf %d %d %u",
                    &x, &y, &angle, &radius, &control, &sprite_id);
                Game.player_id = createObject((float)x, (float)y, angle, radius, control, 0, 0, 0, sprite_id);
                
            }
        }
    }
}

void levelLoader()
{
    FILE* level_file;
    char buffer[100];
    char filename[13];
    char c;
    int i = 0;
 
    int x, y;
    double angle;
    int radius, control, ai_mode, ai_timer;
    id_t ai_target, sprite_id;
    
    /* build filename and load it */
    sprintf(filename, "LEVELS/level%d.txt", Game.Map.level_num);
    level_file = fopen(filename, "rb");
    
    if (level_file == NULL)
    {
        setVideoMode(TEXT_MODE);
        printf("Unable to open file: %s\n", filename);
        printf("Please check the file actually exists!\n");
        quit();
    }
    
    while ((c = fgetc(level_file)) != EOF)
    {
        if (c == '$')
        {
            fscanf(level_file, "%s", buffer);
            if (strcmp(buffer, "leveldim") == 0)
            {
                fscanf(level_file, "%d %d", &Game.Map.width, &Game.Map.height);
                Game.Map.collision = malloc((Game.Map.width * Game.Map.height) * sizeof(uint8_t));
                Game.Map.tiles = malloc((Game.Map.width * Game.Map.height) * sizeof(uint8_t));
            }
            else if (strcmp(buffer, "player") == 0)
            {
                fscanf(level_file, "%d %d %lf %d %d %u",
                    &x, &y, &angle, &radius, &control, &sprite_id);
                Game.player_id = createObject((float)x, (float)y, angle, radius, control, 0, 0, 0, sprite_id);
                
            }
            else if (strcmp(buffer, "dude") == 0)
            {
                fscanf(level_file, "%d %d %lf %d %d %d %d %u %u",
                    &x, &y, &angle, &radius, &control, &ai_mode, &ai_timer, &ai_target, &sprite_id);
                createObject((float)x, (float)y, angle, radius, control, ai_mode, ai_timer, ai_target, sprite_id);
            }
            else if (strcmp(buffer, "collisiondata") == 0)
            {
                while (i < Game.Map.width * Game.Map.height)
                {
                    if (c != '\n')
                    {
                        fscanf(level_file, "%d", &Game.Map.collision[i]);
                        i++;
                    }
                    else
                        putchar('\n');
                }
            }
            else if (strcmp(buffer, "tiledata") == 0)
            {
                i = 0;
                while ((c = fgetc(level_file)) != EOF)
                {
                    if (c != '\n')
                    {
                        fscanf(level_file, "%d", &Game.Map.tiles[i]);
                        i++;
                    }
                    else
                        putchar('\n');
                }
            }
        }
    }
    fclose(level_file);
}
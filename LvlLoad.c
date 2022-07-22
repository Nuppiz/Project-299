#include "Common.h"
#include "Game.h"
#include "Structs.h"
#include "Loadgfx.h"

/* Level data loader */

extern GameData_t Game;
extern Texture_t* Textures;
Tile_t TileSet[100];

void loadTileset(char* filename)
{
    FILE* tileset;
    char c, symbol, tex_id;
    char name[50];

    tileset = fopen(filename, "rb");

    if (tileset == NULL)
        tileset = fopen(DEFAULT_TILESET, "rb");

    ASSERT(tileset != NULL);

    while ((c = fgetc(tileset)) != EOF)
    {
        if (c == '$')
        {
            symbol = fgetc(tileset) - 32;
            fscanf(tileset, "%s", name);
            TileSet[symbol].texture_id = loadTexture(name);

            while((c = fgetc(tileset)) != '\n' && c != EOF)
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
    fclose(tileset);
}

void levelLoader()
{
    FILE* level_file;
    char buffer[100];
    char tileset_file[20] = DEFAULT_TILESET;
    char texture_filename[20];
    char c;
    int i;
    int tileset_found = FALSE;
 
    int x, y;
    double angle;
    int radius, control, ai_mode, ai_timer;
    id_t ai_target, texture_id;

    level_file = fopen("LEVELS/tiletest.txt", "r");
    
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
                tileset_found = TRUE;
                loadTileset(tileset_file);
            }
            else if (strcmp(buffer, "leveldim") == 0)
            {
                fscanf(level_file, "%d %d", &Game.Map.width, &Game.Map.height);
                Game.Map.tilemap = malloc((Game.Map.width * Game.Map.height) * sizeof(Tile_t));
                ASSERT(Game.Map.width == 30);
                ASSERT(Game.Map.height == 30);
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
            }
            else if (strcmp(buffer, "player") == 0)
            {
                fscanf(level_file, "%d %d %lf %d %d %s",
                    &x, &y, &angle, &radius, &control, texture_filename);
                Game.player_id = createObject((float)x, (float)y, angle, radius, control, 0, 0, 0, texture_filename);
            }
            else if (strcmp(buffer, "dude") == 0)
            {
                fscanf(level_file, "%d %d %lf %d %d %d %d %u %s",
                    &x, &y, &angle, &radius, &control, &ai_mode, &ai_timer, &ai_target, texture_filename);
                createObject((float)x, (float)y, angle, radius, control, ai_mode, ai_timer, ai_target, texture_filename);
            }
        }
    }
    if (tileset_found == FALSE)
    {
        loadTileset(tileset_file);
    }
    fclose(level_file);
}
#include "Common.h"
#include "Structs.h"
#include "Menu.h"
#include "LvlLoad.h"
#include "Text.h"
#include "State.h"
#include "Filech.h"
#include "Input.h"
#include "Keyb.h"
#include "Draw.h"
#include "Video.h"

// Menu functionalities

Menu_t* current_menu;
extern unsigned musicVolume;
extern unsigned SFXVolume;
extern uint8_t music_on;
extern uint8_t SFX_on;
extern char levelname_global[];
extern System_t System;
extern Timer_t Timers;
extern GameData_t Game;
extern Keyboard_t Keyboard;

uint8_t story_text[] =
    "I COULD WRITE A FANCY STORY BUT\n" 
    "CAN'T BE ARSED RN LOL";

Option_t mainmenu_options[] =
{
    {"NEW GAME",  menuNewGame},
    {"LOAD GAME", menuLoadGame},
    {"OPTIONS",   menuOptions},
    {"HELP!",     menuHelp},
    {"STORY",     menuStory},
    {"QUIT",      quitGame}
};

Option_t loadmenu_options[] =
{
    {"EMPTY    ", loadGameFromMenu},
    {"EMPTY    ", loadGameFromMenu},
    {"EMPTY    ", loadGameFromMenu},
    {"EMPTY    ", loadGameFromMenu},
    {"EMPTY    ", loadGameFromMenu},
    {"EMPTY    ", loadGameFromMenu},
    {"EMPTY    ", loadGameFromMenu},
    {"EMPTY    ", loadGameFromMenu},
    {"EMPTY    ", loadGameFromMenu},
    {"EMPTY    ", loadGameFromMenu},
};

Option_t settings_options[] =
{
    {"SFX",         optSfxToggle},
    {"SFX VOL",     optSfxVolume},
    {"MUSIC",       optMusicToggle},
    {"MUSIC VOL",   optMusicVolume},
    {"KEY CONFIG",  menuKeyconf}
};

Option_t keyconf_options[] =
{
    {"UP",      dummy}, 
    {"LEFT",    dummy},
    {"DOWN",    dummy},
    {"RIGHT",   dummy}
};


Option_t basic_options[] =
{
  {"BACK TO MAIN", menuMain}
};

Option_t ingamemenu_options[] =
{
    {"NEW GAME",  menuNewGame},
    {"LOAD GAME", menuLoadGame},
    {"SAVE GAME", menuSaveGame},
    {"OPTIONS",   menuOptions},
    {"HELP!",     menuHelp},
    {"QUIT",      quitGame}
};

Option_t savemenu_options[] =
{
    {"EMPTY    ", saveGameFromMenu},
    {"EMPTY    ", saveGameFromMenu},
    {"EMPTY    ", saveGameFromMenu},
    {"EMPTY    ", saveGameFromMenu},
    {"EMPTY    ", saveGameFromMenu},
    {"EMPTY    ", saveGameFromMenu},
    {"EMPTY    ", saveGameFromMenu},
    {"EMPTY    ", saveGameFromMenu},
    {"EMPTY    ", saveGameFromMenu},
    {"EMPTY    ", saveGameFromMenu},
};

Menu_t mainmenu =
{
  6,
  0,
  65,
  65,
  105,
  15,
  mainmenu_options
};

Menu_t loadmenu =
{
  10,
  0,
  5,
  5,
  105,
  15,
  loadmenu_options
};

Menu_t optionsmenu =
{
  5,
  0,
  65,
  65,
  91,
  20,
  settings_options
};

Menu_t kconfmenu =
{
  4,
  0,
  65,
  65,
  80,
  30,
  keyconf_options
};

Menu_t helpmenu =
{
  1,
  0,
  180,
  180,
  80,
  0,
  basic_options
};

Menu_t storymenu =
{
  1,
  0,
  180,
  180,
  80,
  0,
  basic_options
};

Menu_t savemenu =
{
  10,
  0,
  5,
  5,
  105,
  15,
  savemenu_options
};

Menu_t ingamemenu =
{
  6,
  0,
  65,
  65,
  105,
  15,
  ingamemenu_options
};

void changeMenu()
{
    drawMenuText();
}

void cursorUp()
{
    changeMenu(); // optimize later
    if (current_menu->cursor_loc == 0) // if already at the top selection...
    {
        current_menu->cursor_y += (current_menu->num_selections - 1) * current_menu->cursor_spacing; // move cursor to bottom selection
        current_menu->cursor_loc = current_menu->num_selections - 1;
    }
    else
    {
        current_menu->cursor_y -= current_menu->cursor_spacing;
        current_menu->cursor_loc--;
    }
}

void cursorDown()
{
    changeMenu(); // optimize later
    if (current_menu->cursor_loc == current_menu->num_selections - 1) // if already at the bottom selection...
    {
        current_menu->cursor_y = current_menu->start_y; // move cursor to top selection
        current_menu->cursor_loc = 0;
    }
    else
    {
        current_menu->cursor_y += current_menu->cursor_spacing;;
        current_menu->cursor_loc++;
    }
}

void menuMain()
{
    current_menu = &mainmenu;
    changeMenu();
}

void menuLoadGame()
{
    int directory_count, i;
    int d = 0;
    char** directory_list;

    current_menu = &loadmenu;
    //directory_count = countSubdirectories("SAVES");
    //directory_list = malloc(directory_count * sizeof(char*));

    directory_count = listSubdirectories("SAVES", &directory_list);
    // CURRENT and QUICK should go first on the list
    for (i = 0; i < directory_count; i++)
    {
        if (strcmp(directory_list[i], "CURRENT") == 0 || strcmp(directory_list[i], "QUICK") == 0)
        {
            strcpy(loadmenu_options[d].text, directory_list[i]);
            d++;
        }
    }
    // then the rest
    for (i = 0; i < directory_count; i++)
    {
        if (strcmp(directory_list[i], "CURRENT") != 0 && strcmp(directory_list[i], "QUICK") != 0)
        {
            strcpy(loadmenu_options[d].text, directory_list[i]);
            d++;
        }
    }
    free(directory_list);
    changeMenu();
}

void menuSaveGame()
{
    int directory_count, i;
    int d = 0;
    char** directory_list;

    current_menu = &savemenu;
    //directory_count = countSubdirectories("SAVES");
    //directory_list = malloc(directory_count * sizeof(char*));
    
    directory_count = listSubdirectories("SAVES", &directory_list);
    // CURRENT and QUICK should go first on the list
    for (i = 0; i < directory_count; i++)
    {
        if (strcmp(directory_list[i], "CURRENT") == 0 || strcmp(directory_list[i], "QUICK") == 0)
        {
            strcpy(savemenu_options[d].text, directory_list[i]);
            d++;
        }
    }
    // then the rest
    for (i = 0; i < directory_count; i++)
    {
        if (strcmp(directory_list[i], "CURRENT") != 0 && strcmp(directory_list[i], "QUICK") != 0)
        {
            strcpy(savemenu_options[d].text, directory_list[i]);
            d++;
        }
    }
    free(directory_list);
    changeMenu();
}

void menuOptions()
{
    current_menu = &optionsmenu;
    changeMenu();
}

void menuKeyconf()
{
    current_menu = &kconfmenu;
    changeMenu();
}

void menuHelp()
{
    current_menu = &helpmenu;
    changeMenu();
}

void menuStory()
{
    current_menu = &storymenu;
    changeMenu();
    drawText(4, 52, story_text, COLOUR_WHITE);
}

void menuNewGame()
{
    popState();
    pushState(STATE_INGAME);
    levelLoader("LEVEL3.LEV", LOAD_NEW_LEVEL);
    if (!checkDirectoryExists("SAVES/CURRENT"))
    {
        createDirectory("SAVES/CURRENT");
    }
    else
        deleteDirectoryContents("SAVES/CURRENT");
    Timers.accumulator = 0;
    Timers.frame_count = 0;
    Timers.last_env_damage = 0;
    Timers.last_frame = 0;
    Timers.last_midas_time = 0;
    Timers.last_sfx = 0;
    Timers.last_tick = 0;
    Timers.last_time = 0;
}

void quitGame()
{
    System.running = 0;
}

void optSfxToggle()
{
    if (SFX_on == TRUE)
        SFX_on = FALSE;
    else
        SFX_on = TRUE;
    changeMenu(); // optimize later
}

void optSfxVolume()
{
    if (SFX_on == TRUE)
        SFX_on = FALSE;
    else
        SFX_on = TRUE;
    changeMenu(); // optimize later
}

void optMusicToggle()
{
    if (SFX_on == TRUE)
        SFX_on = FALSE;
    else
        SFX_on = TRUE;
    changeMenu(); // optimize later
}

void optMusicVolume()
{
    if (music_on == TRUE)
        music_on = FALSE;
    else
        music_on = TRUE;
    changeMenu(); // optimize later
}

void dummy()
{

}

void loadGameFromMenu()
{
    char foldername[10];
    char savepath[45] = {'\0'};
    char curstatepath[45];
    char savefilename[15] = {'\0'};

    strcpy(foldername, loadmenu_options[current_menu->cursor_loc].text);

    // if entry is labelled as empty, stop the function
    if (strcmp(foldername, "EMPTY    ") != 0)
    {
        // construct the basic strings
        strcpy(savepath, "SAVES/");

        strcat(foldername, "/"); // add slash to the save subfolder name
        strcat(savepath, foldername); // construct full folder path

        strcpy(curstatepath, savepath);
        strcat(curstatepath, "CURSTATE.SAV");

        // make sure the folder contains CURSTATE
        if (checkFileExists(curstatepath))
        {
            // check current level from the save folder
            checkLevelFromSave(foldername);

            strncpy(savefilename, levelname_global, (strlen(levelname_global) - 4)); // drop the level filename ending
            strcat(savefilename, ".SAV"); // add save filename ending
            strcat(savepath, savefilename);
            if (checkFileExists(savepath)) // if savefile exists, load save, else do nothing
            {
                popState();
                pushState(STATE_INGAME);
                levelLoader(levelname_global, LOAD_SAVED_LEVEL);
                loadLevelState(foldername, savefilename);
                loadGameState(foldername);
                copyAllFolderToFolder(savepath, "SAVES/CURRENT/");
            }
        }
        levelname_global[0] = '\0'; // reset levelname
    }
}

void saveGameFromMenu()
{
    // replace scanf with proper text input and printf with proper text output

    char foldername[10];
    TextInput_t folder_input;
    char savepath[25] = "SAVES/"; // needed for the copy function
    char savefilename[15] = {'\0'};
    uint8_t saving = TRUE;

    folder_input.capacity = 9;
    folder_input.buffer = calloc(folder_input.capacity, 1);

    strcpy(foldername, loadmenu_options[current_menu->cursor_loc].text);

    if (!KEY_WAS_HIT(KEY_ESC))
    {
        memset(foldername, 0, 10);

        if (KEY_WAS_HIT(KEY_ENTER))
        {
            Keyboard.keystates[KEY_ENTER] = 0; // reset Enter button to avoid double press
            resetInput(&folder_input);

            while (!KEY_WAS_HIT(KEY_ENTER))
            {
                gameDraw();
                drawMenuText();
                drawRectangle(current_menu->cursor_x + 18, current_menu->cursor_y - 1, 90, 10, 0);
                drawText(current_menu->cursor_x + 20, current_menu->cursor_y, folder_input.buffer, COLOUR_WHITE);
                processKeyEvents(TRUE, &folder_input);
                renderWithoutClear();
                if (KEY_WAS_HIT(KEY_ESC))
                {
                    saving = FALSE;
                    break;
                }
            }
        }
        if (saving == TRUE)
        {
            strcpy(foldername, folder_input.buffer);
            strcat(foldername, "/"); // add slash to the save subfolder name
            strcat(savepath, foldername);

            if (!checkDirectoryExists(savepath)) // if folder doesn't exist, create it
                createDirectory(savepath);

            copyAllFolderToFolder("SAVES/CURRENT/", savepath);
            strncpy(savefilename, Game.current_level_name, (strlen(Game.current_level_name) - 4)); // drop the level filename ending
            saveLevelState(foldername, savefilename);
            saveGameState(foldername);
        }
    }
    
    free(folder_input.buffer);
    popState();
}

void menuController()
{
    current_menu->options[current_menu->cursor_loc].action();
}
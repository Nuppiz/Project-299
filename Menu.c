#include "Common.h"
#include "Structs.h"
#include "Menu.h"
#include "LvlLoad.h"
#include "Text.h"
#include "State.h"
#include "Filech.h"

// Menu functionalities

Menu_t* current_menu;
extern unsigned musicVolume;
extern unsigned SFXVolume;
extern uint8_t music_on;
extern uint8_t SFX_on;
extern System_t System;

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
    {"EMPTY         ", menuLoadGame},
    {"EMPTY         ", menuLoadGame},
    {"EMPTY         ", menuLoadGame},
    {"EMPTY         ", menuLoadGame},
    {"EMPTY         ", menuLoadGame},
    {"EMPTY         ", menuLoadGame},
    {"EMPTY         ", menuLoadGame},
    {"EMPTY         ", menuLoadGame},
    {"EMPTY         ", menuLoadGame},
    {"EMPTY         ", menuLoadGame},
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
    char** directory_list;

    current_menu = &loadmenu;
    directory_count = countSubdirectories("SAVES");
    directory_list = malloc(directory_count * sizeof(char*));
    directory_list = listSubdirectories("SAVES", directory_count);
    for (i = 0; i < directory_count; i++)
    {
        strcpy(loadmenu_options[i].text, directory_list[i]);
    }
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

void menuController()
{
    current_menu->options[current_menu->cursor_loc].action();
}
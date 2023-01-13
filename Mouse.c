#include "Common.h"
#include "Structs.h"
#include "Mouse.h"
#include "Movecoll.h"
#include "Action.h" // for testing

extern GameData_t Game;
extern flags_t player_control;

Vec2 mouse_location;

int initMouse()
{
  union REGS regs;
  regs.x.ax = INIT_MOUSE;
  int86(MOUSE_INT, &regs, &regs);
  return 0xFFFF == regs.x.ax;
}

void showMouse()
{
  union REGS regs;
  regs.x.ax = SHOW_MOUSE;
  int86(MOUSE_INT, &regs, &regs);
}

void hideMouse()
{
  union REGS regs;
  regs.x.ax = HIDE_MOUSE;
  int86(MOUSE_INT, &regs, &regs);
}

void getMouse(int* x, int* y, int* left, int* right)
{
  union REGS regs;
  regs.x.ax = GET_MOUSE_STATUS;
  int86(MOUSE_INT, &regs, &regs);
  *x = regs.x.cx / 2;
  *y = regs.x.dx;
  *left = regs.x.bx & 0x1;
  *right = regs.x.bx & 0x2;
}

void playerMouseAim()
{
    playerTurnTowards(mouse_location);
}

void handleMouseInput()
{
    int mouse_x, mouse_y, mouse_lb, mouse_rb;

    getMouse(&mouse_x, &mouse_y, &mouse_lb, &mouse_rb);

    if (mouse_lb)
    {
        shootWeapon(PLAYER_ACTOR.primary_weapon_id, &PLAYER_ACTOR);
    }
    else if (mouse_rb)
    {
        if ((player_control & CONTROL_MOUSE_AIM) == 0)
            player_control |= CONTROL_MOUSE_AIM;
        else
            player_control &= ~CONTROL_MOUSE_AIM;
    }
    mouse_location.x = (float)mouse_x;
    mouse_location.y = (float)mouse_y;
}
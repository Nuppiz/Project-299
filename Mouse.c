#include "Common.h"
#include "Structs.h"
#include "Mouse.h"
#include "Movecoll.h"
#include "Action.h" // for testing

extern GameData_t Game;
extern flags_t player_control;

union REGS in, out;
Vec2 mouse_location;

int detectMouse()
{
  in.x.ax = INIT_MOUSE;
  int86(MOUSE_INT, &in, &out);
  return out.x.ax; 
}

void showMouse()
{
  in.x.ax = SHOW_MOUSE;
  int86(MOUSE_INT, &in, &out); 
}

void hideMouse()
{
  in.x.ax = HIDE_MOUSE;
  int86(MOUSE_INT, &in, &out); 
}

void getMouse(int* x, int* y, int* left, int* right)
{
  in.x.ax = GET_MOUSE_STATUS;
  int86(MOUSE_INT, &in, &out);
  *x = out.x.cx;
  *y = out.x.dx; 
  *left = out.x.bx & 0x1;
  *right = out.x.bx & 0x2;
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
      playerMouseAim();
    }
    mouse_location.x = (float)mouse_x;
    mouse_location.y = (float)mouse_y;
}
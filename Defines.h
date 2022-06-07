#ifndef DEFINES_H
#define DEFINES_H

#define VIDEO_INT           0x10      /* the BIOS video interrupt. */
#define SET_MODE            0x00      /* BIOS func to set the video mode. */
#define VGA_256_COLOR_MODE  0x13      /* use to set 256-color mode. */
#define TEXT_MODE           0x03      /* use to set 80x25 text mode. */

#define SCREEN_WIDTH        320       /* width in pixels of mode 0x13 */
#define SCREEN_HEIGHT       200       /* height in pixels of mode 0x13 */
#define NUM_COLORS          256       /* number of colors in mode 0x13 */

#define PALETTE_WRITE       0x03C8
#define PALETTE_DATA        0x03C9

#if SCREEN_WIDTH == 320
#define SET_PIXEL(x,y,color) screen_buf[(((y)<<8)+((y)<<6)) + (x)] = color
#else
#define SET_PIXEL(x,y,color) screen_buf[(y)*SCREEN_WIDTH + (x)] = color
#endif

#define KB_ARRAY_LENGTH     256
#define KB_QUEUE_LENGTH     256

#define KEY_HIT_FLAG        1
#define KEY_PRESSED_FLAG    2
#define KEY_RELEASED_FLAG   0x80 // 128, MSB
#define KEY_SPECIAL_CODE    0xE0 // 224
#define KEY_IS_PRESSED(k)   (g_Keyboard[k] & KEY_PRESSED_FLAG)
#define KEY_WAS_HIT(k)      (g_Keyboard[k] & KEY_HIT_FLAG)
#define KEY_WAS_RELEASED(k) (g_Keyboard[k] & KEY_RELEASED_FLAG)
#define KEY_WAS_TOGGLED(k)  (g_Keyboard[k] & (KEY_HIT_FLAG|KEY_RELEASED_FLAG))
#define KEY_LEFT            75
#define KEY_RIGHT           77
#define KEY_UP              72 
#define KEY_DOWN            80
#define KEY_F10             68
 
#define X_AXIS              1
#define Y_AXIS              2

#define SQUARE_SIZE         20
#define SCREEN_SIZE         64000
#define SQUARE_ROWS         SCREEN_HEIGHT / SQUARE_SIZE
#define SQUARE_COLUMNS      SCREEN_WIDTH / SQUARE_SIZE
#define degToRad(degree)    ((degree) * M_PI / 180.0)

#define RAD_360             (2*M_PI)
#define RAD_270             (4.172)
#define RAD_180             M_PI
#define RAD_120             (M_PI/1.5)
#define RAD_105             (M_PI-1.3)
#define RAD_90              (M_PI/2)
#define RAD_60              (M_PI/3)
#define RAD_45              (M_PI/4)
#define RAD_30              (M_PI/6)
#define RAD_15              (M_PI/12)

#define VIEW_ANGLE_COS      0.5
#define LOS_STEP            10
#define OUT_OF_SIGHT        0
#define IN_SIGHT            1

#define WALL                7
#define COLOUR_WHITE        15
#define COLOUR_RED          40
#define COLOUR_PEACH        64
#define TRANSPARENT_COLOR   13

#define player              object_array[0]
#define DRAG                1.05
#define MAX_SPEED           5.0
#define ACCELERATION_RATE   0.3
#define BRAKE_RATE          0.9
#define SPEED_THRESHOLD     0.2
#define TURN_RATE           10

#define CHASE_DISTANCE      75
#define CHASE_DISTANCE_SQ   CHASE_DISTANCE*CHASE_DISTANCE
#define CHASE_THRESHOLD     15
#define CHASE_TIMEOUT       100

#define TILE_WIDTH          8
#define TILE_HEIGHT         8
#define CHARACTER_SIZE      72

#define IDLE                0
#define CHASE_TARGET        1

#define TRUE                1
#define FALSE               0

#define FULLY_OUT           0
#define PARTIAL             1
#define FULLY_IN            2

#define CONTROL_8253        0x43
#define CONTROL_WORD        0x3C
#define COUNTER_0           0x40
#define TIME_KEEPER_INT     0x1C 
#define TIMER_18HZ          0xFFFF
#define TIMER_1000HZ        1193

#define LOW_BYTE(n)         (n & 0x00ff)
#define HIGH_BYTE(n)        ((n>>8) & 0x00ff)

#endif
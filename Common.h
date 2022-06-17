#ifndef COMMON_H
#define COMMON_H

/* Common standard libraries used by multiple C files */

#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <stdint.h>
#include <math.h>
#include <conio.h>

#include "Defines.h"
#include "Enums.h"

typedef long time_t;
typedef uint16_t id_t;
typedef uint16_t flags_t;

#if DEBUG == 1
extern char debug[NUM_DEBUG][DEBUG_STR_LEN];
#endif/* COMMON_H */

#endif/* COMMON_H */
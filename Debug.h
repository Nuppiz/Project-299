#ifndef DEBUG_H
#define DEBUG_H

#if DEBUG_ASSERT == 1
#include <stdio.h>

// evaluate expression; exit on false
#define ASSERT(exp) \
    if (!(exp)) { \
        fprintf(stderr, "%s:%d: Assert Error\n", __FILE__, __LINE__); \
        fprintf(stderr, "%s\n", #exp); \
        exit(0); }

// evaluate expression; infinite loop on false
#define ASSERT_L(exp) \
    if (!(exp)) { \
        fprintf(stderr, "%s:%d: Assert Error\n", __FILE__, __LINE__); \
        fprintf(stderr, "%s\n", #exp); \
        while(1); }

// evaluate A op B, print their values as (long) and exit on false
#define ASSERT_AB(a,op,b) \
    if (!((a) op (b))) { \
        fprintf(stderr, "%s:%d: Assert Error\n", __FILE__, __LINE__); \
        fprintf(stderr, "%s %s %s\n%ld %s %ld\n" \
            #a, #op, #b, (long)(a), #op, (long)(b)); \
        exit(0); }

// evaluate A op B, print their values as (long) and infinite loop on false
#define ASSERT_AB_L(a,op,b) \
    if (!((a) op (b))) { \
        fprintf(stderr, "%s:%d: Assert Error\n", __FILE__, __LINE__); \
        fprintf(stderr, "%s %s %s\n%ld %s %ld\n" \
            #a, #op, #b, (long)(a), #op, (long)(b)); \
        while(1); }

#else
#define ASSERT(exp)
#define ASSERT_L(exp)
#define ASSERT_AB(a,op,b)
#define ASSERT_AB_L(a,op,b)
#endif

#endif /* DEBUG_H */

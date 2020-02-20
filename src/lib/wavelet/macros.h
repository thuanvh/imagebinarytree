// Onur G. Guleryuz 1995, 1996, 1997,
// University of Illinois at Urbana-Champaign,
// Princeton University,
// Polytechnic University.

#if !defined(_MACROS_)
#define _MACROS_

// For printf().
#include <stdio.h>
// For exit().
#include <stdlib.h>

#define check_ptr(ptr,fn) \
  if((ptr)==NULL) { \
    printf("%10s: NULL pointer\n",(char *)(fn)); \
    perror((char *)(fn)); \
    exit(1); \
  }

#endif
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 

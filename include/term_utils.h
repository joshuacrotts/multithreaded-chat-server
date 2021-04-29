#ifndef TERM_UTILS_H
#define TERM_UTILS_H

#include <stdio.h>

extern void cursor_pos( FILE *stream, int x, int y );
extern void cursor_color( FILE *stream );
extern void clr_scr( FILE *stream );

#endif // TERM_UTILS_H
#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stddef.h>

extern bool str_isempty( const char *str );
extern bool str_eq( const char *s1, const char *s2 , size_t n );
extern int  str_count( const char *str, const char *search );
extern int  strcpy_n( char *dst, const char *src, size_t dst_size );

#endif // UTILS_H

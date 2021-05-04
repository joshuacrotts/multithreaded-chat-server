#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stddef.h>

#define SEC_PER_HR  3600
#define SEC_PER_MIN 60

enum MSG_TYPE { MSG_INIT_CONN, MSG_CLIENT_CONN, MSG_CLIENT_DISCONN, MSG_CLIENT, MSG_BROADCAST };

extern bool  str_isempty( const char *str );
extern bool  str_eq( const char *s1, const char *s2, size_t n );
extern int   str_count( const char *str, const char *search );
extern int   strcpy_n( char *dst, const char *src, size_t dst_size );
extern void *s_malloc( size_t size );
extern void *s_calloc( size_t n, size_t size );
extern void *s_realloc( void *data, size_t size );

#endif // UTILS_H

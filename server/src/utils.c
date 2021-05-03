#include <stdio.h>

#include "string.h"
#include "utils.h"

/**
 * Returns whether or not two strings are equal. This is
 * equivalent to !strcmp(s1, s2) but is easier to read in
 * my opinion.
 *
 * @param const char *s1 to compare.
 * @param const char *s2 to compare.
 * @param size_t number of chars to compare.
 *
 * @return true if strings contain same characters, false otherwise.
 */
bool
str_eq( const char *s1, const char *s2, size_t n ) {
  if ( s1 == NULL || s2 == NULL ) {
    return false;
  }

  return !strncmp( s1, s2, n );
}

/**
 * Safely copies a string from src to the dest buffer.
 *
 * <p>
 * Because strncpy doesn't null-terminate, it makes for a rather
 * poor implementation for what this program needs. Further, the
 * snprintf function is a little cumbersome. So, I wrap it in this
 * function.
 * </p>
 *
 * @param char * ptr to destination buffer of string.
 * @param const char * ptr to source string to copy.
 * @param size_t size of destination buffer.
 *
 * @return int number of chars written to dest.
 */
int
strcpy_n( char *dst, const char *src, size_t dst_size ) {
  return snprintf( dst, dst_size, "%s", src );
}

/**
 * Returns whether or not the string is "empty". We define empty
 * as being either NULL OR only having the null terminating character.
 *
 * @param const char * string to check.
 *
 * @return true if the string is empty, false otherwise.
 */
bool
str_isempty( const char *str ) {
  return str != NULL && str[0] == '\0';
}

/**
 * Counts the number of times the search string appears in the
 * str (needle and haystack algorithm).
 *
 * @param const char * string to look in.
 * @param const char * string to search for.
 *
 * @return number of times the search string was found.
 */
int
str_count( const char *str, const char *search ) {
  int         count = 0;
  const char *temp  = str;
  while ( ( temp = strstr( temp, search ) ) ) {
    count++;
    temp++;
  }

  return count;
}

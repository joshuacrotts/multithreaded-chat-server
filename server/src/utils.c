#include "string.h"

#include "utils.h"

/**
 *
 * @param
 * @param
 * @param
 *
 * @return
 */
inline bool
streq( const char *s1, const char *s2, size_t len ) {
  return strncmp( s1, s2, len ) == 0;
}

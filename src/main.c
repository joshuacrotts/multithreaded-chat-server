#include <stdio.h>
#include <string.h>

#include "task_queue.h"
#include "client_list.h"
#include "utils.h"
#include "server.h"

#define ARG_LEN 2

static void print_usage( void );

server_t server;

/**
 *
 * @param
 * @param
 *
 * @return
 */
int
main( int argc, char *argv[] ) {
  if ( argc != 2 ) {
    print_usage();
  } else if ( streq( argv[1], "-s", ARG_LEN ) ) {
    server_init();
  } else if ( streq( argv[1], "-l", ARG_LEN ) ) {

  } else if ( streq( argv[1], "-q", ARG_LEN ) ) {
  } else {
    print_usage();
  }

  return 0;
}

/**
 *
 * @param
 *
 * @return
 */
static void
print_usage( void ) {
  fprintf( stderr, "Usage: ./main [-s | -l | -q]\n"
                   "-s\tRun server\n"
                   "-l\tTest linked list structure\n"
                   "-q\tTest queue structure\n" );
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "client_list.h"
#include "server.h"
#include "task_queue.h"
#include "utils.h"

#define ARG_LEN 2

static void print_usage( void );
static void linked_list_test( void );

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
  srand( time( NULL ) );
  if ( argc != 2 ) {
    print_usage();
  } else if ( streq( argv[1], "-s", ARG_LEN ) ) {
    server_init();
  } else if ( streq( argv[1], "-l", ARG_LEN ) ) {
    linked_list_test();
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

static void
linked_list_test() {
  client_list_t linked_list;
  client_list_create( &linked_list );

  while ( true ) {
    char * buff;
    size_t in = 0;
    while ( getline( &buff, &in, stdin ) != -1 ) {
      if ( streq( buff, "add", in ) ) {
        client_list_add( &linked_list, client_create( "Test str", rand() ) );
      } else if ( streq( buff, "print", in ) ) {
        struct client_node_s *cns = NULL;
        for ( cns = linked_list.head; cns != NULL; cns = cns->next ) {
          printf( "%s, %d\t", cns->client->name, cns->client->comm_id );
        }
        printf( "\n" );
      }

      free( buff );
    }
  }
}
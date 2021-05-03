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
  if ( argc != ARG_LEN ) {
    print_usage();
  } else if ( str_eq( argv[1], "-s", ARG_LEN ) ) {
    server_init();
  } else if ( str_eq( argv[1], "-l", ARG_LEN ) ) {
    linked_list_test();
  } else if ( str_eq( argv[1], "-q", ARG_LEN ) ) {
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

/**
 * 
 */
static void
linked_list_test() {
  client_list_t linked_list;
  client_list_create( &linked_list );
  printf("%p\n", &linked_list.mutex);

  while ( true ) {
    char buff[1024];
    while ( fgets(buff, sizeof buff, stdin ) != NULL ) {
      buff[strlen(buff) - 1] = '\0';
      if ( str_eq( buff, "add", sizeof buff ) ) {
      } else if ( str_eq( buff, "print", sizeof buff ) ) {
        struct client_node_s *cns = NULL;
        for ( cns = linked_list.head; cns != NULL; cns = cns->next ) {
          printf( "%s, %d\t", cns->client->name, cns->client->comm_id );
        }
        printf( "\n" );
      }
    }
  }
}

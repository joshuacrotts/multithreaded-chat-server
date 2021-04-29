#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "client.h"
#include "server.h"

static void *client_listen( void * );

extern server_t server;

/**
 *
 * @param
 * @param
 *
 * @return
 */
struct client_s *
client_create( const char *ip, int comm_id ) {
  struct client_s *client = malloc( sizeof( struct client_s ) );
  if ( client == NULL ) {
    fprintf( stderr, "Could not allocate memory for client.\n" );
    exit( EXIT_FAILURE );
  }

  client->name    = ( char * ) ip;
  client->comm_id = comm_id;
  client->flags   = CLIENT_CONNECTED;
  pthread_create( &client->pid, NULL, client_listen, ( void * ) client );

  return client;
}

/**
 *
 * @param
 *
 * @return
 */
void
client_destroy( struct client_s *client ) {
  if ( close( client->comm_id ) < 0 ) {
    fprintf( stderr, "Could not close client communication id.\n" );
  }

  if ( fclose( client->read_fp ) < 0 ) {
    fprintf( stderr, "Could not close client read file pointer.\n" );
  }

  if ( fclose( client->write_fp ) < 0 ) {
    fprintf( stderr, "Could not close client write file pointer.\n" );
  }

  if ( delwin( client->output_window ) < 0 ) {
    fprintf( stderr, "Could not free client output window.\n" );
  }

  if ( delwin( client->input_window ) < 0 ) {
    fprintf( stderr, "Could not free client input window.\n" );
  }

  free( client );
}

/**
 *
 * @param
 *
 * @return
 */
static void *
client_listen( void *c ) {
  struct client_s *client = ( struct client_s * ) c;
  pthread_detach( client->pid );
  return NULL;
}

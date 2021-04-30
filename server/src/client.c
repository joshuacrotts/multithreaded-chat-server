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
void
client_create( const char *ip, int comm_id ) {
  struct client_s *client = malloc( sizeof( struct client_s ) );
  if ( client == NULL ) {
    fprintf( stderr, "Could not allocate memory for client.\n" );
    exit( EXIT_FAILURE );
  }

  client->name    = ( char * ) ip;
  client->comm_id = comm_id;
  client->flags   = CLIENT_CONNECTED;

  // Open the file pointers from the socket conn.
  // We need to verify that the ptrs are valid before continuing. If not, free.
  if ( ( client->write_fp = fdopen( dup( client->comm_id ), "w" ) ) == NULL ) {
    close( client->comm_id );
    free( client );
    fprintf( stderr, "Could not open client write file-pointer.\n" );
    exit( EXIT_FAILURE );
  }

  if ( ( client->read_fp = fdopen( dup( client->comm_id ), "r" ) ) == NULL ) {
    close( client->comm_id );
    fclose( client->write_fp );
    free( client );
    fprintf( stderr, "Could not open client read file-pointer.\n" );
    exit( EXIT_FAILURE );
  }

  // Now, add it to our linked list.
  client_list_add( &server.client_list, client );
  pthread_create( &client->pid, NULL, client_listen, ( void * ) client );
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
  while ( client->flags & CLIENT_CONNECTED ) {
  }

  return NULL;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "client.h"
#include "server.h"
#include "utils.h"

static void *client_listen( void * );
static void  client_parse_command( struct client_s *client, char *command );
static void  client_send_message( struct client_s *client, const struct text_attribute_s *attr, const char *msg );
static void  client_parse_leave( struct client_s *client, char *leave_command );
static void  client_parse_login( struct client_s *client, char *login_command );

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
  struct client_s *client = calloc( 1, sizeof( struct client_s ) );
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

  printf( "Freeing..\n" );
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
    char buff[1024];
    memset(buff, 0, sizeof buff);
    fflush( client->write_fp );
    if ( fgets( buff, sizeof buff, client->read_fp ) != NULL ) {
      buff[strcspn( buff, "\n" )] = 0;
      client_parse_command( client, buff );
    }
  }

  printf( "Removed index %d\n", client_list_remove( &server.client_list, client ) );
  return NULL;
}

/**
 *
 */
static void
client_parse_command( struct client_s *client, char *cmd ) {
  // Tokenize the command.
  char * rest    = cmd;
  char * command = strtok_r( rest, " ", &rest );
  size_t len     = strlen( command );

  // If there isn't a command or the client is NULL, just quit early.
  if ( command == NULL || client == NULL ) {
    return;
  }

  // Now parse each individual command.
  if ( str_eq( command, "leave", len ) ) {
    client_parse_leave( client, rest );
  } else if ( str_eq( command, "login", len ) ) {
    client_parse_login( client, rest );
  }
}

/**
 *
 */
static void
client_send_message( struct client_s *client, const struct text_attribute_s *attr,
                     const char *msg ) {
  if ( attr != NULL ) {
    fprintf( client->write_fp, "%d,%d\n\n%s\n", attr->style_flag, attr->color, msg );
    printf("SERVER: %d,%d\\n\\n%s\\n", attr->style_flag, attr->color, msg );
  } else {
    fprintf( client->write_fp, "\n\n%s\n", msg );
    printf("SERVER: \\n\\n%s\\n",  msg );
  }
}

/**
 *
 */
static void
client_parse_login( struct client_s *client, char *login_command ) {}

/**
 *
 */
static void
client_parse_leave( struct client_s *client, char *leave_command ) {
  printf("Leave command: %s\n", leave_command);
  if ( !str_isempty( leave_command ) ) {
    struct text_attribute_s t;
    t.style_flag = TEXT_ATTR_ITALIC;
    t.color      = 0xff000000;
    client_send_message( client, &t, "Error usage: <leave>");
  } else if (client->flags & CLIENT_IN_ROOM) {
    client->flags &= ~CLIENT_IN_ROOM;
  } else {
    client->flags &= ~CLIENT_CONNECTED;
  }
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "client.h"
#include "server.h"
#include "utils.h"

static void *client_listen( void * );
static void  client_parse_command( struct client_s *client, char *command );
static void  client_send_message( struct client_s *client, const struct text_attribute_s *attr, const char *msg );
static void  client_parse_leave( struct client_s *client, char *leave_command );
static void  client_parse_login( struct client_s *client, char *login_command );
static void  client_parse_msg( struct client_s *client, char *msg_command );
static void  client_leave( struct client_s *client );
static void  client_login( struct client_s *client );
static void  client_msg( struct task_s *task );
static void  client_gettime( char *buffer, size_t buffer_size );

static const struct text_attribute_s ERR_ATTR = { TEXT_ATTR_ITALIC, 0xff0000ff };   /* */
static const struct text_attribute_s SEC_ATTR = { TEXT_ATTR_ITALIC, 0x0000007f };   /* */
static const struct text_attribute_s OK_ATTR  = { 0, 0x000000ff };                  /* */

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
  client->text_attributes = OK_ATTR;

  // Open the file pointers from the socket conn.
  // We need to verify that the ptrs are valid before continuing. If not, free.
  if ( ( client->write_fp = fdopen( dup( client->comm_id ), "w" ) ) == NULL ) {
    close( client->comm_id );
    free( client );
    fprintf( stderr, "Could not open client write file-pointer.\n" );
    exit( EXIT_FAILURE );
  }
  // Disable buffering for the write fp.
  setbuf( client->write_fp, NULL );

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
 */
void
client_process_task( struct task_s *task ) {
  switch ( task->task_type ) {
  case TASK_MSG:
    client_msg( task );
    break;
  default:
    fprintf( stderr, "Unrecognized task: %d.\n", task->task_type );
  }
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

  printf( "Freeing.\n" );
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

  char buff[1024];
  memset( buff, 0, sizeof buff );
  while ( client->flags & CLIENT_CONNECTED ) {
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
  // If the user isn't sending a command, they're sending a broadcast
  // message to everyone in the room. This is checked first because
  // we don't want to tokenize the message.
  if ( ( client->flags & CLIENT_LOGGED_IN ) && cmd[0] != '/' ) {
    struct task_s *task = calloc( 1, sizeof( struct task_s ) );
    if (task == NULL) {
      fprintf( stderr, "Could not allocate memory for task struct.\n" );
      exit( EXIT_FAILURE );
    }

    task->task_type     = TASK_MSG;
    task->sender        = client;
    strcpy_n( task->receiver, "BROADCAST", sizeof task->data );
    strcpy_n( task->data, cmd, sizeof task->data );
    task_queue_enqueue( &server.task_queue, task );
    return;
  }
  
  // Tokenize the command.
  char * rest    = cmd;
  char * command = strtok_r( rest, " ", &rest );
  size_t len     = strlen( command );

  // The user must be logged in to use any commands (except /login).
  if ( client->flags & CLIENT_LOGGED_IN ) {
    if ( str_eq( command, "/leave", len ) ) {
      client_parse_leave( client, rest );
    } else if ( str_eq( command, "/msg", len ) ) {
      client_parse_msg( client, rest );
    } else if ( str_eq( command, "/login", len) ) {
      client_parse_login( client, rest );
    }
  } else {
    // Otherwise, if they are logging in, let them.
    if ( str_eq( command, "/login", len ) ) {
      client_parse_login( client, rest );
    } else {
      client_send_message( client, &ERR_ATTR, "Error - no commands are allowed before logging in." );
    }
  }

}

/**
 *
 */
static void
client_send_message( struct client_s *client, const struct text_attribute_s *attr,
                     const char *msg ) {
  if ( attr != NULL ) {
    fprintf( client->write_fp, "%d,%d,%s\n", attr->style_flag, attr->color, msg );
  } else {
    fprintf( stderr, "Error - attr is NULL; this shouldn't be possible...\n" );
    exit( EXIT_FAILURE );
  }
}

/**
 *
 */
static void
client_parse_login( struct client_s *client, char *login_command ) {
  if ( str_isempty( login_command ) || str_count( login_command, " " ) > 1 ) {
    client_send_message( client, &ERR_ATTR, "Error - usage: /login <name> <password>");
  } else {
    client_login( client );
  }
}

/**
 *
 */
static void
client_login( struct client_s *client ) {
  client->flags |= CLIENT_LOGGED_IN;

  // Display the current uptime of the server.
  struct timespec curr_time;
  clock_gettime( CLOCK_MONOTONIC_RAW, &curr_time );

  // Compute the current elapsed time from the start of the server to now.
  time_t now = curr_time.tv_sec - server.start_time.tv_sec;

  // Convert the tm struct to a string.
  char buffer[256];
  snprintf( buffer, sizeof buffer, "Logged in. Server uptime: %ldh:%ldm:%lds", ( now / SEC_PER_HR ),
                                                                               ( ( now / SEC_PER_MIN ) % SEC_PER_MIN ),
                                                                               ( now % SEC_PER_MIN ) );
  client_send_message( client, &SEC_ATTR, buffer );
}

/**
 *
 */
static void
client_parse_leave( struct client_s *client, char *leave_command ) {
  if ( !str_isempty( leave_command ) ) {
    client_send_message( client, &ERR_ATTR, "Error - usage: /leave" );
  } else {
    client_leave( client );
  }
}

/**
 *
 */
static void
client_leave( struct client_s *client ) {
  if ( client->flags & CLIENT_IN_ROOM ) {
    client->flags &= ~CLIENT_IN_ROOM;
  } else {
    client->flags &= ~CLIENT_CONNECTED;
    client_send_message( client, &SEC_ATTR, "Disconnected." );
  }
}

/**
 *
 */
static void
client_parse_msg( struct client_s *client, char *msg_command ) {
  char *msg      = NULL;
  char *receiver = strtok_r( msg_command, " ", &msg );

  if ( receiver == NULL || str_isempty( msg ) ) {
    client_send_message( client, &ERR_ATTR, "Error - usage /msg <usr> <msg>" );
  } else {
    struct task_s *task = calloc( 1, sizeof( struct task_s ) );
    if ( task == NULL ) {
      fprintf( stderr, "Could not allocate memory for task struct.\n" );
      exit( EXIT_FAILURE );
    }

    strcpy_n( task->receiver, receiver, sizeof task->receiver );
    strcpy_n( task->data, msg, sizeof task->data );
    task_queue_enqueue( &server.task_queue, task );
  }
}

/**
 *
 */
static void
client_msg( struct task_s *msg_task ) {
  const bool BROADCAST = str_eq( msg_task->receiver, "BROADCAST", strlen( msg_task->receiver ) );
  char buffer[2048];
  char time_buffer[128];
  struct client_node_s *curr;

  // Append the time and create the message to send.
  client_gettime( time_buffer, sizeof time_buffer );
  snprintf( buffer, sizeof buffer, "<%s - %s> %s", time_buffer, msg_task->sender->name, msg_task->data );

  // Iterate through the server to find the client we're looking for.
  for ( curr = server.client_list.head; curr != NULL; curr = curr->next ) {
    if ( BROADCAST ) {
      client_send_message( curr->client, &curr->client->text_attributes, buffer );
    } else if ( str_eq( msg_task->receiver, curr->client->name, strlen( msg_task->receiver ) ) ) {
      client_send_message( curr->client, &SEC_ATTR, buffer );
    }
  }
}

/**
 *
 */
static void
client_gettime( char *buffer, const size_t buffer_size ) {
  time_t rawtime;
  time( &rawtime );
  struct tm *time_now = localtime( &rawtime );
  strftime( buffer, buffer_size, "%F %T", time_now );
}

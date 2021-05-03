#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "client.h"
#include "server.h"
#include "utils.h"

static void *client_listen( void * );
static void  client_process_task( struct task_s *task );
static void  client_parse_command( struct client_s *client, char *command );
static void  client_send_message( struct client_s *client, const struct text_attribute_s *attr, const char *msg );
static void  client_parse_leave( struct client_s *client, char *leave_command );
static void  client_parse_login( struct client_s *client, char *login_command );
static void  client_parse_msg( struct client_s *client, char *msg_command );

static void client_leave( struct client_s *client );
static void client_login( struct client_s *client );
static void client_msg( struct task_s *task );

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
client_process_task( struct task_s *task ) {
  switch (task->task_type) {
    case TASK_MSG:
      client_msg( task );
      break;
    default:
      fprintf( stderr, "Unrecognized task: %d.\n", task->task_type );
  }
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

  // If the command is empty, that means they want to send a message to
  // everyone in the same room.
  if ( command == NULL ) {
    struct task_s *task = calloc( 1, sizeof( struct task_s ) );
    task->task_type = TASK_MSG;
    task->sender = client;
    task->receiver = "BROADCAST";
    task->data = rest;
    task_queue_enqueue( &server.task_queue, task );
  } else if ( str_eq( command, "/leave", len ) ) {
    client_parse_leave( client, rest );
  } else if ( str_eq( command, "/login", len ) ) {
    client_parse_login( client, rest );
  } else if ( str_eq( command, "/msg", len ) ) {
    client_parse_msg( client, rest );
  }
}

/**
 *
 */
static void
client_send_message( struct client_s *client, const struct text_attribute_s *attr,
                     const char *msg ) {
  if ( attr != NULL ) {
    fprintf( client->write_fp, "%d,%d,%s", attr->style_flag, attr->color, msg );
    printf("SERVER: %d,%d,%s\\n\n", attr->style_flag, attr->color, msg );
  } else {
    fprintf( client->write_fp, "%s", msg );
    printf("SERVER: %s\\n\n",  msg );
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
client_login( struct client_s *client) {

}

/**
 *
 */
static void
client_parse_leave( struct client_s *client, char *leave_command ) {
  if ( !str_isempty( leave_command ) ) {
    struct text_attribute_s t = {TEXT_ATTR_ITALIC, 0xff0000ff};
    client_send_message( client, &t, "Error - usage: /leave");
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
  }
}

/**
 *
 */
static void 
client_parse_msg( struct client_s *client, char *msg_command ) {
  char *msg = NULL;
  char *receiver = strtok_r( msg_command, " ", &msg );

  if ( receiver == NULL || str_isempty( msg ) ) {
    struct text_attribute_s t = {TEXT_ATTR_ITALIC, 0xff0000ff};
    client_send_message( client, &t, "Error - usage /msg <usr> <msg>" );
  } else {
    struct task_s *task = calloc( 1, sizeof ( struct task_s ) );
    task->task_type = TASK_MSG;
    task->sender = client;
    task->receiver = receiver;
    task->data = msg;
    task_queue_enqueue( &server.task_queue, task );
  }
}

/**
 *
 */
static void 
client_msg( struct task_s *msg_task ) {
  const bool BROADCAST = str_eq( msg_task->receiver, "BROADCAST", strlen( msg_task->receiver ) );
  struct client_node_s *curr;

  // Iterate through the server to find the client we're looking for.
  for ( curr = server.client_list.head; curr != NULL; curr = curr->next ) {
    if ( BROADCAST ) {
      client_send_message( curr->client, &curr->client->text_attributes, msg_task->data );
    } else if ( str_eq( msg_task->receiver, curr->client->name, strlen( msg_task->receiver ) ) ) {
      struct text_attribute_s t = {TEXT_ATTR_ITALIC, 0x0000007f};
      client_send_message( curr->client, &t, msg_task->data );
    }
  }
}


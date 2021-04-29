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

  // Redirect the curses i/o to our write and read fps.
  // Acquire the lock since we're manipulating the global stdscr var.
  pthread_mutex_lock( &server.client_list.mutex );
  client->screen = newterm( NULL, client->write_fp, client->read_fp );
  if ( client->screen == NULL ) {
    fprintf( stderr, "Could not create curses SCREEN *\n" );
    exit( EXIT_FAILURE );
  }

  // Get the dimensions of the currenty screen and create the ncurses windows.
  int parent_x, parent_y;
  getmaxyx( stdscr, parent_y, parent_x );
  const int TEXT_HEIGHT = 3;
  client->output_window = newwin( parent_y - TEXT_HEIGHT, parent_x, 0, 0 );
  client->input_window  = newwin( TEXT_HEIGHT, parent_x, parent_y - TEXT_HEIGHT, 0 );

  // Now, add it to our linked list.
  pthread_mutex_unlock( &server.client_list.mutex );
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

  if ( delwin( client->output_window ) < 0 ) {
    fprintf( stderr, "Could not free client output window.\n" );
  }

  if ( delwin( client->input_window ) < 0 ) {
    fprintf( stderr, "Could not free client input window.\n" );
  }

  endwin();
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
    mvwprintw( client->output_window, 0, 0, "Hi dookie world!\n" );
    mvwprintw( client->input_window, 0, 0, "> ");
    wrefresh( client->output_window );
    wrefresh( client->input_window );
  }

  return NULL;
}

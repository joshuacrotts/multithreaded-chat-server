#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "server.h"
#include "tcpdefs.h"

#define DEBUG 1

static void server_listen( void );
static void server_close( int );
static int server_create_listener( char *service );
static void server_close_handle_init( void );

/**
 *
 */
void
server_init( void ) {
  server.socket_fd = server_create_listener( SERVER_PORT );
  if ( server.socket_fd < 0 ) {
    fprintf( stderr, "DEBUG: Could not initialize socket file descriptor.\n" );
    exit( EXIT_FAILURE );
  }

  printf( "SERVER: Chat server starting...\n" );

  server.flags = SERVER_ACTIVE;
  client_list_create( &server.client_list );
  task_queue_create( &server.task_queue );

  // Set up the threadpool here...
  server_close_handle_init();
  server_listen();
}

/**
 * Initializes the socket listener.
 *
 * @param char * string ptr to the service we want to start the socket on.
 * For this server, it is localhost.
 *
 * @return int socket file descriptor set up.
 */
static int
server_create_listener( char *service ) {
  int socket_fd;
  if ( ( socket_fd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 ) {
    fprintf( stderr, "DEBUG: Could not create socket listener.\n" );
    return -1;
  }

  // Let us reuse the port immediately for debugging.
  int optval = 1;
  setsockopt( socket_fd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof( optval ) );

  // Now create the addrinfo hints for the connection later.
  struct addrinfo hints;
  memset( &hints, 0, sizeof( hints ) );
  hints.ai_flags    = AI_PASSIVE;
  hints.ai_family   = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = 0;

  struct addrinfo *result;
  int              return_val;
  if ( ( return_val = getaddrinfo( NULL, service, &hints, &result ) ) != 0 ) {
    fprintf( stderr, "DEBUG: Error in getaddrinfo: %s\n", gai_strerror( return_val ) );
    close( socket_fd );
    return -1;
  }

  // Binds the name and address to the socket.
  int bind_return_val = bind( socket_fd, result->ai_addr, result->ai_addrlen );
  if ( bind_return_val < 0 ) {
    fprintf( stderr, "DEBUG: Could not bind name and address to socket.\n" );
    close( socket_fd );
    return -1;
  }

  // Lastly, set up the listener.
  int listen_return_val = listen( socket_fd, 128 );
  if ( listen_return_val < 0 ) {
    fprintf( stderr, "DEBUG: Could not initialize socket listener.\n" );
    close( socket_fd );
    return -1;
  }

  freeaddrinfo( result );
  return socket_fd;
}

/**
 * 
 * @param
 * 
 * @return
 */
static void 
server_listen( void ) {
  struct sockaddr_storage client_addr;
  socklen_t               client_addr_len = sizeof( client_addr );
  int                     comm_fd;

  // Each time we receive a new connection, we'll iterate through this loop again.
  while ( ( comm_fd = accept( server.socket_fd, ( struct sockaddr * ) &client_addr,
                              &client_addr_len ) ) >= 0 ) {
    char *user = inet_ntoa( ( ( struct sockaddr_in * ) &client_addr )->sin_addr );
    printf( "SERVER: Got connection from %s\n", user );
    client_create( user, comm_fd );
  }
}

/**
 * Shuts the server down when an interrupt or quit signal is received.
 *
 * <p>
 * The server does not free resources unless it is shut down properly - meaning that if
 * an interrupt is received, the lists aren't freed. While this isn't strictly necessary
 * since the application releases all memory either way at the end, I still wanted to
 * have this here to properly close the socket.
 * </p>
 *
 * @param void.
 *
 * @return void.
 */
static void
server_close_handle_init( void ) {
  struct sigaction sa;
  sa.sa_handler = server_close;
  sa.sa_flags   = 0;
  sigemptyset( &sa.sa_mask );
  sigaction( SIGINT, &sa, NULL );
  sigaction( SIGQUIT, &sa, NULL );
}

/**
 * Closes the socket file descriptor.
 *
 * <p>
 * When a sigint or sigquit is received, this function is called to close the file
 * descriptor and shut down the thread pool.
 * </p>
 *
 * @param signal - signal received.
 *
 * @return void.
 */
static void
server_close( int signal ) {
  if ( close( server.socket_fd ) < 0 ) {
    exit( EXIT_FAILURE );
  }

  // Shuts the thread pool down by broadcasting a signal which causes the
  // threads to leave the loop since the server flag is turned off.
  server.flags &= ~SERVER_ACTIVE;

  // Free the clients that are still in the list. Note that if the server
  // is ended with an interrupt and there are still users in the list, it leaks.
  // This also doesn't free the thread pool.
  client_list_destroy( &server.client_list );
  task_queue_destroy( &server.task_queue );
}
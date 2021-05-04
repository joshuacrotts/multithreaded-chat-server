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

#include "client.h"
#include "server.h"
#include "task_queue.h"
#include "tcpdefs.h"

#define DEBUG 1

static void  server_thread_pool_init( void );
static void  server_close_handle_init( void );
static void  server_listen( void );
static void  server_close( int );
static void *server_thread_exec( void * );
static int   server_create_listener( char *service );

/**
 *
 */
void
server_init( void ) {
  server.socket_fd = server_create_listener( SERVER_PORT );
  if ( server.socket_fd < 0 ) {
    perror( "DEBUG: Could not initialize socket file descriptor.\n" );
    exit( EXIT_FAILURE );
  }

  printf( "SERVER: Chat server starting...\n" );
  server.flags = SERVER_ACTIVE;

  memset( &server.start_time, 0, sizeof server.start_time );
  clock_gettime( CLOCK_MONOTONIC_RAW, &server.start_time );

  // Initialize the different data structures.
  client_list_create( &server.client_list );
  task_queue_create( &server.task_queue );
  server_thread_pool_init();
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
    perror( "DEBUG: Could not create socket listener.\n" );
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
    perror( "DEBUG: Could not bind name and address to socket.\n" );
    close( socket_fd );
    return -1;
  }

  // Lastly, set up the listener.
  int listen_return_val = listen( socket_fd, 128 );
  if ( listen_return_val < 0 ) {
    perror( "DEBUG: Could not initialize socket listener.\n" );
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
 * Initializes the thread pool.
 *
 * <p>
 * The thread pool is instantiated inside the server struct alongside a mutex
 * and condition variable.
 * </p>
 *
 * @param void.
 *
 * @return void.
 */
static void
server_thread_pool_init( void ) {
  for ( int i = 0; i < NUM_THREADS; i++ ) {
    pthread_create( &server.thread_pool[i], NULL, server_thread_exec, NULL );
  }
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
  pthread_cond_broadcast( &server.task_queue.cond );
  printf( "SERVER: Waking up all threads in the pool...\n" );
  for ( int i = 0; i < NUM_THREADS; i++ ) {
    pthread_join( server.thread_pool[i], NULL );
  }

  // Free the clients that are still in the list. Note that if the server
  // is ended with an interrupt and there are still users in the list, it leaks.
  // This also doesn't free the thread pool.
  client_list_destroy( &server.client_list );
  task_queue_destroy( &server.task_queue );
}

/**
 * Thread pool loop.
 *
 * <p>
 * While the server is active, a thread will wait until a task becomes available. Once
 * one is, a signal is sent to the condition variable and the thread wakes up to perform
 * the task. A NULL check exists because once the server "shuts down", there are no tasks
 * to perform, and therefore gettask will return NULL. This is in place to allow for
 * join-ing the threads at the end (not really necessary but still nice to have).
 * </p>
 *
 * @param void * args (should be null).
 *
 * @return void *.
 */
static void *
server_thread_exec( void *args ) {
  while ( server.flags & SERVER_ACTIVE ) {
    struct task_s *task = task_queue_gettask( &server.task_queue );
    if ( task != NULL ) {
      client_process_task( task );
      free( task );
    }
  }

  return NULL;
}

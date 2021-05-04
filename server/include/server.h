#ifndef SERVER_H
#define SERVER_H

#include <time.h>

#include "client_list.h"
#include "task_queue.h"

#define NUM_THREADS 32

#define SERVER_ACTIVE 0x00000001

/**
 *
 */
typedef struct server_t {
  pthread_t       thread_pool[NUM_THREADS]; /* */
  task_queue_t    task_queue;               /* */
  client_list_t   client_list;              /* */
  struct timespec start_time;               /* */
  int             socket_fd;                /* */
  int             flags;                    /* */
} server_t;

extern server_t server;

extern void server_init( void );

#endif // SERVER_H

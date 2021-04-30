#ifndef CLIENT_LIST_H
#define CLIENT_LIST_H

#include <pthread.h>

#include "client.h"

/**
 *
 */
typedef struct client_list_t {
  pthread_mutex_t       mutex; /* */
  struct client_node_s *head;  /* */
  struct client_node_s *tail;  /* */
} client_list_t;

/**
 *
 */
struct client_node_s {
  struct client_s *     client; /* */
  struct client_node_s *prev;   /* */
  struct client_node_s *next;   /* */
};

extern void client_list_create( client_list_t *client_list );
extern void client_list_destroy( client_list_t *client_list );
extern int  client_list_add( client_list_t *client_list, struct client_s *client );
extern int  client_list_remove( client_list_t *client_list, struct client_s *client );
extern int  client_list_search( client_list_t *client_list, struct client_s *client );

#endif // CLIENT_LIST_H

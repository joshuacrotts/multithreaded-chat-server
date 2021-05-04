#include <stdlib.h>

#include "client_list.h"

/**
 *
 * @param
 *
 * @return
 */
void
client_list_create( client_list_t *client_list ) {
  if ( pthread_mutex_init( &client_list->mutex, NULL ) < 0 ) {
    perror( "Could not initialize the mutex for client list.\n" );
    exit( EXIT_FAILURE );
  }

  client_list->head = NULL;
  client_list->tail = NULL;
}

/**
 *
 * @param
 * @param
 *
 * @return
 */
void
client_list_add( client_list_t *client_list, struct client_s *client ) {
  struct client_node_s *client_node = s_calloc( 1, sizeof( struct client_node_s ) );
  client_node->client = client;
  client_node->next   = NULL;
  client_node->prev   = NULL;

  pthread_mutex_lock( &client_list->mutex );
  // Add the client to the list.
  if ( client_list->head == NULL ) {
    client_list->tail = client_list->head = client_node;
  } else {
    client_node->prev       = client_list->tail;
    client_list->tail->next = client_node;
    client_list->tail       = client_node;
  }

  pthread_mutex_unlock( &client_list->mutex );
}

/**
 *
 * @param
 * @param
 *
 * @return
 */
int
client_list_remove( client_list_t *client_list, struct client_s *client ) {
  pthread_mutex_lock( &client_list->mutex );
  struct client_node_s *curr;
  int                   i = 0;
  for ( curr = client_list->head; curr != NULL; curr = curr->next, i++ ) {
    if ( curr->client == client ) {
      if ( curr->prev == NULL ) {
        // If the node is the head then it won't have a prev ptr.
        client_list->head = client_list->head->next;
        // If we just removed the last element in the list then
        // we can't reassign prev.
        if ( client_list->head != NULL ) {
          client_list->head->prev = NULL;
        }
      } else if ( curr->next == NULL ) {
        // If the node is the tail, then the next ptr is null.
        client_list->tail       = client_list->tail->prev;
        client_list->tail->next = NULL;
      } else {
        // Otherwise, set the previous's next link to curr's next, and
        // set the next's prev to curr.prev.
        curr->prev->next = curr->next;
        curr->next->prev = curr->prev;
      }

      // Destroy the client AND the backing LL node.
      client_destroy( curr->client );
      free( curr );
      break;
    }
  }

  pthread_mutex_unlock( &client_list->mutex );
  return curr != NULL ? i : -1;
}

/**
 *
 *
 * @param
 * @param
 *
 * @return
 */
int
client_list_search( client_list_t *client_list, struct client_s *client ) {
  struct client_node_s *curr;
  int                   i = 0;
  for ( curr = client_list->head; client != NULL; curr = curr->next, i++ ) {
    if ( curr->client == client ) {
      return i;
    }
  }
  return -1;
}

/**
 *
 * @param
 *
 * @return
 */
void
client_list_destroy( client_list_t *client_list ) {
  pthread_mutex_lock( &client_list->mutex );
  struct client_node_s *curr = client_list->head;
  struct client_node_s *next = curr;
  while ( next != NULL ) {
    next = curr->next;
    // Destroy the client associated with this node.
    client_destroy( curr->client );
    // Free the client_node_s itself.
    free( curr );
  }

  pthread_mutex_unlock( &client_list->mutex );
}

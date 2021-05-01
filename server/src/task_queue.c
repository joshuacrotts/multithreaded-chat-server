#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "server.h"
#include "task_queue.h"

/**
 * Initializes a queue (it starts empty).
 *
 * @param task_queue_t * pointer to queue object to initialize.
 *
 * @return void
 */
void
task_queue_create( task_queue_t *q ) {
  pthread_mutex_init( &q->mutex, NULL );
  pthread_cond_init( &q->cond, NULL );
  pthread_mutex_lock( &q->mutex );
  q->head = q->tail = NULL;
  pthread_mutex_unlock( &q->mutex );
}

/**
 * Enqueues a task structure to the queue. This should be
 * dynamically allocated before passing it to the enqueue
 * function.
 *
 * @param task_queue_t * pointer to queue object.
 * @param struct task_s * pointer to dynamically-allocated task structure.
 *
 * @return void.
 */
void
task_queue_enqueue( task_queue_t *q, struct task_s *task ) {
  // Initialize the node for the list and store the task in it.
  struct task_queue_node_s *node = calloc( 1, sizeof( struct task_queue_node_s ) );
  if ( node == NULL ) {
    printf( "SERVER: Could not allocate memory for node in task queue.\n" );
    exit( EXIT_FAILURE );
  }

  node->task = task;
  node->next = NULL;

  pthread_mutex_lock( &q->mutex );
  if ( q->head == NULL ) {
    q->head = node;
  } else {
    q->tail->next = node;
  }
  q->tail = node;

  // Tell the queue to wake up the condition variable.
  pthread_cond_signal( &q->cond );
  pthread_mutex_unlock( &q->mutex );
}

/**
 * Return the task_s struct at the front of the queue. Returns NULL if the
 * queue is empty.
 *
 * @param task_queue_t * pointer to queue object.
 *
 * @return pointer to task_s struct at the front of the queue.
 */
struct task_s *
task_queue_front( task_queue_t *q ) {
  if ( q->head == NULL ) {
    return NULL;
  }

  struct task_s *front = NULL;
  pthread_mutex_lock( &q->mutex );
  front = q->head->task;
  pthread_mutex_unlock( &q->mutex );
  return front;
}

/**
 * Remove the task structure at the front of the queue. Calls with an empty
 * queue will be ignored.
 *
 * @param task_queue_t * pointer to queue object.
 *
 * @return void.
 */
void
task_queue_dequeue( task_queue_t *q ) {
  pthread_mutex_lock( &q->mutex );

  struct task_queue_node_s *curr = q->head;
  if ( curr == NULL ) {
    q->tail = NULL;
  } else {
    q->head = q->head->next;
    free( curr->task );
    free( curr );
  }

  pthread_mutex_unlock( &q->mutex );
}

/**
 * Print out the task structures in the queue, with commas separating them. If
 * the queue is empty, it just prints "EMPTY".
 *
 * @param task_queue_t * pointer to queue object.
 *
 * @return void.
 */
void
task_queue_print( task_queue_t *q ) {
  pthread_mutex_lock( &q->mutex );

  struct task_queue_node_s *curr = q->head;
  if ( curr == NULL ) {
    printf( "EMPTY\n" );
  }

  while ( curr != NULL ) {
    struct task_s *t = curr->task;
    printf( "Data: %s\tTask Type: %d\tSender: %p\tReceiver: %s", t->data, t->task_type, t->sender,
            t->receiver );
    curr = curr->next;
    if ( curr != NULL )
      printf( ", " );
  }
  printf( "\n" );

  pthread_mutex_unlock( &q->mutex );
}

/**
 * Returns the task at the front of the queue. We wait on the condition variable set
 * by the queue, and until a task is enqueued (and the signal is received), we block
 * here.
 *
 * @param task_queue_t * pointer to queue object.
 *
 * @return task_s * ptr to task.
 */
struct task_s *
task_queue_gettask( task_queue_t *q ) {
  pthread_mutex_lock( &q->mutex );
  struct task_queue_node_s *f = NULL;
  while ( ( f = q->head ) == NULL && ( server.flags & SERVER_ACTIVE ) ) {
    pthread_cond_wait( &q->cond, &q->mutex );
  }

  // If the server is no longer active, just bail out.
  if ( ( server.flags & SERVER_ACTIVE ) == 0) {
    pthread_mutex_unlock( &q->mutex );
    return NULL;
  }

  // Dequeue the first element. We can't call dequeue since it's already atomic
  // and requires the lock.
  struct task_s *task = q->head->task;
  q->head             = q->head->next;
  if ( q->head == NULL ) {
    q->head = NULL;
  }

  free( f );
  pthread_mutex_unlock( &q->mutex );
  return task;
}

/**
 * Destroys a queue - frees up all resources associated with the queue.
 *
 * @param task_queue_t * pointer to queue object.
 *
 * @return void.
 */
void
task_queue_destroy( task_queue_t *q ) {
  pthread_mutex_lock( &q->mutex );

  while ( q->head != NULL ) {
    struct task_queue_node_s *curr = q->head;
    q->head                        = q->head->next;
    free( curr->task );
    free( curr );
  }

  pthread_mutex_unlock( &q->mutex );
}

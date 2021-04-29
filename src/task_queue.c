#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "task_queue.h"

/**
 *
 * @param
 *
 * @return
 */
void
task_queue_create( task_queue_t *queue ) {
  queue = malloc( sizeof( task_queue_t ) );
  if ( queue == NULL ) {
    fprintf( stderr, "Could not allocate memory for task queue.\n" );
    exit( EXIT_FAILURE );
  }

  pthread_mutex_init( &queue->mutex, NULL );
  pthread_cond_init( &queue->cond, NULL );
  queue->head = NULL;
  queue->tail = NULL;
}

/**
 *
 * @param
 * @param
 *
 * @return
 */
void
task_queue_enqueue( task_queue_t *queue, struct task_s *task ) {
  pthread_mutex_lock( &queue->mutex );

  pthread_cond_signal( &queue->cond );
  pthread_mutex_unlock( &queue->mutex );
}

/**
 *
 * @param
 *
 * @return
 */
void
task_queue_dequeue( task_queue_t *queue ) {
  pthread_mutex_lock( &queue->mutex );

  pthread_mutex_unlock( &queue->mutex );
}

/**
 *
 * @param
 *
 * @return
 */
struct task_s *
task_queue_front( task_queue_t *queue ) {
  return NULL;
}

/**
 *
 * @param
 *
 * @return
 */
void
task_queue_destroy( task_queue_t *queue ) {
  pthread_mutex_lock( &queue->mutex );
  struct task_queue_node_s *curr = queue->head;
  struct task_queue_node_s *next = curr;
  while ( next != NULL ) {
    next = curr->next;
    // We really only need to free the task itself and the receiver
    // because we malloc the string.
    free( curr->task->receiver );
    free( curr->task );
    free( curr );
  }

  pthread_mutex_unlock( &queue->mutex );
  pthread_mutex_destroy( &queue->mutex );
  pthread_cond_destroy( &queue->cond );
  free( queue );
}

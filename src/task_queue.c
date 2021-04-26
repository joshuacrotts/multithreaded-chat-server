#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

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
  if (queue == NULL) {
    fprintf(stderr, "Could not allocate memory for task queue.\n");
    exit(EXIT_FAILURE);
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
task_queue_enqueue( task_queue_t *queue, struct task_s *task ) {}

/**
 *
 * @param
 *
 * @return
 */
void
task_queue_dequeue( task_queue_t *queue ) {}

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
task_queue_destroy( task_queue_t *queue ) {}

#ifndef TASK_QUEUE_H
#define TASK_QUEUE_H

#include <pthread.h>

#include "utils.h"

/**
 *
 */
enum TASK_TYPE { TASK_NULL, TASK_MSG };
/**
 *
 */
typedef struct task_queue_t {
  pthread_mutex_t           mutex; /* */
  pthread_cond_t            cond;  /* */
  struct task_queue_node_s *head;  /* */
  struct task_queue_node_s *tail;  /* */
} task_queue_t;

/**
 *
 */
struct task_queue_node_s {
  struct task_s *           task; /* */
  struct task_queue_node_s *next; /* */
};

/**
 *
 */
struct task_s {
  enum TASK_TYPE   task_type; /* */
  enum MSG_TYPE    msg_type;  /* */
  struct client_s *sender;    /* */

  char receiver[1024]; /* */
  char data[1024];     /* */
};

extern void           task_queue_create( task_queue_t *queue );
extern void           task_queue_destroy( task_queue_t *queue );
extern void           task_queue_enqueue( task_queue_t *queue, struct task_s *task );
extern void           task_queue_dequeue( task_queue_t *queue );
extern struct task_s *task_queue_gettask( task_queue_t *queue );
extern struct task_s *task_queue_front( task_queue_t *queue );

#endif // TASK_QUEUE_H

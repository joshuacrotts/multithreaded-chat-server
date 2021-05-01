#ifndef CLIENT_H
#define CLIENT_H

#include <curses.h>
#include <pthread.h>

#define CLIENT_CONNECTED 0x00000001
#define CLIENT_LOGGED_IN 0x00000002

/**
 *
 */
struct text_attribute_s {
    int color;      /* */
    int style_flag; /* */
};

/**
 *
 */
struct cursor_s {
  int last_x;
  int last_y;
};

/**
 *
 */
struct client_s {
  char *name;                                   /* */
  int   comm_id;                                /* */
  int   flags;                                  /* */
  int   row;                                    /* */
  int   col;                                    /* */

  FILE *    read_fp;                            /* */
  FILE *    write_fp;                           /* */
  pthread_t pid;                                /* */

  struct text_attribute_s   text_attributes;    /* */
  struct client_s           *friends;           /* */
};

extern void client_create( const char *ip, int comm_id );
extern void client_destroy( struct client_s *client );

#endif // CLIENT_H

#include "backlog.h"

#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <errno.h>

static inline
void sem_v(sem_t* sem) {
  if(sem_post(sem) == -1) {
    perror("sem_post");
    pthread_exit(NULL);
  }
}

static inline
void sem_p(sem_t* sem) {
  if(sem_wait(sem) == -1) {
    perror("sem_wait");
    pthread_exit(NULL);
  }
}

static inline
void sem_create(sem_t* sem, unsigned int value) {
  if (sem_init(sem, 1, value) == -1) {
    perror("sem_init");
    pthread_exit(NULL);
  }
}

static inline
void sem_delete(sem_t* sem) {
  if (sem_destroy(sem) == -1) {
    perror("sem_destroy");
    pthread_exit(NULL);
  }
}

backlog_t* initBacklog(size_t backlogsize) {
  backlog_t* backlog =
    malloc((sizeof (backlog_t)) + backlogsize * (sizeof (packet_t)));
  backlog->front = 0;
  backlog->rear  = 0;
  backlog->length = backlogsize;
  sem_create(&backlog->mutex, 1);
  sem_create(&backlog->empty, backlog->length);
  sem_create(&backlog->full, 0);
  return backlog;
}

void freeBacklog(backlog_t* backlog) {
  sem_delete(&backlog->mutex);
  sem_delete(&backlog->empty);
  sem_delete(&backlog->full);
  free(backlog);
}

void pushBacklog(const packet_t* packet, backlog_t* backlog) {
  sem_p(&backlog->empty);
  sem_p(&backlog->mutex);

  backlog->queue[backlog->front] = *packet;
  backlog->front = (backlog->front + 1) % backlog->length;
  
  sem_v(&backlog->mutex);
  sem_v(&backlog->full);
}

packet_t popBacklog(backlog_t* backlog) {
  packet_t output;

  sem_p(&backlog->full);
  sem_p(&backlog->mutex);

  output = backlog->queue[backlog->rear];
  backlog->rear = (backlog->rear + 1) % backlog->length;

  sem_v(&backlog->mutex);
  sem_v(&backlog->empty);

  return output;
}

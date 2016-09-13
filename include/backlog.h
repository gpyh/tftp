#ifndef TFTP_BACKLOG_H
#define TFTP_BACKLOG_H

#include "common.h"

#include <semaphore.h>

typedef struct {
  size_t length;
  size_t rear;
  size_t front;
  sem_t empty;
  sem_t full;
  sem_t mutex;
  packet_t queue[];
} backlog_t;

backlog_t* initBacklog(size_t backlogsize);

void pushBacklog(const packet_t* packet, backlog_t* backlog);

packet_t popBacklog(backlog_t* backlog);

void freeBacklog(backlog_t* backlog);

#endif // TFTP_BACKLOG_H

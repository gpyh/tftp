#ifndef TFTP_DEBUG_H
#define TFTP_DEBUG_H

#include "common.h"

#ifndef NDEBUG

void debugPacket(const packet_t* packet);
void debugStatus(status_t status);

#else

#define debugPacket(ignore) ((void) 0)
#define debugStatus(ignore) ((void) 0)

#endif // NDEBUG

#endif // TFTP_DEBUG_H

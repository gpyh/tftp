#ifndef TFTP_DEBUG_H
#define TFTP_DEBUG_H

#ifndef NDEBUG

#include "common.h"

void debugPrintPacket(const packet_t* packet);

#else

#define debugPrintPacket(ignore) ((void) 0)

#endif // NDEBUG

#endif // TFTP_DEBUG_H

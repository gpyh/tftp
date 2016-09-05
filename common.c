#include "common.h"

#include <stdio.h>
#include <string.h>

static inline uint16_t collapseTwoBytes(char* buf) {
  return *((uint16_t*) buf);
} 

opcode_t readOpcode(char* buf) {
  uint16_t shortOpcode = ntohs(collapseTwoBytes(buf));
  if(shortOpcode >= MAX_OPCODE) {
    return UNDEFINED_OPCODE;
  }
  return ((opcode_t) shortOpcode);
}

void writeOpcode(opcode_t opcode, char* buf) {
  uint16_t opcodeNet = htons((uint16_t) opcode);
  memcpy(buf, &opcodeNet, 2);
}

errcode_t readErrcode(char* buf) {
  uint16_t shortErrcode = ntohs(collapseTwoBytes(buf));
  if(shortErrcode >= MAX_ERRCODE) {
    return UNDEFINED_ERRCODE;
  }
  return ((errcode_t) shortErrcode);
}

void writeErrcode(errcode_t errcode, char* buf) {
  uint16_t errcodeNet = htons((uint16_t) errcode);
  memcpy(buf, &errcodeNet, 2);
}

packet_t unmarshall(char* packetBuf) {
  packet_t packet;
  packet.opcode = readOpcode(packetBuf);
  switch(packet.opcode) {
    case RRQ:
    case WRQ:
      packet.content.rq.filename = &packetBuf[2];
      packet.content.rq.mode = &packetBuf[2 + strlen(&packetBuf[2]) + 1];
      break;
    case DATA:
      packet.content.data.block = collapseTwoBytes(&packetBuf[2]);
      packet.content.data.data = &packetBuf[4];
      break;
    case ACK:
      packet.content.ack.block = collapseTwoBytes(&packetBuf[2]);
      break;
    case ERROR:
      packet.content.error.errcode = readErrcode(&packetBuf[2]);
      packet.content.error.errmsg = &packetBuf[4];
      break;
    default:
      break;
  }
  return packet;
}


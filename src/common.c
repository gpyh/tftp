#include "common.h"

#include <AdresseInternet.h>
#include <SocketUDP.h>

#include <stdio.h>
#include <string.h>

#define MODE "octet"

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
  ((uint16_t*) buf)[0] = htons((uint16_t) opcode);
}

errcode_t readErrcode(char* buf) {
  uint16_t shortErrcode = ntohs(collapseTwoBytes(buf));
  if(shortErrcode >= MAX_ERRCODE) {
    return UNDEFINED_ERRCODE;
  }
  return ((errcode_t) shortErrcode);
}

void writeErrcode(errcode_t errcode, char* buf) {
  ((uint16_t*) buf)[0] = htons((uint16_t) errcode);
}

bool validate(char* buf) {
  
}

packet_t unmarshall(char* buf) {
  packet_t packet;
  packet.opcode = readOpcode(buf);
  switch(packet.opcode) {
    case RRQ:
    case WRQ:
      packet.content.rq.filename = &buf[2];
      packet.content.rq.mode = &buf[2 + strlen(&buf[2]) + 1];
      break;
    case DATA:
      packet.content.data.block = ntohs(collapseTwoBytes(&buf[2]));
      packet.content.data.data = &buf[4];
      break;
    case ACK:
      packet.content.ack.block = ntohs(collapseTwoBytes(&buf[2]));
      break;
    case ERROR:
      packet.content.error.errcode = readErrcode(&buf[2]);
      packet.content.error.errmsg = &buf[4];
      break;
    case UNDEFINED_OPCODE:
    case MAX_OPCODE:
      break;
  }
  return packet;
}

void marshall(char* buf, size_t* length, packet_t packet) {
  ((uint16_t*) buf)[0] = htons(packet.opcode);
  switch(packet.opcode) {
    case RRQ:
    case WRQ:
      *length = 2 + strlen(packet.content.rq.filename) + 1 + strlen(MODE) + 1;
      // Not a problem to use strcpy because we don't know the length anyway
      strcpy(&buf[2], packet.content.rq.filename);
      strcpy(&buf[2 + strlen(packet.content.rq.filename) + 1], MODE);
      break;
    case DATA:
      *length = 2 + 2 + packet.content.data.length;
      ((uint16_t*) buf)[1] = htons(packet.content.data.block);
      memcpy(&buf[4], packet.content.data.data, packet.content.data.length);
    case ACK:
      *length = 2 + 2;
      ((uint16_t*) buf)[1] = htons(packet.content.ack.block);
      break;
    case ERROR:
      *length = 2 + 2 + strlen(packet.content.error.errmsg) + 1;
      ((uint16_t*) buf)[1] = htons(packet.content.error.errcode);
      strcpy(&buf[4], packet.content.error.errmsg);
      break;
    case UNDEFINED_OPCODE:
    case MAX_OPCODE:
      break;
  }
}

#define TIMEOUT 10
#define TRY 10

void sendDataWaitAck(SocketUDP* socket, const AdresseInternet* dst,
                     const char* buf, size_t buflen) {
  // Le timeout : On relance
  // On reçoit pas un packet tftp : On relance
  // On reçoit pas un ACK
  // On reçoit un ACK avec un mauvais num de block
  writeToSocketUDP(socket, dst, buf, buflen);
  const char* ackBuf[4];
  recvFromSocketUDP(socket, ackBuf, 4, dst, TIMEOUT);
}


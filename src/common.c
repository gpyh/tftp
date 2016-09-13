#include "common.h"

#include <AdrInet.h>

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// Verification

int verifyRQ(const char* filename, const char* mode) {
  if(strcmp(mode, "netascii") == 0) {
    return UNIMPLEMENTED_MODE;
  }
  if(strcmp(mode, "mail") == 0) {
    return UNIMPLEMENTED_MODE;
  }
  if(strcmp(mode, "octet") != 0) {
    return UNKNOWN_MODE;
  }
  if(2 + strlen(filename) + 1 + strlen(mode) > PACKET_SIZE) {
    return FILENAME_TOO_LONG;
  }
  return SUCCESS;
}

int verifyRRQ(const char* filename, const char* mode) {
  int verif = verifyRQ(filename, mode);
  if(verif != SUCCESS) {
    return verif;
  }
  return SUCCESS;
}

int verifyWRQ(const char* filename, const char* mode) {
  int verif = verifyRQ(filename, mode);
  if(verif != SUCCESS) {
    return verif;
  }
  return SUCCESS;
}

int verifyDATA(uint16_t block, const char* data, size_t datalen) {
  (void)(block);
  (void)(data);
  if(2 + 2 + datalen > PACKET_SIZE) {
    return DATA_TOO_BIG;
  }
  return SUCCESS;
}

int verifyERROR(errcode_t errcode, const char* errmsg) {
  if(errcode <= MIN_ERRCODE || errcode >= MAX_ERRCODE) {
    return UNKNOWN_ERRCODE;
  }
  if(2 + 2 + strlen(errmsg) + 1 > PACKET_SIZE) {
    return ERRMSG_TOO_BIG;
  } 
  return SUCCESS;
}

// Unmarshalling

int unmarshallRRQ(packet_t* packet, const char* buf, size_t buflen) {
  if(buflen < 2 + 1 + 1 + 1 + 1) {
    return BUFFER_TOO_SMALL;
  }
  const char* filename = &buf[2];
  const char* mode = &buf[2 + strlen(filename) + 1];
  int status;
  if((status = verifyRRQ(filename, mode)) != SUCCESS) {
    return status;
  };
  createRRQ(packet, filename, mode);
  return SUCCESS;
}

int unmarshallWRQ(packet_t* packet, const char* buf, size_t buflen) {
  if(buflen < 2 + 1 + 1 + 1 + 1) {
    return BUFFER_TOO_SMALL;
  }
  const char* filename = &buf[2];
  const char* mode = &buf[2 + strlen(filename) + 1];
  int status;
  if((status = verifyWRQ(filename, mode)) != SUCCESS) {
    return status;
  }
  createWRQ(packet, filename, mode);
  return SUCCESS;
}

int unmarshallDATA(packet_t* packet, const char* buf, size_t buflen) {
  uint16_t block = ntohs(concatTwoBytes(&buf[2]));
  const char* data = &buf[2 + 2];
  size_t datalen = buflen - (2 + 2);
  int status;
  if((status = verifyDATA(block, data, datalen)) != SUCCESS) {
    return status;
  }
  createDATA(packet, block, data, datalen);
  return SUCCESS;
}

int unmarshallACK(packet_t* packet, const char* buf, size_t buflen) {
  (void)(buflen);
  packet->content.ack.block = ntohs(concatTwoBytes(&buf[2]));
  return SUCCESS;
}

int unmarshallERROR(packet_t* packet, const char* buf, size_t buflen) {
  if(buflen < 2 + 2 + 1) {
    return BUFFER_TOO_SMALL;
  }
  errcode_t errcode = readErrcode(&buf[2]);
  const char* errmsg = &buf[2 + 2];
  if(buf[buflen - 1] != 0) {
    return ERRMSG_TRUNCATED;
  }
  int status;
  if((status = verifyERROR(errcode, errmsg)) != SUCCESS) {
    return status;
  }
  createERROR(packet, errcode, errmsg);
  return SUCCESS;
}

int (*unmarshalls[6])(packet_t*, const char*, size_t) = {
  NULL,
  unmarshallRRQ,
  unmarshallWRQ,
  unmarshallDATA,
  unmarshallACK,
  unmarshallERROR };

int unmarshall(packet_t* packet, const char* buf, size_t buflen) {
  if(buflen < 4) {
    return BUFFER_TOO_SMALL;
  }
  opcode_t opcode = readOpcode(buf);
  if(opcode <= MIN_OPCODE || opcode >= MAX_OPCODE) {
    return UNKNOWN_OPCODE;
  }
  return unmarshalls[opcode](packet, buf, buflen);
}

// Marshalling

size_t marshallXRQ(char* buf, const packet_t* packet) {
  size_t buflen = 2 + strlen(packet->content.rq.filename) + 1 + strlen("octet") + 1;
  // Not a problem to use strcpy because we don't know the length anyway
  strcpy(&buf[2], packet->content.rq.filename);
  strcpy(&buf[2 + strlen(packet->content.rq.filename) + 1], "octet");
  return buflen;
}

size_t marshallRRQ(char* buf, const packet_t* packet) {
  ((uint16_t*) buf)[0] = htons(RRQ);
  return marshallXRQ(buf, packet);
}

size_t marshallWRQ(char* buf, const packet_t* packet) {
  ((uint16_t*) buf)[0] = htons(WRQ);
  return marshallXRQ(buf, packet);
}

size_t marshallDATA(char* buf, const packet_t* packet) {
  ((uint16_t*) buf)[0] = htons(DATA);
  size_t buflen = 2 + 2 + packet->content.data.datalen;
  ((uint16_t*) buf)[2 / 2] = htons(packet->content.data.block);
  memcpy(&buf[4], packet->content.data.data, packet->content.data.datalen);
  return buflen;
}

size_t marshallACK(char* buf, const packet_t* packet) {
  ((uint16_t*) buf)[0] = htons(ACK);
  size_t buflen = 2 + 2;
  ((uint16_t*) buf)[2 / 2] = htons(packet->content.ack.block);
  return buflen;
}

size_t marshallERROR(char* buf, const packet_t* packet) {
  ((uint16_t*) buf)[0] = htons(ERROR);
  size_t buflen = 2 + 2 + strlen(packet->content.error.errmsg) + 1;
  ((uint16_t*) buf)[2 / 2] = htons(packet->content.error.errcode);
  strcpy(&buf[2 + 2], packet->content.error.errmsg);
  return buflen;
}

size_t (*marshalls[6])(char*, const packet_t*) = {
  NULL,
  marshallRRQ,
  marshallWRQ,
  marshallDATA,
  marshallACK,
  marshallERROR };

size_t marshall(char* buf, const packet_t* packet) {
  return marshalls[packet->opcode](buf, packet);
}

// Communication

int sendPacket(const sudpSocket_t* socket, const AdrInet* dst,
               const packet_t* packet, size_t buflenMax) {
  char buf[buflenMax];
  size_t buflen = marshall(buf, packet);
  return sudpWriteToSocket(socket, dst, buf, buflen);
}

int waitPacket(packet_t* packet, const sudpSocket_t* socket,
               const AdrInet* connection, size_t buflenMax, int timeout) {
  char buf[buflenMax];
  int buflen = sudpRecvFromSocket(socket, buf, PACKET_SIZE, connection, timeout);
  if(buflen < 0) {
    return SOCKET_ERROR;
  }
  if(buflen == 0) {
    return TIMED_OUT;
  }
  int status = unmarshall(packet, buf, (size_t) buflen);
  if(status != SUCCESS) {
    return status;
  }
  return SUCCESS;
}

int sendAndWait(const connection_t* connection, const packet_t* packetOut,
                packet_t* packetIn, onWait_t callback) {
  // I am aware of the sacrosaint "goto considered harmful,
  // but in this case it makes the code much clearer
  unsigned int packetsSent = 0;
  send:{
    if(packetsSent == connection->attempts) {
      return 0;
    }
    if(sendPacket(connection->socket, connection->other, packetOut,
                  connection->packetSize) < 0) {
      return -1;
    }
    packetsSent++;
  }

  wait:{
    int status =
      waitPacket(packetIn, connection->socket, connection->self,
                 connection->packetSize, connection->timeout);
    switch(status) {
      case SUCCESS:{
        callbackAction_t action =
          callback(connection, packetOut, packetIn);
        switch(action) {
          case GO_THROUGH:
            return 0;
          case RESEND:
            goto send;
          case IGNORE:
            goto wait;
          case ABORT:
            return -1;
        }
      };
      case TIMED_OUT:
        goto send;
      default:
        return -1;
    }
  }

  return 1;
}

#define TIMEOUT 10
#define ATTEMPTS 10 
int sendError(const connection_t* connection,
              errcode_t errcode, const char* errmsg) {
  packet_t packet;
  createERROR(&packet, errcode, errmsg);
  return sendPacket(connection->socket, connection->other, &packet,
                    connection->packetSize);
}

/* callbackAction_t sRRQwDATA(const connection_t* connection, */
/*                            const packet_t* packetOut, */
/*                            const packet_t* packetIn){ */
/*   if(packetIn->opcode == DATA) { */
/*     if(packetIn->content.data.block != 1) { */
/*       sendError(connection->socket, connection->other, */
/*                 UNDEFINED_ERROR, "TODO"); */
/*       return ABORT; */
/*     } */
/*     return GO_THROUGH; */
/*   } */
/*   sendError(connection->socket, connection->other, UNDEFINED_ERROR, "TODO"); */
/*   return ABORT; */
/* } */

/* callbackAction_t sDATAwACK(const connection_t* connection, */
/*                            const packet_t* packetOut, */
/*                            const packet_t* packetIn){ */
/*   if(packetIn->opcode == ACK) { */
/*     if(packetIn->content.ack.block < packetOut->content.data.block) { */
/*       return IGNORE; */
/*     } */
/*     if(packetIn->content.ack.block == packetOut->content.data.block) { */
/*       return GO_THROUGH; */
/*     } */
/*   } */
/*   sendError(connection->socket, connection->other, UNDEFINED_ERROR, "TODO"); */
/*   return ABORT; */
/* } */

/* callbackAction_t sACKwDATA(const connection_t* connection, */
/*                            const packet_t* packetOut, */
/*                            const packet_t* packetIn){ */
/*   if(packetIn->opcode == DATA) { */
/*     if(packetIn->content.data.block <= packetOut->content.ack.block) { */
/*       return IGNORE; */
/*     } */
/*     if(packetIn->content.data.block == packetOut->content.ack.block + 1) { */
/*       return GO_THROUGH; */
/*     } */
/*   } */
/*   sendError(connection->socket, connection->other, UNDEFINED_ERROR, "TODO"); */
/*   return ABORT; */
/* } */


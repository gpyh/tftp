#include "common.h"

#include <AdresseInternet.h>
#include <sudp.h>

#include <stdio.h>
#include <string.h>

// Verification

int verifyRQ(const char* filename, char* mode) {
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

int verifyRRQ(const char* filename, char* mode) {
  int verif = verifyRQ(filename, mode);
  if(verif != SUCCESS) {
    return verif;
  }
  return SUCCESS;
}

int verifyWRQ(const char* filename, char* mode) {
  int verif = verifyRQ(filename, mode);
  if(verif != SUCCESS) {
    return verif;
  }
  return SUCCESS;
}

int verifyDATA(uint16_t block, char* data, size_t datalen) {
  if(block == 0) {
  }
  if(2 + 2 + datalen > PACKET_SIZE) {
    return DATA_TOO_BIG;
  }
  return SUCCESS;
}

int verifyERROR(errcode_t errcode, char* errmsg) {
  if(errcode <= MIN_ERRCODE || errcode >= MAX_ERRCODE) {
    return UNKNOWN_ERRCODE;
  }
  if(2 + 2 + strlen(errmsg) + 1 > PACKET_SIZE) {
    return ERRMSG_TOO_BIG;
  } 
  return SUCCESS;
}

// Unmarshalling

int unmarshallRRQ(packet_t* packet, char* buf, size_t buflen) {
  if(buflen < 2 + 1 +1) {
    return BUFFER_TOO_SMALL;
  }
  if(readOpcode(buf) != RRQ) {
    return WRONG_OPCODE;
  }
  char* filename = &buf[2];
  char* mode = &buf[2 + strlen(filename) + 1];
  int status;
  if((status = verifyRRQ(filename, mode)) != SUCCESS) {
    return status;
  };
  createRRQ(packet, filename, mode);
  return SUCCESS;
}

int unmarshallWRQ(packet_t* packet, char* buf, size_t buflen) {
  if(buflen < 2 + 1 +1) {
    return BUFFER_TOO_SMALL;
  }
  if(readOpcode(buf) != WRQ) {
    return WRONG_OPCODE;
  }
  char* filename = &buf[2];
  char* mode = &buf[2 + strlen(filename) + 1];
  int status;
  if((status = verifyWRQ(filename, mode)) != SUCCESS) {
    return status;
  }
  createWRQ(packet, filename, mode);
  return SUCCESS;
}

int unmarshallDATA(packet_t* packet, char* buf, size_t buflen) {
  if(buflen < 2 + 2 + 0) {
    return BUFFER_TOO_SMALL;
  }
  if(readOpcode(buf) != DATA) {
    return WRONG_OPCODE;
  }
  uint16_t block = ntohs(concatTwoBytes(&buf[2]));
  char* data = &buf[2 + 2];
  size_t datalen = buflen - (2 + 2);
  int status;
  if((status = verifyDATA(block, data, datalen)) != SUCCESS) {
    return status;
  }
  createDATA(packet, block, data, datalen);
  return SUCCESS;
}

int unmarshallACK(packet_t* packet, char* buf, size_t buflen) {
  if(buflen < 2 + 2) {
    return BUFFER_TOO_SMALL;
  }
  if(readOpcode(buf) != ACK) {
    return WRONG_OPCODE;
  }
  packet->content.ack.block = ntohs(concatTwoBytes(&buf[2]));
  return SUCCESS;
}

int unmarshallERROR(packet_t* packet, char* buf, size_t buflen) {
  if(buflen < 2 + 2 + 1) {
    return BUFFER_TOO_SMALL;
  }
  if(readOpcode(buf) != ERROR) {
    return WRONG_OPCODE;
  }
  errcode_t errcode = readErrcode(&buf[2]);
  char* errmsg = &buf[2 + 2];
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

int (*unmarshalls[6])(packet_t*, char*, size_t) = {
  NULL,
  unmarshallRRQ,
  unmarshallWRQ,
  unmarshallDATA,
  unmarshallACK,
  unmarshallERROR };

int unmarshall(packet_t* packet, opcode_t opcode, char* buf, size_t buflen) {
  return unmarshalls[opcode](packet, buf, buflen);
}

// Marshalling

size_t marshallXRQ(char* buf, packet_t* packet) {
  size_t buflen = 2 + strlen(packet->content.rq.filename) + 1 + strlen("octet") + 1;
  // Not a problem to use strcpy because we don't know the length anyway
  strcpy(&buf[2], packet->content.rq.filename);
  strcpy(&buf[2 + strlen(packet->content.rq.filename) + 1], "octet");
  return buflen;
}


size_t marshallRRQ(char* buf, packet_t* packet) {
  ((uint16_t*) buf)[0] = htons(RRQ);
  return marshallXRQ(buf, packet);
}


size_t marshallWRQ(char* buf, packet_t* packet) {
  ((uint16_t*) buf)[0] = htons(WRQ);
  return marshallXRQ(buf, packet);
}

size_t marshallDATA(char* buf, packet_t* packet) {
  ((uint16_t*) buf)[0] = htons(DATA);
  size_t buflen = 2 + 2 + packet->content.data.datalen;
  ((uint16_t*) buf)[2 / 2] = htons(packet->content.data.block);
  memcpy(&buf[4], packet->content.data.data, packet->content.data.datalen);
  return buflen;
}

size_t marshallACK(char* buf, packet_t* packet) {
  ((uint16_t*) buf)[0] = htons(ACK);
  size_t buflen = 2 + 2;
  ((uint16_t*) buf)[2 / 2] = htons(packet->content.ack.block);
  return buflen;
}

size_t marshallERROR(char* buf, packet_t* packet) {
  ((uint16_t*) buf)[0] = htons(ERROR);
  size_t buflen = 2 + 2 + strlen(packet->content.error.errmsg) + 1;
  ((uint16_t*) buf)[2 / 2] = htons(packet->content.error.errcode);
  strcpy(&buf[2 + 2], packet->content.error.errmsg);
  return buflen;
}

size_t (*marshalls[6])(char*, packet_t*) = {
  NULL,
  marshallRRQ,
  marshallWRQ,
  marshallDATA,
  marshallACK,
  marshallERROR };


size_t marshall(char* buf, packet_t* packet) {
  return marshalls[packet->opcode](buf, packet);
}

// Communication

int sendPacket(sudpSocket_t* socket, const AdresseInternet* dst,
               packet_t* packet) {
  char* buf;
  size_t buflen = marshall(buf, packet);
  return sudpWriteToSocket(socket, dst, buf, buflen);
}

int waitPacketWithTimeout(packet_t* packet, opcode_t opcode,
                          sudpSocket_t* socket, AdresseInternet* connection,
                          int timeout) {
  char* buf;
  int buflen = sudpRecvFromSocket(socket, buf, PACKET_SIZE, connection, timeout);
  if(buflen < 0) {
    return SOCKET_ERROR;
  }
  if(buflen == 0) {
    return TIMED_OUT;
  }
  int status = unmarshall(packet, opcode, buf, (size_t) buflen);
  if(status != SUCCESS) {
    return status;
  }
  return SUCCESS;
}

#define TIMEOUT 10
#define TRY 10

int sendRRQwaitDATA(sudpSocket_t* socket, const AdresseInternet* dst,
                    const char* filename, AdresseInternet* connection,
                    packet_t* data) {
  packet_t* rrq;
  createRRQ(rrq, filename, "octet");
  if(sendPacket(socket, dst, rrq) < 0) {
    return -1;
  }
  int status = waitPacketWithTimeout(data, DATA, socket, connection, TIMEOUT);
  if(status == TIMED_OUT) {
    return 0;
  }
  if(status != SUCCESS && data->content.data.block != 1) {
    packet_t* error;
    createERROR(error, UNDEFINED_ERROR, "TODO");
    // No check because we don't care if sending the error fails
    sendPacket(socket, connection, error);
    return -1;
  }
  return 1;
}

/* void sendDataWaitAck(sudpSocket_t* socket, const AdresseInternet* dst, */
/*                      const char* fichier, AdresseInternet* connexion, */
/*                      char* reponse, size_t replength) { */
/*   // Le timeout : On relance */
/*   // On reçoit pas un packet tftp : On relance */
/*   // On reçoit pas un ACK */
/*   // On reçoit un ACK avec un mauvais num de block */
/* #ifndef NDEBUG */
/*   if(verifyDATA(1, fichier, "octet") != SUCCESS) { */
/*     printf("Oops"); */
/*     exit(EXIT_FAILURE); */
/*   } */
/* #endif */
/*   packet_t packet; */
/*   createDATA(fichier, "octet"); */
/*   writeTosudpSocket_t(socket, dst, buf, buflen); */
/*   char ackBuf[4]; */
/*   recvFromsudpSocket_t(socket, ackBuf, 4, dst, TIMEOUT); */
/* } */


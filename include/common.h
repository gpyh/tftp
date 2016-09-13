#ifndef TFTP_COMMON_H
#define TFTP_COMMON_H

#include <AdrInet.h>
#include <sudp.h>

#include <arpa/inet.h>
#include <stdint.h>

#define DEFAULT_PORT 6969
#define MAX_BLOCKSIZE 65464

static inline
uint16_t concatTwoBytes(const char* buf) {
  return *((uint16_t*) buf);
} 

typedef enum {
  MIN_OPCODE = 0,

  RRQ,
  WRQ,
  DATA,
  ACK,
  ERROR,

  MAX_OPCODE
} opcode_t;

static inline
opcode_t readOpcode(const char* buf) {
  uint16_t shortOpcode = ntohs(concatTwoBytes(buf));
  if(shortOpcode >= MAX_OPCODE) {
    return MIN_OPCODE;
  }
  return ((opcode_t) shortOpcode);
}

static inline
void writeOpcode(opcode_t opcode, char* buf) {
  ((uint16_t*) buf)[0] = htons((uint16_t) opcode);
}

typedef enum {
  MIN_ERRCODE = -1,

  UNDEFINED_ERROR,
  FILENOTFOUND,
  ACCVIOL,
  DISKERR,
  ILLEGALOP,
  NOID,
  FILEEXISTS,
  NOUSER,

  MAX_ERRCODE
} errcode_t;

static inline
errcode_t readErrcode(const char* buf) {
  uint16_t shortErrcode = ntohs(concatTwoBytes(buf));
  if(shortErrcode >= MAX_ERRCODE) {
    return MIN_ERRCODE;
  }
  return ((errcode_t) shortErrcode);
}

static inline
void writeErrcode(errcode_t errcode, char* buf) {
  ((uint16_t*) buf)[0] = htons((uint16_t) errcode);
}

typedef struct {
  opcode_t opcode;
  union {
    struct {
      const char* filename;
      const char* mode;
    } rq;

    struct {
      uint16_t block;
      size_t datalen;
      const char* data;
    } data;

    struct {
      uint16_t block;
    } ack;

    struct {
      errcode_t errcode;
      const char* errmsg;
    } error;

  } content;
} packet_t;

static inline
void createRRQ(packet_t* packet, const char* filename, const char* mode) {
  packet->opcode = RRQ;
  packet->content.rq.filename = filename;
  packet->content.rq.mode = mode;
}

static inline
void createWRQ(packet_t* packet, const char* filename, const char* mode) {
  packet->opcode = WRQ;
  packet->content.rq.filename = filename;
  packet->content.rq.mode = mode;
}

static inline
void createACK(packet_t* packet, uint16_t block) {
  packet->opcode = ACK;
  packet->content.ack.block = block;
}

static inline
void createDATA(packet_t* packet, uint16_t block,
                const char* data, size_t datalen) {
  packet->opcode = DATA;
  packet->content.data.block = block;
  packet->content.data.datalen = datalen;
  packet->content.data.data = data;
}

static inline
void createERROR(packet_t* packet, errcode_t errcode, const char* errmsg) {
  packet->opcode = ERROR;
  packet->content.error.errcode = errcode;
  packet->content.error.errmsg = errmsg;
}

#define PACKET_SIZE 512

enum {
  UNIMPLEMENTED_MODE,
  UNKNOWN_MODE,
  UNKNOWN_ERRCODE,
  UNKNOWN_OPCODE,
  BUFFER_TOO_BIG,
  BUFFER_TOO_SMALL,
  FILENAME_TOO_LONG,
  DATA_TOO_BIG,
  ERRMSG_TOO_BIG,
  ERRMSG_TRUNCATED,
  MODE_TRUNCATED,
  INVALID_BLOCK,
  WRONG_OPCODE,
  ERROR_RECEIVED,
  SOCKET_ERROR,
  TIMED_OUT,
  SUCCESS = 0
};

int verifyRRQ(const char* filename, const char* mode);
int verifyWRQ(const char* filename, const char* mode);
// There's no need to verify ACK
// int verifyACK(uint16_t block);
int verifyDATA(uint16_t block, const char* data, size_t datalen);
int verifyERROR(errcode_t errcode, const char* errmsg);

typedef enum {
  GO_THROUGH,
  RESEND,
  IGNORE,
  ABORT
} callbackAction_t;

typedef struct {
  sudpSocket_t* socket;
  AdrInet* self;
  AdrInet* other;
  unsigned int timeout;
  unsigned int attempts;
  size_t packetSize;
} connection_t;

typedef callbackAction_t (*onWait_t)(const connection_t* connection,
                                     const packet_t* packetOut,
                                     const packet_t* packetIn);

int sendPacket(const sudpSocket_t* socket, const AdrInet* dst,
               const packet_t* packet, size_t buflenMax);

int waitPacket(packet_t* packet, const sudpSocket_t* socket,
               const AdrInet* connection, size_t buflenMax, int timeout);

int sendAndWait(const connection_t* connection, const packet_t* packetOut,
                packet_t* packetIn, onWait_t callback);

#endif // TFTP_COMMON_H

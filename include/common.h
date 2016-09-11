
#include <AdrInet.h>
#include <sudp.h>

#include <arpa/inet.h>
#include <stdint.h>

static inline
uint16_t concatTwoBytes(char* buf) {
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
opcode_t readOpcode(char* buf) {
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
  ILLEGAL,
  NOID,
  FILEEXISTS,
  NOUSER,
  MAX_ERRCODE
} errcode_t;

static inline
errcode_t readErrcode(char* buf) {
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
      char* mode;
    } rq;

    struct {
      uint16_t block;
      size_t datalen;
      char* data;
    } data;

    struct {
      uint16_t block;
    } ack;

    struct {
      errcode_t errcode;
      char* errmsg;
    } error;

  } content;
} packet_t;

static inline
void createRRQ(packet_t* packet, const char* filename, char* mode) {
  packet->opcode = RRQ;
  packet->content.rq.filename = filename;
  packet->content.rq.mode = mode;
}

static inline
void createWRQ(packet_t* packet, const char* filename, char* mode) {
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
void createDATA(packet_t* packet, uint16_t block, char* data,
                    size_t datalen) {
  packet->opcode = DATA;
  packet->content.data.block = block;
  packet->content.data.datalen = datalen;
  packet->content.data.data = data;
}

static inline
void createERROR(packet_t* packet, errcode_t errcode, char* errmsg) {
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
  SOCKET_ERROR,
  TIMED_OUT,
  SUCCESS = 0
};

int verifyRRQ(const char* filename, char* mode);
int verifyWRQ(const char* filename, char* mode);
// There's no need to verify ACK
// int verifyACK(uint16_t block);
int verifyDATA(uint16_t block, char* data, size_t datalen);
int verifyERROR(errcode_t errcode, char* errmsg);

/* status_t unmarshall(packet_t* packet, char* buf, size_t buflen); */

typedef enum {
  OK,
  RESEND,
  IGNORE,
  ABORT
} checkStatus_t;

typedef checkStatus_t (*checkFunction_t)(packet_t*);

int sendAndWait(sudpSocket_t* socket,
                AdrInet* dst, packet_t* packetOut,
                AdrInet* connection, 
                packet_t* packetIn, opcode_t opcodeIn,
                checkFunction_t checkPacketIn,
                unsigned int timeout, unsigned int attempts);


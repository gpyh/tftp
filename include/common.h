#include <arpa/inet.h>
#include <stdint.h>

typedef enum {
  UNDEFINED_OPCODE,
  RRQ,
  WRQ,
  DATA,
  ACK,
  ERROR,
  MAX_OPCODE
} opcode_t;

typedef enum {
  UNDEFINED_ERRCODE,
  FILENOTFOUND,
  ACCVIOL,
  DISKERR,
  ILLEGAL,
  NOID,
  FILEEXISTS,
  NOUSER,
  MAX_ERRCODE
} errcode_t;

struct contentRq {
  char* filename;
  char* mode;
};

struct contentData {
  uint16_t block;
  size_t length;
  char* data;
};

struct contentAck {
  uint16_t block;
};

struct contentError {
  errcode_t errcode;
  char* errmsg;
};

typedef struct {
  opcode_t opcode;
  union {
    struct contentRq rq;
    struct contentData data;
    struct contentAck ack;
    struct contentError error;
  } content;
} packet_t;

opcode_t readOpcode(char* buf);
void writeOpcode(opcode_t opcode, char* buf);

errcode_t readErrcode(char* buf);
void writeErrcode(errcode_t errcode, char* buf);

packet_t unmarshall(char* packetBuf);


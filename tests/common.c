#include "common.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#define abort(ignore) ((void) 0)

void readOpcodeSimple() {
  char buf[2] = { 0, 2 };
  opcode_t opcode = readOpcode(buf);
  opcode_t opcodeMustBe = 2;

  if(opcode != opcodeMustBe) {
    printf("Opcode is %d but should be %d\n", opcode, opcodeMustBe);
    abort();
  } else {
    printf("readOpcodeSimple passed!\n");
  }
}

void writeOpcodeSimple() {
  opcode_t opcode = 2;
  char buf[2];
  writeOpcode(opcode, buf);
  opcode_t opcodePacket = ((opcode_t)buf[0]*256 + (opcode_t)buf[1]);

  if(opcode != opcodePacket) {
    printf("Opcode is %d but buf has %d\n", opcode, opcodePacket);
    abort();
  } else {
    printf("writeOpcodeSimple passed!\n");
  }
}

void writeReadOpcodeIdentity() {
  opcode_t opcode = 2;
  char buf[2];
  writeOpcode(opcode, buf);
  opcode_t opcodeOut = readOpcode(buf);
  
  if(opcode != opcodeOut) {
    printf("Opcode is %d but should be %d\n", opcodeOut, opcode);
    abort();
  } else {
    printf("writeReadOpcodeIdentity passed!\n");
  }
  
}

void readWriteOpcodeIdentity() {
  char buf[2] = { 0, 2 };
  char bufOut[2];
  opcode_t opcode = readOpcode(buf);
  writeOpcode(opcode, bufOut);
  
  if(memcmp(buf, bufOut, 2) != 0) {
    printf(
        "Original opcode has bytes (%hhd, %hhd)"
        " but output opcode has bytes (%hhd, %hhd).\n",
        buf[0], buf[1], bufOut[0], bufOut[1]);
    abort();
  } else {
    printf("readWriteOpcodeIdentity passed!\n");
  }
}

void readErrcodeSimple() {
  char buf[2] = { 0, 2 };
  errcode_t errcode;
  errcode = readErrcode(buf);
  errcode_t errcodeMustBe = 2;

  if(errcode != errcodeMustBe) {
    printf("Errcode is %d but should be %d\n", errcode, errcodeMustBe);
    abort();
  } else {
    printf("readErrcodeSimple passed!\n");
  }
}

void writeErrcodeSimple() {
  errcode_t errcode = 2;
  char buf[2];
  writeErrcode(errcode, buf);

  errcode_t errcodePacket = ((errcode_t)buf[0]*256 + (errcode_t)buf[1]);

  if(errcode != errcodePacket) {
    printf("Errcode is %d but buf has %d\n", errcode, errcodePacket);
    abort();
  } else {
    printf("writeErrcodeSimple passed!\n");
  }
}

void writeReadErrcodeIdentity() {
  errcode_t errcode = 2;
  char buf[2];
  writeErrcode(errcode, buf);
  errcode_t errcodeOut = readErrcode(buf);
  
  if(errcode != errcodeOut) {
    printf("Errcode is %d but should be %d\n", errcodeOut, errcode);
    abort();
  } else {
    printf("writeReadErrcodeIdentity passed!\n");
  }
  
}

void readWriteErrcodeIdentity() {
  char buf[2] = { 0, 2 };
  char bufOut[2];
  errcode_t errcode = readErrcode(buf);
  writeErrcode(errcode, bufOut);
  
  if(memcmp(buf, bufOut, 2) != 0) {
    printf(
        "Original errcode has bytes (%hhd, %hhd)"
        " but output errcode has bytes (%hhd, %hhd).\n",
        buf[0], buf[1], bufOut[0], bufOut[1]);
    abort();
  } else {
    printf("readWriteErrcodeIdentity passed!\n");
  }
}

void unmarshallRRQ() {
  const char* filename = "test";
  const char* mode = "octet";
  char buf[2 + strlen(filename) + 1 + strlen(mode) + 1];
  buf[0] = 0;
  buf[1] = 1;
  strncpy(&buf[2], filename, strlen(filename));
  buf[2 + strlen(filename)] = 0;
  strncpy(&buf[2 + strlen(filename) + 1], mode, strlen(mode));
  buf[2 + strlen(filename) + 1 + strlen(mode)] = 0;
  packet_t packet = unmarshall(buf);

  if(packet.opcode != RRQ) {
    printf("Opcode is %d but should be %d\n", packet.opcode, RRQ);
    abort();
  }
  if(strncmp(packet.content.rq.filename, filename, strlen(filename)) != 0) {
    printf("Filename is %s but should be %s\n",
        packet.content.rq.filename,
        filename);
    abort();
  }
  if(strncmp(packet.content.rq.mode, mode, strlen(mode)) != 0) {
    printf("Mode is %s but should be %s\n",
        packet.content.rq.mode,
        mode);
    abort();
  }
}

void unmarshallWRQ() {
  const char* filename = "test";
  const char* mode = "octet";
  char buf[2 + strlen(filename) + 1 + strlen(mode) + 1];
  buf[0] = 0;
  buf[1] = 2;
  strncpy(&buf[2], filename, strlen(filename));
  buf[2 + strlen(filename)] = 0;
  strncpy(&buf[2 + strlen(filename) + 1], mode, strlen(mode));
  buf[2 + strlen(filename) + 1 + strlen(mode)] = 0;
  packet_t packet = unmarshall(buf);

  if(packet.opcode != WRQ) {
    printf("Opcode is %d but should be %d\n", packet.opcode, WRQ);
    abort();
  }
  if(strncmp(packet.content.rq.filename, filename, strlen(filename)) != 0) {
    printf("Filename is %s but should be %s\n",
        packet.content.rq.filename,
        filename);
    abort();
  }
  if(strncmp(packet.content.rq.mode, mode, strlen(mode)) != 0) {
    printf("Mode is %s but should be %s\n",
        packet.content.rq.mode,
        mode);
    abort();
  }
}

void unmarshallDATA() {
  uint16_t block = 42;
  const char* data = "some data to fill the packet!";
  uint16_t blockNet = htons(block);
  char buf[2 + 2 + strlen(data)];
  buf[0] = 0;
  buf[1] = 3;
  memcpy(&buf[2], &blockNet, 2);
  strncpy(&buf[4], data, strlen(data));
  packet_t packet = unmarshall(buf);

  if(packet.opcode != DATA) {
    printf("Opcode is %d but should be %d\n", packet.opcode, DATA);
    abort();
  }
  if(packet.content.data.block != block) {
    printf("Block is %d but should be %d\n", packet.content.data.block, block);
    abort();
  }
  if(strncmp(packet.content.data.data, data, strlen(data)) != 0) {
    printf("Data is \"%s\" but should be \"%s\"\n",
        packet.content.data.data,
        data);
    abort();
  }
}

void unmarshallACK() {
  uint16_t block = 42;
  uint16_t blockNet = htons(block);
  char buf[2 + 2];
  buf[0] = 0;
  buf[1] = 4;
  memcpy(&buf[2], &blockNet, 2);
  packet_t packet = unmarshall(buf);

  if(packet.opcode != DATA) {
    printf("Opcode is %d but should be %d\n", packet.opcode, DATA);
    abort();
  }
  if(packet.content.ack.block != block) {
    printf("Block is %d but should be %d\n", packet.content.ack.block, block);
    abort();
  }
}

int main(void) {
  readOpcodeSimple();
  writeOpcodeSimple();
  writeReadOpcodeIdentity();
  readWriteOpcodeIdentity();

  readErrcodeSimple();
  writeErrcodeSimple();
  writeReadErrcodeIdentity();
  readWriteErrcodeIdentity();

  unmarshallRRQ();
  unmarshallWRQ();
  unmarshallDATA();
  unmarshallACK();

}

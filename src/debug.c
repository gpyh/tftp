#include "debug.h"

#include <stdio.h>
#include <string.h>

#ifndef NDEBUG

void debugPrintPacket(const packet_t* packet) {
  switch(packet->opcode) {
    case RRQ: {
      const char* str = "|   RRQ   | %s | %s |\n";
      printf(str, packet->content.rq.filename, packet->content.rq.mode);
      } break;
    case WRQ: {
      const char* str = "|   WRQ   | %s | %s |\n";
      printf(str, packet->content.rq.filename, packet->content.rq.mode);
      } break;
    case ACK: {
      const char* str = "|   ACK   | %zu |\n";
      printf(str, packet->content.ack.block);
      } break;
    case DATA: {
      const char* str = "|  DATA   | %zu | %s |\n";
      char data[packet->content.data.datalen + 1];
      strncpy(data, packet->content.data.data, packet->content.data.datalen);
      data[packet->content.data.datalen] = 0;
      printf(str, packet->content.data.block, data);
      } break;
    case ERROR: {
      char* literrcode;
      switch(packet->content.error.errcode) {
        case UNDEFINED_ERROR:
          literrcode = " UNDEFINED_ERROR ";
          break;
        case FILENOTFOUND:
          literrcode = "  FILENOTFOUND   ";
          break;
        case ACCVIOL:
          literrcode = "     ACCVIOL     ";
          break;
        case DISKERR:
          literrcode = "     DISKERR     ";
          break;
        case ILLEGALOP:
          literrcode = "    ILLEGALOP    ";
          break;
        case NOID:
          literrcode = "      NOID       ";
          break;
        case FILEEXISTS:
          literrcode = "   FILEEXISTS    ";
          break;
        case NOUSER:
          literrcode = "     NOUSER      ";
          break;
        case MAX_ERRCODE:
          printf("Packet with errcode MAX_ERRCODE!\n");
          return;
        case MIN_ERRCODE:
          printf("Packet with errcode MIN_ERRCODE!\n");
          return;
        default:
          printf("Error packet with unknown/unhandled errcode!\n");
          return;
      }
      const char* str = "|  ERROR  | %s | %s |\n";
      printf(str, literrcode, packet->content.error.errmsg);
      } break;
    case MAX_OPCODE:
      printf("Packet with opcode MAX_OPCODE!\n");
      break;
    case MIN_OPCODE:
      printf("Packet with opcode MIN_OPCODE!\n");
      break;
    default:
      printf("Packet with unknown/unhandled opcode!\n");
      break;
  }
}

#endif // NDEBUG

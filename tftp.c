#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "serve.h"
#include "fetch.h"
#include "send.h"

int main(int argc, char* argv[]) {

  static char* cmds[] = { "serve", "fetch", "send" };

  const char* usage = 
    "Usage: tftp [-h] <command> [<args>]\n"
    "\n"
    "<command> is one of the following:\n"
    "  serve     Start a tftp server\n"
    "  fetch     Fetch file from a tftp server\n"
    "  send      Send file to a tftp server\n"
    "\n";

  if(argc < 2) {
    fprintf(stderr, "Please provide a command.\n\n%s", usage);
    exit(EXIT_FAILURE);
  }
  if((strlen(argv[1]) == 2 && strncmp(argv[1], "-h", 2) == 0) ||
      (strlen(argv[1]) == 6 && strncmp(argv[1], "--help", 6) == 0)) {
    printf("%s", usage);
    exit(EXIT_SUCCESS);
  }


  unsigned int i;
  for(i = 0; i < (sizeof cmds) / (sizeof (char*)); i++) {
    if(strncmp(argv[1], cmds[i], 6) == 0) {
      break;
    }
  }

  switch(i) {
    case 0:
      serve(argc, argv);
      break;
    case 1:
      fetch(argc, argv);
      break;
    case 2:
      send(argc, argv);
      break;
    default:
      fprintf(stderr, "%s is not a valid command.\n\n%s", argv[1], usage);
      exit(EXIT_FAILURE);
  }
  
}

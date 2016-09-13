#include "common.h"
#include "serve.h"

#include <getopt.h> // arg parsing
#include <signal.h> // signal handling
#include <errno.h>  // perror
#include <limits.h> // PATH_MAX
#include <unistd.h> // getcwd

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>


volatile bool running;

void sigUsrHandler(int signum) {
  if(signum != SIGINT) {
    fprintf(stderr, "Wrong signal.\n");
    exit(EXIT_FAILURE);
  }
  running = false;
}

void serve(int argc, char* argv[]) {

  const char* usage =
    "Usage: tftp serve [-w <windowsize>] [-b <blocksize>] [-directory <dir>] "
            "[-p <port>] [-a <attempts>] [-t <timeout>] [-h]\n"
    "\n"
    "  -h, --help\n"
    "  -w, --windowsize <windowsize>\n"
    "  -b, --blocksize <blocksize>\n"
    "  -d, --directory <directory>\n"
    "  -p, --port <port>\n"
    "  -a, --attempts <attempts>\n"
    "  -t, --timeout <timeout>\n"
    "\n";

  static struct option long_options[] = {
    {"help"      , no_argument,       NULL, 'h'},
    {"windowsize", required_argument, NULL, 'w'},
    {"blocksize" , required_argument, NULL, 'b'},
    {"directory" , required_argument, NULL, 'd'},
    {"port"      , required_argument, NULL, 'p'},
    {"attempts"  , required_argument, NULL, 'a'},
    {"timeout"   , required_argument, NULL, 't'}
  };

  char directory[PATH_MAX];
  int port;
  int attempts;
  int timeout;

  bool useDefaultDirectory = true;
  bool useDefaultPort      = true;
  bool useDefaultAttempts  = true;
  bool useDefaultTimeout   = true;

  if(argc < 2) {
    fprintf(stderr, "%s", usage);
    exit(EXIT_FAILURE);
  }

  int c;

  while (1) {
    int option_index = 0;
    c = getopt_long(argc - 1, &argv[1], "hw:b:dp", long_options, &option_index);
    if (c == -1) {
      break;
    }
    switch(c) {
      case 'h':
        printf("%s", usage);
        exit(EXIT_SUCCESS);
        break;
      case 'o':
        // TODO
        break;
      case 'p':
        // TODO
        break;
      default:
        fprintf(stderr, "%s", usage);
        exit(EXIT_FAILURE);
    }
  }

  if(useDefaultDirectory) {
    if(getcwd(directory, (sizeof directory)) == NULL) {
      perror("getcwd");
      exit(EXIT_FAILURE);
    }
  }
  if(useDefaultPort) {
    port = DEFAULT_PORT;
  }
  if(useDefaultAttempts) {
    attempts = DEFAULT_ATTEMPTS_SERVE;
  }
  if(useDefaultTimeout) {
    timeout = DEFAULT_TIMEOUT_SERVE;
  }

  struct sigaction action;
  action.sa_handler = sigUsrHandler;
  action.sa_flags = 0;
  if(sigfillset(&action.sa_mask) == -1) {
    perror("sigfillset");
    exit(EXIT_FAILURE);
  }
  if(sigaction(SIGINT, &action, NULL) == -1) {
    perror("sigaction");
    exit(EXIT_FAILURE);
  }
  
  running = true;

  sudpSocket_t* mainSocket = sudpCreateSocket();
  AdrInet* self = AdrInet_loopback(port);
  sudpInitSocket(mainSocket);

  (void)(attempts);

  while(running) {
    packet_t packet;
    waitPacket(&packet, mainSocket, self, timeout, MAX_BLOCKSIZE);
    // TODO
  }

  sudpCloseSocket(mainSocket);

  printf("Server successfully stopped.\n");
  exit(EXIT_SUCCESS);
}


#include "common.h"
#include "debug.h"
#include "serve.h"
#include "backlog.h"

#include <getopt.h>    // arg parsing
#include <signal.h>    // signal handling
#include <errno.h>     // perror
#include <limits.h>    // PATH_MAX
#include <unistd.h>    // getcwd
#include <semaphore.h> // semaphores
#include <pthread.h>   // threads

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
  uint16_t port;
  unsigned int windowsize;
  size_t blocksize;
  char directory[PATH_MAX];
  unsigned int attempts;
  unsigned int timeout;
  size_t backlogsize;
} config_t;

// Globals
static config_t cfg;
volatile bool running;

void sigUsrHandler(int signum) {
  if(signum != SIGINT) {
    fprintf(stderr, "Wrong signal.\n");
    exit(EXIT_FAILURE);
  }
  running = false;
}

static void handleArgs(config_t* config, int argc, char* argv[]) {

  const char* usage =
    "Usage: tftp serve [-w <windowsize>] [-b <blocksize>] [-directory <dir>] "
            "[-p <port>] [-a <attempts>] [-t <timeout>] [-B <backlogsize>]"
            "[-h]\n"
    "\n"
    "  -h, --help\n"
    "  -w, --windowsize <windowsize>\n"
    "  -b, --blocksize <blocksize>\n"
    "  -d, --directory <directory>\n"
    "  -p, --port <port>\n"
    "  -a, --attempts <attempts>\n"
    "  -t, --timeout <timeout>\n"
    "  -B, --backlogsize <backlogsize>\n"
    "\n";

  static struct option long_options[] = {
    {"help"       , no_argument,       NULL, 'h'},
    {"windowsize" , required_argument, NULL, 'w'},
    {"blocksize"  , required_argument, NULL, 'b'},
    {"directory"  , required_argument, NULL, 'd'},
    {"port"       , required_argument, NULL, 'p'},
    {"attempts"   , required_argument, NULL, 'a'},
    {"timeout"    , required_argument, NULL, 't'},
    {"backlogsize", required_argument, NULL, 'B'}
  };

  bool useDefaultWindowsize  = true;
  bool useDefaultBlocksize   = true;
  bool useDefaultDirectory   = true;
  bool useDefaultPort        = true;
  bool useDefaultAttempts    = true;
  bool useDefaultTimeout     = true;
  bool useDefaultBacklogsize = true;

  if(argc < 2) {
    fprintf(stderr, "%s", usage);
    exit(EXIT_FAILURE);
  }

  int c;

  while (1) {
    int option_index = 0;
    c = getopt_long(argc - 1, &argv[1], "hw:b:d:p:a:t:B:",
                    long_options, &option_index);
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

  if(useDefaultWindowsize) {
    config->windowsize = DEFAULT_WINDOWSIZE;
  }
  if(useDefaultBlocksize) {
    config->blocksize = DEFAULT_BLOCKSIZE;
  }
  if(useDefaultDirectory) {
    if(getcwd(config->directory, (sizeof config->directory)) == NULL) {
      perror("getcwd");
      exit(EXIT_FAILURE);
    }
  }
  if(useDefaultPort) {
    config->port = DEFAULT_PORT;
  }
  if(useDefaultAttempts) {
    config->attempts = DEFAULT_ATTEMPTS_SERVE;
  }
  if(useDefaultTimeout) {
    config->timeout = DEFAULT_TIMEOUT_SERVE;
  }
  if(useDefaultBacklogsize) {
    config->backlogsize = DEFAULT_BACKLOGSIZE;
  }

}

void answerRequest(packet_t* packet) {
  printf("I must answer to this: \n");
  debugPacket(packet);
};

void* routineRequests(void* arg) {
  backlog_t* backlog = (backlog_t*) arg;
  packet_t packet = popBacklog(backlog);
  answerRequest(&packet);
  return NULL;
}

void serve(int argc, char* argv[]) {
  
  handleArgs(&cfg, argc, argv);

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

  backlog_t* backlog = initBacklog(cfg.backlogsize);

  sudpSocket_t* mainSocket = sudpCreateSocket();
  AdrInet* self = AdrInet_loopback(cfg.port);
  sudpInitSocket(mainSocket);
  sudpAttachSocket(mainSocket, NULL, cfg.port, ANY);

  printf("Server started on port %d.\n", cfg.port);

  pthread_t threadRequests;
  if(pthread_create(&threadRequests, NULL, routineRequests, backlog) != 0) {
    perror("pthread_create");
    exit(EXIT_FAILURE);
  }

  int status;
  packet_t packet;
  while(running) {
    printf("Waiting...\n");
    status = waitPacket(&packet, mainSocket, self, MAX_BLOCKSIZE, 0);
    if(status == SUCCESS) {
      pushBacklog(&packet, backlog);
    } else {
      debugStatus(status);
    }
  }

  sudpCloseSocket(mainSocket);
  freeBacklog(backlog);

  printf("Server successfully stopped.\n");
  exit(EXIT_SUCCESS);
}


#include "common.h"
#include "fetch.h"

#include <getopt.h> // arg parsing
#include <limits.h> // PATH_MAX
#include <unistd.h> // getcwd
#include <libgen.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>


callbackAction_t onRequest(const connection_t* connection,
                           const packet_t* packetOut,
                           const packet_t* packetIn) {
  (void)(connection);
  (void)(packetOut);
  (void)(packetIn);
  printf("YOLO CLIENT");
  return ABORT;
}

void snapBaseOnCwd(char* buf, size_t buflen, const char* source) {
    char cwd[PATH_MAX];
    if(getcwd(cwd, (sizeof cwd)) == NULL) {
      perror("getcwd");
      exit(EXIT_FAILURE);
    }

    char sourceCpy[PATH_MAX];
    strcpy(sourceCpy, source);
    char* base = basename(sourceCpy);

    if(strlen(cwd) + 1 + strlen(base) + 1 > buflen) {
      fprintf(stderr, "Path is too long!\n");
      exit(EXIT_FAILURE);
    }

    strcpy(buf, cwd);
    buf[strlen(cwd)] = '/';
    strcpy(&buf[strlen(cwd) + 1], base);
}

void fetch(int argc, char* argv[]) {

  const char* usage =
    "Usage: tftp fetch <server> <file1> [-o <file2>] [-p <port>] "
            "[-a <attempts>] [-t <timeout>] [-h]\n"
    "\n"
    "  -h, --help\n"
    "  -o, --output <file2>\n"
    "  -p, --port <port>\n"
    "  -a, --attempts <attempts>\n"
    "  -t, --timeout <timeout>\n"
    "\n";

  static struct option long_options[] = {
    {"help"    , no_argument,       NULL, 'h'},
    {"output"  , required_argument, NULL, 'o'},
    {"port"    , required_argument, NULL, 'p'},
    {"attempts", required_argument, NULL, 'a'},
    {"timeout" , required_argument, NULL, 't'}
  };

  char* server;
  char* fnameRead;

  char fnameWrite[PATH_MAX];
  int port;
  int attempts;
  int timeout;

  bool useDefaultFnameWrite = true;
  bool useDefaultPort       = true;
  bool useDefaultAttempts   = true;
  bool useDefaultTimeout    = true;

  if(argc < 4) {
    fprintf(stderr, "%s", usage);
    exit(EXIT_FAILURE);
  }

  int c;

  // TODO check sizes
  while (1) {
    int option_index = 0;
    c = getopt_long(argc - 1, &argv[1], "ho:p:", long_options, &option_index);
    if (c == -1) {
      break;
    }
    switch(c) {
      case 'h':
        printf("%s", usage);
        exit(EXIT_SUCCESS);
        break;
      case 'o':
        if(strlen(optarg) >= (sizeof fnameWrite)) {
          fprintf(stderr, "Path is too long!\n");
          exit(EXIT_FAILURE);
        }
        strcpy(fnameWrite, optarg);
        useDefaultFnameWrite = false;
        break;
      case 'p':
        // TODO
        port = atoi(optarg);
        if(port <= 0) {
          fprintf(stderr, "Incorrect port!\n");
          exit(EXIT_FAILURE);
        }
        useDefaultPort = false;
        break;
      case 'a':
        attempts = atoi(optarg);
        if(attempts <= 0) {
          fprintf(stderr, "Incorrect number of attempts!\n");
          exit(EXIT_FAILURE);
        }
        useDefaultAttempts = false;
      case 't':
        timeout = atoi(optarg);
        if(timeout <= 0) {
          fprintf(stderr, "Incorrect timeout!\n");
          exit(EXIT_FAILURE);
        }
        useDefaultTimeout = false;
        break;
      default:
        fprintf(stderr, "%s", usage);
        exit(EXIT_FAILURE);
    }
  }
  
  for(int i = optind; i < argc; i++) {
    printf("%s\n", argv[i]);
  }

  // TODO properly
  server = argv[1];
  fnameRead = argv[2];

  if(useDefaultFnameWrite) {
    snapBaseOnCwd(fnameWrite, (sizeof fnameWrite), fnameRead);
  }
  if(useDefaultPort) {
    port = DEFAULT_PORT;
  }
  if(useDefaultAttempts) {
    attempts = DEFAULT_ATTEMPTS_FETCH;
  }
  if(useDefaultTimeout) {
    timeout = DEFAULT_TIMEOUT_FETCH;
  }

  printf("file2: %s\n", fnameWrite);
  
  connection_t connection;
  connection.socket = sudpCreateSocket();
  connection.self = AdrInet_loopback(9876);
  connection.other = AdrInet_new(server, port);
  connection.timeout = timeout;
  connection.attempts = attempts;
  sudpInitSocket(connection.socket);

  packet_t packetRRQ;
  createRRQ(&packetRRQ, fnameRead, "octet");
  packet_t packetIn;
  sendAndWait(&connection, &packetRRQ, &packetIn, onRequest);


  exit(EXIT_SUCCESS);
}

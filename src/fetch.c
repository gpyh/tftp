#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>

void fetch(int argc, char* argv[]) {

  const char* usage =
    "Usage: tftp fetch <server> <file1> [-o <file2>] [-p <port>]\n"
    "\n"
    "  -h, --help\n"
    "  -o, --output <file2>\n"
    "  -p, --port <port>\n"
    "\n";

  static struct option long_options[] = {
    {"help"  , no_argument,       NULL, 'h'},
    {"output", required_argument, NULL, 'o'},
    {"port"  , required_argument, NULL, 'p'}
  };

  int c;

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
}

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>

void serve(int argc, char* argv[]) {

  const char* usage =
    "Usage: tftp serve [-w <windowsize>] [-b <blocksize>] [-directory <dir>] [-p <port>] [-h]\n"
    "\n"
    "  -h, --help\n"
    "  -w, --windowsize <windowsize>\n"
    "  -b, --blocksize <blocksize>\n"
    "  -d, --directory <directory>\n"
    "  -p, --port <port>\n"
    "\n";

  static struct option long_options[] = {
    {"help"      , no_argument,       NULL, 'h'},
    {"windowsize", required_argument, NULL, 'w'},
    {"blocksize" , required_argument, NULL, 'b'},
    {"directory" , required_argument, NULL, 'd'},
    {"port"      , required_argument, NULL, 'p'}
  };

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
}


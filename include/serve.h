#ifndef TFTP_SERVE_H
#define TFTP_SERVE_H

#define DEFAULT_WINDOWSIZE 1
#define DEFAULT_BLOCKSIZE 512
#define DEFAULT_ATTEMPTS_SERVE 10
#define DEFAULT_TIMEOUT_SERVE 10
#define DEFAULT_BACKLOGSIZE 10

void serve(int argc, char* argv[]);

#endif // TFTP_SERVE_H


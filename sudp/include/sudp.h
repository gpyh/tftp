#ifndef SUDP_H
#define SUDP_H

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>

typedef struct {
    int fd;
    AdrInet *addr;
    bool bound;
} sudpSocket_t;

#define SUDP_SIZE_QUEUE 10
#define SUDP_BUFFER 1024
enum { ANY, LOOPBACK };


sudpSocket_t* sudpCreateSocket();
int sudpInitSocket(sudpSocket_t *socket);
int sudpAttachSocket(sudpSocket_t *sock, const char *address, uint16_t port,
                     int flags);
bool sudpIsAttached(sudpSocket_t *socket);
int sudpGetLocalName(sudpSocket_t *socket, char *buf, int buflen);
int sudpGetLocalIP(const sudpSocket_t *socket, char *ip, int iplen);
uint16_t sudpGetLocalPort(const sudpSocket_t *socket);
ssize_t sudpWriteToSocket(sudpSocket_t *socket, const AdrInet *address,
                          const char *buf, int buflen);
ssize_t sudpRecvFromSocket(sudpSocket_t *socket, char *buf, int buflen,
                           AdrInet *address, int timeout);
int sudpCloseSocket(sudpSocket_t *socket);

#endif // SUDP_H

#ifndef SUDP_H
#define SUDP_H

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

typedef struct {
    int sock;
    AdresseInternet *addr;
    int is_bound;
} sudpSocket_t;

#define SUDP_SIZE_QUEUE 10
#define SUDP_BUFFER 1024
enum { ANY, LOOPBACK };


sudpSocket_t* sudpCreateSocket();
int sudpInitSocket(sudpSocket_t *socket);
int sudpAttachSocket(sudpSocket_t *sock, const char *address, uint16_t port, int flags);
int sudpIsAttached(sudpSocket_t *socket);
int sudpGetLocalName(sudpSocket_t *socket, char *buffer, int taille);
int sudpGetLocalIP(const sudpSocket_t *socket, char *localIP, int tailleIP);
uint16_t sudpGetLocalPort(const sudpSocket_t *socket);
ssize_t sudpWriteToSocket(sudpSocket_t *sock, const AdresseInternet *address, const char *buffer, int length);
ssize_t sudpRecvFromSocket(sudpSocket_t *socket, char *buffer, int length, AdresseInternet *address, int timeout);
int sudpCloseSocket(sudpSocket_t *socket);

#endif // SUDP_H

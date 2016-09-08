#ifndef SOCKUDPTYPE_H_
#define SOCKUDPTYPE_H_

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

typedef struct SocketUDP {
    int sock;
    AdresseInternet *addr;
    int is_bound;
} SocketUDP;

#define SIZE_QUEUE 10
#define BUFFER 1024
enum { ANY, LOOPBACK};


SocketUDP* createSocketUDP ();
int initSocketUDP(SocketUDP *socket);
int attacherSocketUDP(SocketUDP *sock, const char *address, uint16_t port, int flags);
int estAttachee(SocketUDP *socket);
int getLocalName(SocketUDP *socket, char *buffer, int taille);
int getLocalIP(const SocketUDP *socket, char *localIP, int tailleIP);
uint16_t getLocalPort(const SocketUDP *socket);
ssize_t writeToSocketUDP(SocketUDP *sock, const AdresseInternet *address, const char *buffer, int length);
ssize_t recvFromSocketUDP(SocketUDP *socket, char *buffer, int length, AdresseInternet *address, int timeout);
int closeSocketUDP(SocketUDP *socket);

#endif // SOCKUDPTYPE_H_

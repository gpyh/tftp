#include <string.h>
#include <AdresseInternet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "sudp.h"

sudpSocket_t *sudpCreateSocket () {
    sudpSocket_t *sock = (sudpSocket_t*)malloc(sizeof(sudpSocket_t));
    memset(sock, 0, sizeof(*sock));
    return sock;
}

int sudpInitSocket(sudpSocket_t *sock) {
    if ((sock->sock = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {
        return -1;
    }
    return 0;
}

int sudpAttachSocket(sudpSocket_t *sock, const char *address, uint16_t port, int flags) {
    if (flags == 0 && address == NULL) {
        sock->addr = AdresseInternet_any (port);
    } else if (flags == LOOPBACK) {
        sock->addr = AdresseInternet_loopback(port);
        sock->is_bound = 1;
    } else if (address != NULL) {
        sock->addr = AdresseInternet_new(address, port);
        sock->is_bound = 1;
    }
    struct sockaddr addr;
    AdresseInternet_to_sockaddr(sock->addr, &addr);

    bind(sock->sock, &addr, sizeof(addr));
    return -1;
}

int sudpIsAttached(sudpSocket_t *socket) {
    if (socket->is_bound == 1) {
        return -1;
    }
    else {
        return 0;
    }
}

int sudpGetLocalName(sudpSocket_t *socket, char *buffer, int taille) {
    if(socket == NULL || socket->addr == NULL || buffer == NULL) {
        return -1;
    }
    AdresseInternet_getinfo(socket->addr, buffer, taille, NULL, 0);
    int n = strlen(buffer);
    return n;
}

int sudpGetLocalIP(const sudpSocket_t *socket, char *localIP, int tailleIP) {
    if(socket == NULL || socket->addr == NULL || localIP == NULL) {
        return -1;
    }
    AdresseInternet_getIP(socket->addr, localIP, tailleIP);
    int n = strlen(localIP);
    return n;
}

uint16_t sudpGetLocalPort (const sudpSocket_t *socket) {
    if(socket == NULL || socket->addr == NULL) {
        return -1;
    }
    return AdresseInternet_getPort(socket->addr);
}

ssize_t sudpWriteToSocket (sudpSocket_t *sock, const AdresseInternet *address, const char *buffer, int length) {
    if(sock == NULL || address == NULL || buffer == NULL) {
        return (ssize_t)-1;
    }
    struct sockaddr sockAddr;
    AdresseInternet_to_sockaddr(address, &sockAddr);
    return sendto(sock->sock, buffer, (size_t)length, 0, &sockAddr, sizeof(sockAddr));
}

ssize_t sudpRecvFromSocket (sudpSocket_t *sock, char *buffer, int length, AdresseInternet *address, int timeout) {
    if(sock == NULL || address == NULL || buffer == NULL) {
        return (ssize_t)-1;
    }
    struct sockaddr sockAddr;
    memset(&sockAddr, 0, sizeof(struct sockaddr));
    socklen_t *addr_length = (socklen_t*)malloc(sizeof(*addr_length));
    *addr_length = sizeof(struct sockaddr);
    int nfds;
    fd_set read_fds;
    struct timeval tv;
    tv.tv_sec = timeout;
    tv.tv_usec = 0;
    FD_ZERO(&read_fds);
    FD_SET(sock->sock, &read_fds);
    nfds = (sock->sock+1);
    int result = select(nfds, &read_fds, NULL, NULL, &tv);
    if(result == -1) {
        perror("Select : ");
        return -1;
    }
    ssize_t size = 0;
    if(FD_ISSET(sock->sock, &read_fds)) {
       size = recvfrom(sock->sock, buffer, (size_t)length, 0, &sockAddr, addr_length);
    }
    sockaddr_to_AdresseInternet(&sockAddr, address); 
    free(addr_length);
    return size;
}

int sudpCloseSocket(sudpSocket_t *socket) {
    if(socket == NULL || socket->addr == NULL) {
        return -1;
    }
    int status = close(socket->sock);
    AdresseInternet_free(socket->addr);
    free(socket);
    return status;
}

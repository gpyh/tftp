#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <AdrInet.h>
#include "sudp.h"

int main (void) {
    char buffer[] = "Test de SocketUDP\n";
    char ip[128];
    memset(ip, 0, sizeof(ip));

    sudpSocket_t *sock = (sudpSocket_t*)malloc(sizeof(*sock));
    AdrInet *addr = (AdrInet*)malloc(sizeof(AdrInet));
    memset(addr, 0, sizeof(*addr));
    sudpInitSocket(sock);
    sudpAttachSocket(sock, NULL, 5555, LOOPBACK);
    sudpRecvFromSocket(sock, buffer, sizeof(buffer), addr, 100);
    AdrInet_getIP(addr, ip, sizeof(ip));
    printf("%s\n", ip);
    free(addr);
    sudpCloseSocket(sock);
    return 0;
}

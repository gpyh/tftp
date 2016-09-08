#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <AdresseInternet.h>
#include "SocketUDP.h"

int main (void) {
    char buffer[] = "Test de SocketUDP\n";
    char ip[128];
    memset(ip, 0, sizeof(ip));
    SocketUDP *sock = (SocketUDP*)malloc(sizeof(*sock));
    AdresseInternet *addr = (AdresseInternet*)malloc(sizeof(AdresseInternet));
    memset(addr, 0, sizeof(*addr));
    initSocketUDP(sock);
    attacherSocketUDP(sock, NULL, 5555, LOOPBACK);
    recvFromSocketUDP(sock, buffer, sizeof(buffer), addr, 100);
    AdresseInternet_getIP(addr, ip, sizeof(ip));
    printf("%s\n", ip);
    free(addr);
    closeSocketUDP(sock);
    return 0;
}

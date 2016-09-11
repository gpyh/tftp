#include "AdrInet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (void) {
    char input[BUFSIZ];
    printf("Adresse : ");
    scanf("%s", input);

    AdrInet *addr = AdrInet_new(input, 80);
    AdrInet *tmp = (AdrInet*)malloc(sizeof(AdrInet));
    memset(tmp, 0, sizeof(AdrInet));
    struct sockaddr_storage tmp_ss;
    AdrInet *tmp_addr = (AdrInet*)malloc(sizeof(AdrInet)); 
    memset(tmp_addr, 0, sizeof(AdrInet));

    /* Test de copie et de conversion */
    AdrInet_to_sockaddr(addr, (struct sockaddr*)&tmp_ss);
    sockaddr_to_AdrInet((struct sockaddr*)&tmp_ss, tmp_addr);
    AdrInet_copy(tmp, addr);
    /* Affichage */
    char buf[BUFSIZ];
    AdrInet_getIP(addr, buf, BUFSIZ);
    printf("Adresse IP(addr): %s\n", buf);
    AdrInet_getIP(tmp, buf, BUFSIZ);
    printf("Adresse IP(tmp): %s\n", buf);
    AdrInet_getIP(tmp_addr, buf, BUFSIZ);
    printf("Adresse IP(tmp): %s\n", buf);
    AdrInet_free(addr);
    printf("Les trois adresses IP sont normalement les mêmes ce qui signifient que la conversion et la copie sont OK !\n");

    /* Test de création any et loopback */
    addr = AdrInet_any(8080);
    printf("Test de any ! \n");
    printf("Nom: %s, Port: %s\n", addr->nom, addr->service);
    AdrInet_free(addr);
    addr = AdrInet_loopback(8080);
    printf("Test de loopback ! \n");
    printf("Nom: %s, Port: %s\n", addr->nom, addr->service);
    AdrInet_free(addr);
    AdrInet_free(tmp);
    AdrInet_free(tmp_addr);
    return 0;
}

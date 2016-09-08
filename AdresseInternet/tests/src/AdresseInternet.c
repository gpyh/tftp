#include "AdresseInternet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (void) {
    char input[BUFSIZ];
    printf("Adresse : ");
    scanf("%s", input);

    AdresseInternet *addr = AdresseInternet_new(input, 80);
    AdresseInternet *tmp = (AdresseInternet*)malloc(sizeof(AdresseInternet));
    memset(tmp, 0, sizeof(AdresseInternet));
    struct sockaddr_storage tmp_ss;
    AdresseInternet *tmp_addr = (AdresseInternet*)malloc(sizeof(AdresseInternet)); 
    memset(tmp_addr, 0, sizeof(AdresseInternet));

    /* Test de copie et de conversion */
    AdresseInternet_to_sockaddr(addr, (struct sockaddr*)&tmp_ss);
    sockaddr_to_AdresseInternet((struct sockaddr*)&tmp_ss, tmp_addr);
    AdresseInternet_copy(tmp, addr);
    /* Affichage */
    char buf[BUFSIZ];
    AdresseInternet_getIP(addr, buf, BUFSIZ);
    printf("Adresse IP(addr): %s\n", buf);
    AdresseInternet_getIP(tmp, buf, BUFSIZ);
    printf("Adresse IP(tmp): %s\n", buf);
    AdresseInternet_getIP(tmp_addr, buf, BUFSIZ);
    printf("Adresse IP(tmp): %s\n", buf);
    AdresseInternet_free(addr);
    printf("Les trois adresses IP sont normalement les mêmes ce qui signifient que la conversion et la copie sont OK !\n");

    /* Test de création any et loopback */
    addr = AdresseInternet_any(8080);
    printf("Test de any ! \n");
    printf("Nom: %s, Port: %s\n", addr->nom, addr->service);
    AdresseInternet_free(addr);
    addr = AdresseInternet_loopback(8080);
    printf("Test de loopback ! \n");
    printf("Nom: %s, Port: %s\n", addr->nom, addr->service);
    AdresseInternet_free(addr);
    AdresseInternet_free(tmp);
    AdresseInternet_free(tmp_addr);
    return 0;
}

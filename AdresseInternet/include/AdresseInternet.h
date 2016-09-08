#ifndef ADRESSEINTERNET_H
#define ADRESSEINTERNET_H

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define _DNS_NAME_MAX_SIZE 256
#define _SERVICE_NAME_MAX_SIZE 20
 
typedef struct {
  struct sockaddr_storage sockAddr;
  char nom[_DNS_NAME_MAX_SIZE];
  char service[_SERVICE_NAME_MAX_SIZE];
} _adresseInternet_struct;

typedef _adresseInternet_struct AdresseInternet;

/* Functions */
AdresseInternet *AdresseInternet_new (const char* adresse, uint16_t port);
AdresseInternet *AdresseInternet_any (uint16_t port);
AdresseInternet *AdresseInternet_loopback (uint16_t port);
void AdresseInternet_free (AdresseInternet *adresse);
int AdresseInternet_getinfo (AdresseInternet *adresse, char *nomDNS, int tailleDNS, char *nomPort, int taillePort);
int AdresseInternet_getIP (const AdresseInternet *adresse, char *IP, int tailleIP);
uint16_t AdresseInternet_getPort (const AdresseInternet *adresse);
int AdresseInternet_getDomain(const AdresseInternet * adresse);
int sockaddr_to_AdresseInternet (const struct sockaddr *addr, AdresseInternet *adresse);
int AdresseInternet_to_sockaddr (const AdresseInternet *adresse, struct sockaddr *addr);
int AdresseInternet_compare (const AdresseInternet *adresse1, const AdresseInternet *adresse2);
int AdresseInternet_copy (AdresseInternet *adrdst, const AdresseInternet *adrsrc);


#endif // ADRESSEINTERNET_H_

#ifndef ADRINET_H
#define ADRINET_H

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define DNS_NAME_MAX_SIZE 256
#define SERVICE_NAME_MAX_SIZE 20
 
typedef struct {
  struct sockaddr_storage sockAddr;
  char nom[DNS_NAME_MAX_SIZE];
  char service[SERVICE_NAME_MAX_SIZE];
} _adresseInternet_struct;

typedef _adresseInternet_struct AdrInet;

/* Functions */
AdrInet *AdrInet_new (const char* adresse, uint16_t port);
AdrInet *AdrInet_any (uint16_t port);
AdrInet *AdrInet_loopback (uint16_t port);
void AdrInet_free (AdrInet *adresse);
int AdrInet_getinfo (AdrInet *adresse, char *nomDNS, int tailleDNS, char *nomPort, int taillePort);
int AdrInet_getIP (const AdrInet *adresse, char *IP, int tailleIP);
uint16_t AdrInet_getPort (const AdrInet *adresse);
int AdrInet_getDomain(const AdrInet * adresse);
int sockaddr_to_AdrInet (const struct sockaddr *addr, AdrInet *adresse);
int AdrInet_to_sockaddr (const AdrInet *adresse, struct sockaddr *addr);
int AdrInet_compare (const AdrInet *adresse1, const AdrInet *adresse2);
int AdrInet_copy (AdrInet *adrdst, const AdrInet *adrsrc);


#endif // ADRINET_H

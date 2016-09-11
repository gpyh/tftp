#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "AdrInet.h"

AdrInet *AdrInet_new (const char* adresse, uint16_t port) {
    /*Allocation mémoire pour la structure */
    AdrInet *addr = (AdrInet*)malloc(sizeof(*addr));
    /* Déclaration des structures à utiliser avec getaddrinfo */
    struct addrinfo hints, *res;
    char service[BUFSIZ];
    int err;
    memset(&hints, 0, sizeof(hints));
    hints.ai_flags |= AI_CANONNAME;
    sprintf(service, "%d", port); 
    if((err = getaddrinfo(adresse, service, &hints, &res)) != 0) {
        fprintf(stderr, "%s", gai_strerror(err));
        exit(EXIT_FAILURE);
    }
    /* Si l'appel a réussi on récupere la première adresse disponible */
    if(res->ai_family == AF_INET) {
        struct sockaddr_in *tmp = (struct sockaddr_in*)&(addr->sockAddr);
        memcpy(tmp, res->ai_addr, sizeof(struct sockaddr_in));
    } else if (res->ai_family == AF_INET6) {
        struct sockaddr_in6 *tmp = (struct sockaddr_in6*)&(addr->sockAddr);
        memcpy(tmp, res->ai_addr, sizeof(struct sockaddr_in6));
    }
    strcpy(addr->nom, res->ai_canonname);
    strcpy(addr->service, service);
    freeaddrinfo(res);
    return addr;
}

AdrInet *AdrInet_any (uint16_t port) {
    return AdrInet_new("0.0.0.0", port);
}

AdrInet *AdrInet_loopback (uint16_t port) {
    return AdrInet_new("127.0.0.1", port);
}

void AdrInet_free (AdrInet *adresse) {
    if(adresse != NULL) {
        free(adresse);
    }
}

int AdrInet_getinfo (AdrInet *adresse, char *nomDNS, int tailleDNS, char *nomPort, int taillePort) {
    if(adresse == NULL || (nomDNS == NULL && nomPort == NULL)) {
        return -1;
    }
    if(nomDNS != NULL) {
        if(!(strlen(adresse->nom) > (unsigned)tailleDNS)) {
            strncpy(nomDNS, adresse->nom, strlen(adresse->nom+1));
        } else {
            return -1;
        }
    }
    if(nomPort != NULL) {
        if(!(strlen(adresse->service) > (unsigned)taillePort)) {
            strncpy(nomPort, adresse->service, strlen(adresse->service+1));
        } else {
            return -1;
        }
    }
    return 0;
}

int AdrInet_getIP (const AdrInet *adresse, char *IP, int tailleIP) {
    if(adresse == NULL) {
        return -1;
    }
    if(adresse->sockAddr.ss_family == AF_INET) {
       struct sockaddr_in *tmp = (struct sockaddr_in*)(&adresse->sockAddr);
       if(inet_ntop(AF_INET, (void*)(&tmp->sin_addr), IP, tailleIP) == NULL) {
           return -1;
       }
    } else if (adresse->sockAddr.ss_family == AF_INET6) {
       struct sockaddr_in6* tmp = (struct sockaddr_in6*)(&adresse->sockAddr);
       if(inet_ntop(AF_INET6, (void*)(&tmp->sin6_addr), IP, tailleIP) == NULL) {
           return -1;
       }
    }
    return 0;
}

uint16_t AdrInet_getPort (const AdrInet *adresse) {
    if(adresse == NULL) {
        return -1;
    }
    uint16_t port = 0;
    port = atoi(adresse->service);
    return port;
}

int AdrInet_getDomain (const AdrInet *adresse) {
    if(adresse->sockAddr.ss_family != AF_INET || adresse->sockAddr.ss_family != AF_INET6) {
        return -1;
    }
    return adresse->sockAddr.ss_family;
}

int sockaddr_to_AdrInet (const struct sockaddr *addr, AdrInet *adresse) {
    if(addr == NULL || adresse == NULL ) {
        return -1;
    }
    if(addr->sa_family == AF_INET) {
        struct sockaddr_in* tmp = (struct sockaddr_in*)addr;
        memcpy(&adresse->sockAddr, addr, sizeof(*tmp));
        sprintf(adresse->nom, "%d", (int)tmp->sin_addr.s_addr);
        sprintf(adresse->service, "%hu", tmp->sin_port);
    } else if (addr->sa_family == AF_INET6) {
        struct sockaddr_in6* tmp = (struct sockaddr_in6*)addr;
        memcpy(&adresse->sockAddr, addr, sizeof(*tmp));
        sprintf(adresse->nom, "%s", tmp->sin6_addr.s6_addr);
        sprintf(adresse->service, "%hu", tmp->sin6_port);
    }
    return 0;
}

int AdrInet_to_sockaddr (const AdrInet *adresse, struct sockaddr *addr) {
    if(adresse == NULL || addr == NULL) {
        return -1;
    }
    if(adresse->sockAddr.ss_family == AF_INET) {
        memcpy(addr, (struct sockaddr_in*)&adresse->sockAddr, sizeof(struct sockaddr_in));
    } else if(adresse->sockAddr.ss_family == AF_INET6) {
        memcpy(addr, (struct sockaddr_in6*)&adresse->sockAddr, sizeof(struct sockaddr_in6));
    }
    return 0;
}

int AdrInet_compare (const AdrInet *adresse1, const AdrInet *adresse2) {
    if(adresse1 == NULL || adresse2 == NULL) {
        return -1;
    }
    if(adresse1->sockAddr.ss_family != adresse2->sockAddr.ss_family) {
        return 0;
    }
    if(adresse1->sockAddr.ss_family == AF_INET) {
        struct sockaddr_in* tmp1 = (struct sockaddr_in*)&adresse1->sockAddr;
        struct sockaddr_in* tmp2 = (struct sockaddr_in*)&adresse2->sockAddr;
        if(tmp1->sin_addr.s_addr == tmp2->sin_addr.s_addr && tmp1->sin_port == tmp2->sin_port) {
            return 1;
        }
    } else if(adresse1->sockAddr.ss_family == AF_INET6) {
        struct sockaddr_in6* tmp1 = (struct sockaddr_in6*)&adresse1->sockAddr;
        struct sockaddr_in6* tmp2 = (struct sockaddr_in6*)&adresse2->sockAddr;
        if(memcmp(&tmp1->sin6_addr, &tmp2->sin6_addr, sizeof(struct in6_addr)) && tmp1->sin6_port == tmp2->sin6_port) {
            return 1;
        }
    }
    return 0;
}

int AdrInet_copy (AdrInet *adrdst, const AdrInet *adrsrc) {
    memcpy(adrdst, adrsrc, sizeof(AdrInet));
    return 0;
}

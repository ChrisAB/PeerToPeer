#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>

typedef struct sokaddr_in Sokaddr_in;

int set_addr(Sokaddr_in *addr, char *name, u_int32_t inaddr, short sin_port);

int set_addr(Sokaddr_in *addr, char *name, u_int32_t inaddr, short sin_port){
    struct hostent *h;

    memset((void *)addr, 0, sizeof(*addr));
    addr->sin_family = AF_INET;
    if(name != NULL){
        h = gethostbyname(name);
        if (h == NULL)
            return 1;
        addr->sin_addr.s_addr = *(u_int32_t *) h->h_addr_list[0];
    }
    else addr->sin_addr.s_addr = htonl(inaddr);
    addr->sin_port = htons(sin_port);
    return 0;
}
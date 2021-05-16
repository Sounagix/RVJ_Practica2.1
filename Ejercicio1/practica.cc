#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

 int main(int argc, char** argv){
           
    struct addrinfo hints;
    struct addrinfo* result;

    memset((void *)&hints, 0, sizeof(struct addrinfo));

    // hints.ai_family = AF_INET;
    // hints.ai_socktype = SOCK_DGRAM;

    int rc = getaddrinfo(argv[1],argv[2],&hints,&result);

    if(rc != 0){
        std::cerr << "[getaddrinfo]" << gai_strerror(rc) << std::endl;
        return -1;
    }

    int sd = socket(result->ai_family,result->ai_socktype,0);

    if(sd == -1){
        std::cerr << "[Socket] creación de socket" << std::endl;
        return -1;
    }

    //bind(sd,result->ai_addr,result->ai_addrlen);

    for (auto i = result; i != NULL; i = i->ai_next) {

        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        getnameinfo(i->ai_addr,i->ai_addrlen,host,NI_MAXHOST,serv
        ,NI_MAXSERV,NI_NUMERICHOST);

        std::cout <<host <<" " << i->ai_family <<" "<<i->ai_socktype <<std::endl;

    } 
    freeaddrinfo(result);
    return 0; 
}

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include <unistd.h>

 int main(int argc, char** argv){
           
    struct addrinfo hints;
    struct addrinfo* result;

    memset((void *)&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    int rc = getaddrinfo(argv[1],argv[2],&hints,&result);

    if(rc != 0){
        std::cerr << "[getaddrinfo]" << gai_strerror(rc) << std::endl;
        return -1;
    }

    // sd = al identificador del socket
    int sd = socket(result->ai_family,result->ai_socktype,0);

    if(sd == -1){
        std::cerr << "[Socket] creación de socket" << std::endl;
        return -1;
    }

    // Asignar un nombre a un socket = -1 ha fallado en el linkeo
    if(bind(sd,result->ai_addr,result->ai_addrlen) == -1){
        std::cerr << "[Bind] Error al asignar el nombre" << std::endl;
        return -1;
    }

    bool run = true;
    char mensaje [100];
    time_t currTime;
    size_t timeSize;
    while(run){
        struct sockaddr cliente;
        socklen_t size = sizeof(struct sockaddr);
        ssize_t bitsDevueltos = recvfrom(sd,mensaje,100-1,0,&cliente,&size);
        if(bitsDevueltos > 0){
            std::cerr << "[recvfrom] el numero de bits devueltos son menores a 0" << std::endl;
            return -1;
        }
        // Ip del cliente
        char host[NI_MAXHOST];
        //  El puerto del cliente
        char serv[NI_MAXSERV];
        getnameinfo(&cliente,size,host,NI_MAXHOST,serv,NI_MAXSERV,NI_NUMERICHOST);
        std::cout << bitsDevueltos << "Bits devueltos " << host << serv << std::endl;

        switch(mensaje[0]){
            case 'q':
                run = false;
                std::cout << "Cierre de la app" << std::endl;
            break;
            // Devolvemos la fecha
            case 't':
                time(&currTime);
                //  Tamaño de la fecha
                timeSize = strftime(mensaje,100 - 1,"%T %p",localtime(&currTime));
                sendto(sd,mensaje,timeSize,0,&cliente,size);
            break;
            //  Devolvemos la hora
            case 'd':
                time(&currTime);
                //  Tamaño de la fecha
                timeSize = strftime(mensaje,100 - 1,"%F",localtime(&currTime));
                sendto(sd,mensaje,timeSize,0,&cliente,size);
            break;
            default:
                std::cout << "El comando " << mensaje[0] << " no soportado \n";
            break;
        }
    }

    close(sd);

    // for (auto i = result; i != NULL; i = i->ai_next) {


    //     getnameinfo(i->ai_addr,i->ai_addrlen,host,NI_MAXHOST,serv
    //     ,NI_MAXSERV,NI_NUMERICHOST);

    //     std::cout <<host <<" " << i->ai_family <<" "<<i->ai_socktype <<std::endl;

    // } 
    freeaddrinfo(result);
    return 0; 
}

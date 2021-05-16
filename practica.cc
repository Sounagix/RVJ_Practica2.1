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
#include <thread>

const int MAX_THREAD = 10;
const int MAX_LISTEN = 10;
class MessageManager{
    private:
    int socketClient;
    //int idClient;

    public:
    MessageManager(int socketClient_) : socketClient(socketClient_){};

    void conectClient(){

        char message[100];        
        bool active = true;

        while (active)
        {
            
            ssize_t bytes = recv(socketClient,message,99 * sizeof(char),0);
            
            if(bytes <= 0){
                std::cerr << "[recvfrom] El número de bits recibidos es incorrectos" << "\n";
                active = false;
                continue;
            }

            send(socketClient ,message ,bytes ,0);
        }
        close(socketClient);
        
    }
};


 int main(int argc, char** argv){
           
    struct addrinfo hints;
    struct addrinfo* result;

    memset((void *)&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;         //Para ipv4
    hints.ai_socktype = SOCK_DGRAM;    //Para UDP
    
    int rc = getaddrinfo(argv[1],argv[2],&hints,&result);


    if(rc != 0){
        std::cerr << "[getaddrinfo] " << gai_strerror(rc) << std::endl;
        return -1;
    }

    // sd = al identificador del socket
    int sd = socket(result->ai_family,result->ai_socktype,0);

    if(sd == -1){
        std::cerr << "[Socket] Error en la creación del socket" << std::endl;
        return -1;
    }

    if(bind(sd,result->ai_addr,result->ai_addrlen) == -1){
        std::cerr << "[bind] Error" << std::endl;
        return -1;
    }

    if(listen(sd, MAX_LISTEN) == -1){
        std::cerr << "[Listen] Error\n";
        return -1;
    }


    bool servActive = true;

    while (servActive)
    {   
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        struct sockaddr client;
        socklen_t clientSize = sizeof(client);
        int sockClient = accept(sd ,&client ,&clientSize);
        if(sockClient == -1){
            std::cerr << "[accept] Error al conectar\n";
        }
        
        getnameinfo(&client ,clientSize ,host ,NI_MAXHOST, serv ,NI_MAXSERV ,NI_NUMERICHOST | NI_NUMERICSERV);
        std::cout << "Conexión establecida con " << host << std::endl;

        MessageManager* m = new MessageManager(sockClient);
        std::thread([&m]() {m->conectClient(); delete m;});
    }    

    close(sd);
    freeaddrinfo(result);
    return 0; 
}

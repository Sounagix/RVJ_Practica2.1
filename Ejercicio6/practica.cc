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

class MessageManager{
    private:
    int socketClient;
    int idClient;

    public:
    MessageManager(int socketClient_,int idClient_) : socketClient(socketClient_),idClient(idClient_){};

    void conectClient(){

        char message[100];
        char host[NI_MAXHOST];
        char server[NI_MAXSERV];
        time_t timer;
        size_t size;
        struct sockaddr client;
        socklen_t clientSize = sizeof(client);
        
        bool active = true;

        while (active)
        {
            sleep(3);
            ssize_t bytes = recvfrom(socketClient,message,99 * sizeof(char),0,&client,&clientSize);
            message[100] = '\0';
            if(bytes == -1){
                std::cerr << "[recvfrom] El número de bits recibidos es incorrectos" << "\n";
                active = false;
                return;
            }
            getnameinfo(&client, clientSize, host, NI_MAXHOST, server, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
            std::cout << "Bytes recibidos " << bytes << " del " << host << ". Thread " << std::this_thread::get_id() << std::endl; 
             
            switch(message[0]){

                case 'd':
                    time(&timer);
                    size = strftime(message ,99 ,"%F" ,localtime(&timer) );
                    sendto(socketClient ,message ,size ,0 ,&client ,clientSize);
                break;
                case 't':
                    time(&timer);
                    size = strftime(message ,99 ,"%T %p" ,localtime(&timer) );
                    sendto(socketClient ,message ,size ,0 ,&client ,clientSize);
                break;
                default:
                    std::cout << "El comando " << message[0] << " no está soportado \n";
                    sendto(socketClient ,"Comando no soportado\n" ,19 ,0 ,&client ,clientSize);
                break; 
            }
        }
        
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

    for(int i = 0; i < 10 ;i++){
        MessageManager *m = new MessageManager(sd,i);
        std::thread([&m]() {m->conectClient(); delete m; }).detach();
    } 
    std::string key = "";
    while (key != "q")
    {
        std::cin >> key;
    }

    std::cout << "Cierre de conexión\n";
    

    close(sd);
    freeaddrinfo(result);
    return 0; 
}

#include <iostream>
#include <cstring>
#include <arpa/inet.h>

#define MAX_CLNT 256
#define SERVER_PORT 9997

int main(){
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr; 
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size;

    // Create an endpoint for communication. Return file descriptor for the new socket on success.
    serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serv_sock == -1){
        std::cerr << "socket() failed!" << std::endl;
        exit(0);
    }

    memset(&serv_addr,0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    // htonl, htons - convert values between host and network byte order
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 32 bit IPv4 address
    serv_addr.sin_port = htons(SERVER_PORT); // 16 bit port number

    // bind - assigns the addres specified by serv_addr to the socket refered by serv_sock  
    if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1){
        std::cerr << "bind() failed!" << std::endl;
        exit(0);
    }
    printf("the server is running on port %d\n", SERVER_PORT);

    // listen - listen on connections on a socket. marks the socket as passive (it means that it will accept incoming connection requests)
    if (listen(serv_sock, MAX_CLNT) == -1){
        std::cerr << "listen() error!" << std::endl;
        exit(0);
    }
}
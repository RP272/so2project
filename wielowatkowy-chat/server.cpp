#include <iostream>
#include <arpa/inet.h>

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
}
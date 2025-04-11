#include <iostream>
#include <cstring>
#include <thread>
#include <mutex>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unordered_map>
#include <stdarg.h>

#define MAX_CLNT 256
#define SERVER_PORT 9997

int clnt_cnt = 0;
std::mutex mtx;

void handle_clnt(int clnt_sock);

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

    while(true){  
        clnt_addr_size = sizeof(clnt_addr);
        // accept a connection on a socket. Extracts the first connection request on the pending connections from for the listening socket (serv_sock). Creates a new connected socket and returns a new file descriptor referring to that socket (clnt_sock).
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        if (clnt_sock == -1){
            std::cerr << "accept() error!" << std::endl;
            exit(0);
        }

        mtx.lock();
        clnt_cnt++;
        mtx.unlock();

        std::thread th(handle_clnt, clnt_sock);
        // Separates the thread of execution from the thread object, allowing execution to continue independently. Any allocated resources will be freed once the thread exits.
        th.detach();

        // The inet_ntoa() function converts the Internet host address in, given in network byte order, to a string in IPv4 dotted-decimal notation. The string is returned in a statically allocated buffer, which subsequent calls will overwrite.
        printf("Connected client IP: %s \n", inet_ntoa(clnt_addr.sin_addr));
    }
    close(serv_sock);
    return 0;
}
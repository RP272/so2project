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
#define BUF_SIZE 1024

int clnt_cnt = 0;
std::mutex mtx;
std::unordered_map<std::string, int>clnt_socks;

void handle_clnt(int clnt_sock);
void send_msg(const std::string &msg);

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

void handle_clnt(int clnt_sock) {
    char client_message[BUF_SIZE];
    bool user_registered = false;
    const char* tell_name = "#new client:";

    while (true) {
        // recv - receive a message from a socket. If no messages are available at the socket, the receive calls wait for a message to arrive, unless the socket  is  nonblocking in  which  case the value -1 is returned and errno is set to EAGAIN or EWOULDBLOCK.
        ssize_t recv_len = recv(clnt_sock, client_message, sizeof(client_message), 0);
        
        // Check if client left the chat. If so, break the loop.
        if (recv_len <= 0) break;

        if (std::strlen(client_message) > std::strlen(tell_name)) {
            // Check if the client_message starts with '#new client:'. If yes, it means that the clients sends in it's name.
            if (std::strncmp(client_message, tell_name, std::strlen(tell_name)) == 0) {
                char name[20];
                std::strcpy(name, client_message + std::strlen(tell_name));

                mtx.lock();
                // Check if a socket with received name already exists in the unordered map. 
                if (clnt_socks.find(name) == clnt_socks.end()) {
                    printf("the name of socket %d: %s\n", clnt_sock, name);
                    clnt_socks[name] = clnt_sock;
                    user_registered = true;
                    printf("number of users: %d\n", clnt_cnt);
                } else {
                    std::string error_msg = std::string(name) + " exists already. Please quit and enter with another name!";
                    // send - send a message on a socket
                    // 1st - socket descriptor
                    // 2nd - message buffer
                    // 3rd - buffer length
                    // 4th - flags
                    send(clnt_sock, error_msg.c_str(), error_msg.length() + 1, 0);
                    clnt_cnt--;
                    mtx.unlock();
                    break;
                }
                mtx.unlock();
            }
        }

        if (user_registered == true) {
            // Broadcast the message from client to the whole chat.
            send_msg(std::string(client_message));
        }
    }

    if (user_registered == true) {
        // The client has left the chat. Remove the client from unordered map of sockets 
        std::string name, leave_msg;
        mtx.lock();
        auto it = clnt_socks.begin();
        while (it != clnt_socks.end()) {
            if (it->second == clnt_sock) {
                name = it->first;
                it = clnt_socks.erase(it);
                break;
            } else {
                ++it;
            }
        }
        clnt_cnt--;
        mtx.unlock();
        leave_msg = "client " + name + " leaves the chat room";
        send_msg(leave_msg);
        printf("client %s leaves the chat room\n", name.c_str());
    }

    close(clnt_sock);
}

void send_msg(const std::string &msg) {
    mtx.lock();
    for (const auto& [_, sock] : clnt_socks) {
        if (send(sock, msg.c_str(), msg.length() + 1, 0) == -1) {
            printf("Failed to send to socket %d\n", sock);
        }
    }
    mtx.unlock();
}
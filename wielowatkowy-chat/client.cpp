#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdarg.h>

#define BUF_SIZE 1024
#define SERVER_PORT 9997 
#define IP "127.0.0.1"

void send_msg(int sock);
void recv_msg(int sock);

std::string name = "DEFAULT";
std::string msg;

int main(int argc, const char **argv){
    int clnt_sock;
    struct sockaddr_in serv_addr;

    if (argc != 2){
        printf("Usage : %s <Name> \n", argv[0]);
        exit(1);
    }

    name = "["+std::string(argv[1])+"]";

    clnt_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clnt_sock == -1){
        printf("socket() failed!");
        exit(0);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(IP);
    serv_addr.sin_port = htons(SERVER_PORT);
    
    // connect - initiate a connection on a socket
    if (connect(clnt_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1){
        printf("connect() failed!");
        exit(0);
    }
    std::string my_name = "#new client:" + std::string(argv[1]);
    send(clnt_sock, my_name.c_str(), my_name.length() + 1, 0);
    
    std::thread snd(send_msg, clnt_sock);
    std::thread rcv(recv_msg, clnt_sock);
    
    snd.join();
    rcv.join();
    
    close(clnt_sock);

    return 0;
}

void send_msg(int clnt_sock){
    while(true){
        getline(std::cin, msg);
        if (msg == "Quit" || msg == "quit" || msg == "q" || msg == "Q"){
            close(clnt_sock);
            exit(0);
        }
        std::string name_msg = name + " " + msg;
        send(clnt_sock, name_msg.c_str(), name_msg.length() + 1, 0);
    }
}

void recv_msg(int clnt_sock){
    char name_msg[BUF_SIZE + name.length() + 1];
    while (1){
        int str_len = recv(clnt_sock, name_msg, BUF_SIZE+name.length() + 1, 0);
        if (str_len == -1){
            exit(-1);
        }
        printf("%s\n", name_msg);
    }
}
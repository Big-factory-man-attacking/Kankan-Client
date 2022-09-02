#include "tcpsocket.h"
#include <iostream>
#include "json.hpp"
#include <QString>
#include <cstddef>
#include "videosocialcontrol.h"

extern "C" {
    #include <sys/socket.h>
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <string.h>
    #include <time.h>
    #include <stdio.h>
    #include <stdlib.h>
}
#define MAXSIZE 2048



TcpSocket::TcpSocket()
{
}

void TcpSocket::Connect()
{
    m_sockfd = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9877);

    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    if (connect(m_sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect error");
    } else {
        std::cout << "------------------------" << std::endl;
    }
}

int TcpSocket::send(nlohmann::json js)
{
    char send[MAXSIZE+1];
    //清空缓冲区
    bzero(&send, sizeof(send));

    std::string str = js.dump();
    char* c =  str.data();
    std::cout << "\nsend:" << strlen(c) << "\n" << c << std::endl;
    int n = write(m_sockfd, c, strlen(c));

    return n;
}

nlohmann::json TcpSocket::receive()
{
    char recvline[MAXSIZE+1];
    //清空缓冲区
    bzero(&recvline, sizeof(recvline));

    if (read(m_sockfd, recvline, MAXSIZE) == 0) {
        perror("read error");
    }

    nlohmann::json json;
    json = nlohmann::json::parse(std::string(recvline));
    std::cout << "\nreceive:" << json.dump() << std::endl;
    return json;
}

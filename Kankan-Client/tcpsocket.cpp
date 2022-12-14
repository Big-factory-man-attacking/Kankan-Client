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
#define MAXSIZE 1024
#define MAXLINE 4096


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
    }
}

int TcpSocket::send(nlohmann::json js)
{
    char send[MAXSIZE+1];
    //清空缓冲区
    bzero(&send, sizeof(send));

    std::string str = js.dump();
    char* c =  str.data();
    int n = write(m_sockfd, c, strlen(c));

    std::cout << "\nsend:" << strlen(c) << "\n" << c << std::endl;
    return n;
}

nlohmann::json TcpSocket::receive()
{
    std::string s = "";
    char recvline[MAXSIZE+1];
    //清空缓冲区

    int rs = 1;
    while(rs) {
        bzero(&recvline, sizeof(recvline));
        int buflen = recv(m_sockfd, recvline, MAXSIZE, 0);
        if(buflen < 0) {
            // 由于是非阻塞的模式,所以当buflen为EAGAIN时,表示当前缓冲区已无数据可读
            // 在这里就当作是该次事件已处理
            if(buflen == EAGAIN) {
                std::cout << "error" << std::endl;
                break;
            } else {
                break;
            }
         } else if(buflen == 0) {
            std::cout << "close" << std::endl;
                // 这里表示对端的socket已正常关闭.
         }
         if(buflen != MAXSIZE)
            rs = 0;
         else
            rs = 1;// 需要再次读取
        s += recvline;
    }
    nlohmann::json json;
    json = nlohmann::json::parse(s);
    std::cout << "\nreceive:" << json.dump(4) << std::endl;
    return json;
}

int TcpSocket::sendVideo(std::string address)
{
    FILE* fq;
    if( ( fq = fopen(address.c_str(),"rb") ) == nullptr ){
        printf("File open.\n");
        close(m_sockfd);
        exit(1);
    }

    int len, n;
    char buffer[MAXLINE];
    bzero(buffer,sizeof(buffer));
    while(!feof(fq)){
        len = fread(buffer, 1, sizeof(buffer), fq);
        if((n = write(m_sockfd, buffer, len)) <= 0){
            printf("write error.\n");
            break;
        }
    }
    printf("n = %d", n);
    fclose(fq);
    return n;
}

#ifndef TCPSOCKET_H
#define TCPSOCKET_H
#include "json.hpp"

class TcpSocket
{
public:
    TcpSocket();
    void Connect();
    int getSockfd();
    int send(nlohmann::json js);
    nlohmann::json receive();
    int sendVideo(std::string address);

private:
    int m_sockfd;
};

#endif // TCPSOCKET_H

//
// Created by NavKav on 25/05/2025.
//

#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include "OSMultiplayerDependencies.h"


class ClientSocket {
public:
    ClientSocket(SOCKET tcpSocket, const sockaddr_in& tcpAddr, SOCKET udpSocket, const sockaddr_in& udpAddr, const std::string& name);
    void displayClientInfo() const;
    ~ClientSocket() = default;

private:
    friend class Server;
    SOCKET _tcpSocket;
    sockaddr_in _tcpAddr;
    SOCKET _udpSocket;
    sockaddr_in _udpAddr;
    std::string _name;
};



#endif //CLIENTSOCKET_H

//
// Created by NavKav on 25/05/2025.
//

#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include "OSMultiplayerDependencies.h"


class ClientSession {
public:
    ClientSession(SOCKET tcpSocket, const sockaddr_in& tcpAddr, int id);
    void displayClientInfo() const;
    ~ClientSession() = default;

private:
    friend class Server;
    SOCKET _tcpSocket;
    sockaddr_in _tcpAddr;
    int _id;
};



#endif //CLIENTSOCKET_H

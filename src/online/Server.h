//
// Created by NavKav on 18/03/2024.
//

#ifndef ARPG_STORYBOARD_SERVER_H
#define ARPG_STORYBOARD_SERVER_H

#include <map>

#include "OSMultiplayerDependencies.h"
#include "core/User.h"
#include "ClientSocket.h"


class ClientSocket;

class Server {
public:
    Server(int port);
    ~Server();
    void start();

    private :

    std::map<SOCKET, std::unique_ptr<ClientSocket>> _connectedTcpClients;
    SOCKET _udpSocket;
    SOCKET _tcpSocket;
    sockaddr_in _server;
};

#endif //ARPG_STORYBOARD_SERVER_H
//
// Created by NavKav on 18/03/2024.
//

#ifndef ARPG_STORYBOARD_SERVER_H
#define ARPG_STORYBOARD_SERVER_H

#include <map>
#include <vector>

#include "OSMultiplayerDependencies.h"
#include "core/User.h"
#include "ClientSocket.h"

class ClientSocket;

class Server {
public:
    Server(int port);
    ~Server();
    void start();
    void udpPacketHandling(sockaddr_in clientAddr, int bytesReceived, char* buffer);
    void tcpPacketHandling(SOCKET clientSock, int bytesReceived, char* buffer);
    bool tcpAcceptanceHandling(SOCKET newClientSocket, sockaddr_in clientAddr);

    private :

    std::map<SOCKET, std::unique_ptr<ClientSocket>> _connectedTcpClients;
    std::vector<SOCKET> _clientsToProcess;

    SOCKET _udpSocket;
    SOCKET _tcpSocket;
    sockaddr_in _server;
    int _addrLen;
};

#endif //ARPG_STORYBOARD_SERVER_H
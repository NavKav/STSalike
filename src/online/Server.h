//
// Created by NavKav on 18/03/2024.
//

#ifndef ARPG_STORYBOARD_SERVER_H
#define ARPG_STORYBOARD_SERVER_H

#include <map>
#include <vector>
#include <memory>
#include <algorithm>

#include "OSMultiplayerDependencies.h"
#include "ClientSocket.h"

#define BUFFER_SIZE 1024

class Server {
public:
    Server(int port);
    ~Server();
    void start();

    void udpPacketHandling(sockaddr_in& clientAddr);
    void tcpPacketHandling(SOCKET clientSock, int bytesReceived);
    bool tcpAcceptanceHandling(sockaddr_in& clientAddr);

    void sendToTcpClient(int clientId, const std::string& message);

private :

    std::vector<std::unique_ptr<ClientSocket>> _connectedTcpClients;
    std::vector<SOCKET> _clientsToProcess;

    SOCKET _udpSocket;
    SOCKET _tcpSocket;
    sockaddr_in _server;
    socklen_t  _addrLen;

    bool _serverToggle = true;

    char _buffer[BUFFER_SIZE] = {};

    int _idCount = 0;
};

#endif //ARPG_STORYBOARD_SERVER_H
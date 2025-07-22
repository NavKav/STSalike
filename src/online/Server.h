//
// Created by NavKav on 18/03/2024.
//

#ifndef ARPG_STORYBOARD_SERVER_H
#define ARPG_STORYBOARD_SERVER_H

#include <map>
#include <vector>
#include <memory>
#include <algorithm>
#include <thread>
#include <vector>
#include <algorithm>
#include <iostream>
#include <cstring>
#include <string>
#include <memory>
#include <set>

#include "OSMultiplayerDependencies.h"
#include "ClientSession.h"
#include "game/context/GameModel.h"

#define BUFFER_SIZE 1024

class Server {
public:
    Server(int port);
    ~Server();
    void start();

    void runNetworkLoop();

    private :
        void udpPacketHandling(sockaddr_in& clientAddr);
    bool tcpPacketHandling(std::vector<std::unique_ptr<ClientSession>>::iterator& clientIt, SOCKET clientSock);
    bool tcpAcceptanceHandling(sockaddr_in& clientAddr);
    void processOutgoingMessages();

    void sendToTcpClient(int clientId, const std::string& message);

    std::vector<std::unique_ptr<ClientSession>> _connectedTcpClients;
    std::vector<SOCKET> _clientsToProcess;

    SOCKET _udpSocket;
    SOCKET _tcpSocket;
    sockaddr_in _server;
    socklen_t  _addrLen;

    bool _serverToggle = true;

    char _buffer[BUFFER_SIZE] = {};

    int _idCount = 0;
    unsigned int _connectedTotal = 0;

    GameModel _gameModel;

    std::thread _gameModelThread;
};


#endif //ARPG_STORYBOARD_SERVER_H
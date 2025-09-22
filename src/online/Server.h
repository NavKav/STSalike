//
// Created by NavKav on 18/03/2024.
//

#ifndef ARPG_STORYBOARD_SERVER_H
#define ARPG_STORYBOARD_SERVER_H

#include <algorithm>
#include <vector>
#include <memory>
#include <thread>
#include <vector>
#include <string>
#include <memory>

#include "OSMultiplayerDependencies.h"
#include "ClientSession.h"
#include "game/context/GameModel.h"
#include "ServerConsole.h"
#include "util/ThreadPool.h"
#include "GameMessage.h"

#define BUFFER_SIZE 1024

class Server {
public:
    explicit Server(int port);
    ~Server();
    void start();
    void stop();

    void runNetworkLoop();

private :
    void udpPacketHandling(sockaddr_in& clientAddr);
    bool tcpPacketHandling(std::map<int, std::unique_ptr<ClientSession>>::iterator& clientIt);
    bool tcpAcceptanceHandling(sockaddr_in& clientAddr);
    void processOutgoingMessages();

    void sendToTcpClient(int clientId, const std::vector<char>& buffer);


    std::map<int, std::unique_ptr<ClientSession>> _connectedTcpClients;
    std::vector<SOCKET> _clientsToProcess;

    SOCKET _udpSocket;
    SOCKET _tcpSocket;
    sockaddr_in _server{};
    socklen_t  _addrLen{};

    std::atomic<bool> _serverToggle{true};

    char _buffer[BUFFER_SIZE] = {};

    int _idCount = 0;
    unsigned int _connectedTotal = 0;

    GameModel _gameModel;
    std::thread _gameModelThread;
};


#endif //ARPG_STORYBOARD_SERVER_H
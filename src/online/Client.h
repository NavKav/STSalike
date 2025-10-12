//
// Created by NavKav on 24/03/2024.
//

#ifndef ARPG_STORYBOARD_CLIENT_H
#define ARPG_STORYBOARD_CLIENT_H

#include <thread>
#include <chrono>
#include <memory>
#include <vector>
#include <queue> // Ajouter la file d'attente
#include <mutex> // Ajouter le mutex
#include "OSMultiplayerDependencies.h"
#include "GameMessage.h"

#define CLIENT_BUFFER_SIZE 1024

class Client {
public:
    Client(int port, const std::string& ip);
    ~Client();

    void startNetworkLoop();
    void stopNetworkLoop();

    void processAllMessages(std::queue<std::unique_ptr<GameMessage>>& destinationQueue);

    void sendUDP(const std::string& s);
    void sendTCP(const std::vector<char>& serializedMessage);
    std::vector<std::unique_ptr<GameMessage>> receiveTCP();

private:
    SOCKET _udpSocket, _tcpSocket;
    sockaddr_in _server{};

    std::vector<char> _incomingBuffer;
    bool _isRunning = true;

    std::thread _networkThread;

    std::queue<std::unique_ptr<GameMessage>> _messageQueue;
    std::mutex _messageMutex;

    void networkLoop();
};

#endif //ARPG_STORYBOARD_CLIENT_H

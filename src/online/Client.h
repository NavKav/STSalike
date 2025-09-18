//
// Created by NavKav on 24/03/2024.
//

#ifndef ARPG_STORYBOARD_CLIENT_H
#define ARPG_STORYBOARD_CLIENT_H

#include <thread>
#include <chrono>
#include <memory>
#include <vector>
#include "OSMultiplayerDependencies.h"
#include "GameMessage.h"

#define CLIENT_BUFFER_SIZE 1024

class Client {
public:
    Client(int port, const std::string& ip);
    ~Client();
    void sendUDP(const std::string& s);
    void sendTCP(const std::vector<char>& serializedMessage);
    std::unique_ptr<GameMessage> receiveTCP();

private:
    SOCKET _udpSocket, _tcpSocket;
    sockaddr_in _server{};

    char _receiveBuffer[CLIENT_BUFFER_SIZE] = {};
    std::vector<char> _incomingBuffer;
};


#endif //ARPG_STORYBOARD_CLIENT_H

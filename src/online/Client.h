//
// Created by NavKav on 24/03/2024.
//

#ifndef ARPG_STORYBOARD_CLIENT_H
#define ARPG_STORYBOARD_CLIENT_H

#include "OSMultiplayerDependencies.h"
#include <thread>
#include <chrono>

class Client {
public:
    Client(int port, const std::string& ip);
    ~Client();
    void sendTCP(const std::string& s) const;
    void sendUDP(const std::string& s);

private:
    SOCKET _udpSocket, _tcpSocket;
    sockaddr_in _server{};
};


#endif //ARPG_STORYBOARD_CLIENT_H

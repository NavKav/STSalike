//
// Created by navid on 18/03/2024.
//

#ifndef ARPG_STORYBOARD_SERVER_H
#define ARPG_STORYBOARD_SERVER_H

#include <iostream>
#include "OSMultiplayerDependencies.h"
#include "core/Player.h"
#include "util/FrameRate.h"



class Server {
public:
    Server(int port);
    ~Server();
    void start() const;

private :
    SOCKET _udpSocket;
    SOCKET _tcpSocket;
    sockaddr_in _server;
};

#endif //ARPG_STORYBOARD_SERVER_H

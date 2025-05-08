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
    void start();

private :
    SOCKET _serverSocket;
    sockaddr_in _server{}, _client{};
};

#endif //ARPG_STORYBOARD_SERVER_H

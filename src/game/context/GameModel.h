//
// Created by NavKav on 26/06/2025.
//
#ifndef GAMEMODEL_H
#define GAMEMODEL_H

#include <string>
#include <queue>
#include <mutex>
#include <iostream>
#include <utility>
#include <chrono>
#include <thread>

class GameModel {
public:
    GameModel();
    ~GameModel();

    void run();
    void stop();

    void addIncomingMessage(int clientId, const std::string& message);
    void addOutgoingMessage(int clientId, const std::string& message);

    std::queue<std::pair<int, std::string>> getAndClearOutgoingMessages();

private:
    bool _running;

    std::queue<std::pair<int, std::string>> _incomingMessages;
    std::mutex _incomingMutex;

    std::queue<std::pair<int, std::string>> _outgoingMessages;
    std::mutex _outgoingMutex;

    void processGameLogic();
};

#endif //GAMEMODEL_H
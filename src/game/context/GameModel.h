#ifndef GAMEMODEL_H
#define GAMEMODEL_H

#include <string>
#include <queue>
#include <mutex>
#include <memory>
#include <iostream>
#include <utility>
#include <chrono>
#include <thread>

#include "online/ServerConsole.h"
#include "util/ThreadPool.h"

enum MessageType {
    CONNECTION,
    DISCONNECTION,
    PLAYER_INPUT
};

struct GameMessage {
    MessageType type;
    int clientId;

    GameMessage(MessageType type, int clientId) : type(type), clientId(clientId) {}
};

class GameModel {
public:
    GameModel();
    ~GameModel();

    void run();
    void stop();

    void addIncomingMessage(std::unique_ptr<GameMessage> message);
    void addOutgoingMessage(std::unique_ptr<GameMessage> message);

    std::queue<std::unique_ptr<GameMessage>> getAndClearOutgoingMessages();

private:
    bool _running;

    std::queue<std::unique_ptr<GameMessage>> _incomingMessages;
    std::mutex _incomingMutex;

    std::queue<std::unique_ptr<GameMessage>> _outgoingMessages;
    std::mutex _outgoingMutex;

    void processGameLogic();
    void processGameMessage(std::queue<std::unique_ptr<GameMessage>>& currentIncomingMessages);
    Task createTaskFromMessage(std::unique_ptr<GameMessage> message);

};

#endif //GAMEMODEL_H
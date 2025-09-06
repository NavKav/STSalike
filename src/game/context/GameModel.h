#ifndef GAMEMODEL_H
#define GAMEMODEL_H

#include <string>
#include <queue>
#include <mutex>
#include <memory>
#include <iostream>
#include <utility>
#include <chrono>
#include <shared_mutex>
#include <thread>

#include "online/ServerConsole.h"
#include "util/ThreadPool.h"
#include "online/GameMessage.h"
#include "game/map/MapModel.h"
#include "game/Player/Player.h"
#include "online/Serializer.h"
#include "online/GlobalSerializer.h"

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

    std::unordered_map<int, std::unique_ptr<Player>> _players;
    std::shared_mutex _playersMutex;

    MapModel _mapModel;

    void processGameLogic();
    void processGameMessage(std::queue<std::unique_ptr<GameMessage>>& currentIncomingMessages);
    Task createTaskFromMessage(std::unique_ptr<GameMessage> message);
    Task createTaskFromMessageCONNECTION(std::unique_ptr<GameMessage> message);
    Task createTaskFromMessageINPUT(std::unique_ptr<GameMessage> message);
    Task createTaskFromMessageDISCONNECTION(std::unique_ptr<GameMessage> message);


};

#endif //GAMEMODEL_H
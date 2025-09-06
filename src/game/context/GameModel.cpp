#include "GameModel.h"

using namespace std;

GameModel::GameModel() :
_running(false),
_mapModel(1) {
}

GameModel::~GameModel() {
}

void GameModel::run() {
    _running = true;
    auto lastFrameTime = chrono::high_resolution_clock::now();

    while (_running) {
        auto currentFrameTime = chrono::high_resolution_clock::now();
        auto deltaTime = chrono::duration_cast<chrono::milliseconds>(currentFrameTime - lastFrameTime);
        lastFrameTime = currentFrameTime;

        auto processingStartTime = chrono::high_resolution_clock::now();

        queue<unique_ptr<GameMessage>> currentIncomingMessages;
        {
            lock_guard<mutex> lock(_incomingMutex);
            _incomingMessages.swap(currentIncomingMessages);
        }

        if (!currentIncomingMessages.empty()) {
            processGameMessage(currentIncomingMessages);
        }

        processGameLogic();

        auto processingTime = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - processingStartTime);
        auto sleepDuration = chrono::milliseconds(10) - processingTime;

        if (sleepDuration.count() > 0) {
            this_thread::sleep_for(sleepDuration);
        }
    }
    ServerConsole << "[GameModel] Thread de logique de jeu arrêté." << endl;
}

void GameModel::stop() {
    _running = false;
}

void GameModel::addIncomingMessage(unique_ptr<GameMessage> message) {
    lock_guard<mutex> lock(_incomingMutex);
    _incomingMessages.push(move(message));
}

void GameModel::addOutgoingMessage(unique_ptr<GameMessage> message) {
    lock_guard<mutex> lock(_outgoingMutex);
    _outgoingMessages.push(move(message));
}

queue<unique_ptr<GameMessage>> GameModel::getAndClearOutgoingMessages() {
    lock_guard<mutex> lock(_outgoingMutex);
    queue<unique_ptr<GameMessage>> messagesToReturn;
    _outgoingMessages.swap(messagesToReturn);
    return messagesToReturn;
}

void GameModel::processGameLogic() {
}
void GameModel::processGameMessage(queue<unique_ptr<GameMessage>>& currentIncomingMessages) {
    while (!currentIncomingMessages.empty()) {
        unique_ptr<GameMessage> message = move(currentIncomingMessages.front());
        currentIncomingMessages.pop();

        Task task = createTaskFromMessage(move(message));

        ThreadPool.enqueue(move(task));
    }
}

Task GameModel::createTaskFromMessage(std::unique_ptr<GameMessage> message) {
    switch (message->type) {
    case CONNECTION :
        return createTaskFromMessageCONNECTION(std::move(message));
    case DISCONNECTION :
        return createTaskFromMessageCONNECTION(std::move(message));
    case PLAYER_INPUT :
        return createTaskFromMessageINPUT(std::move(message));
    }
    return [](){};
}

Task GameModel::createTaskFromMessageCONNECTION(std::unique_ptr<GameMessage> message) {
    return [this, message = std::move(message)]() mutable {
        int clientId = message->clientId;
        {
            std::unique_lock<std::shared_mutex> lock(_playersMutex);
            if (_players.find(clientId) == _players.end()) {
                _players.emplace(clientId, std::make_unique<Player>(clientId, 0, 0));
                ServerConsole << "Player " << clientId << " spawned at " << _mapModel.getAdjacentNodes(0, 0).size() << std::endl;
            } else {
                ServerConsole << "Player " << clientId << " already exists." << std::endl;
                return;
            }
        }

        GlobalSerializer.clear();

        Node initialNode = _mapModel.getNode(0, 0);
        GlobalSerializer.serialize(initialNode, MessageHeaderType::NODE_UPDATE);

        auto adjacentNodeList = _mapModel.getAdjacentNodes(0, 0);
        for (const auto& n : adjacentNodeList) {
            GlobalSerializer.serialize(n, MessageHeaderType::NODE_UPDATE);
        }

        const auto& bufferToSend = GlobalSerializer.getBuffer();
        ServerConsole << "done" << endl;

        //_outgoingMessages.addMessage(clientId, bufferToSend);

    };
}

Task GameModel::createTaskFromMessageINPUT(std::unique_ptr<GameMessage> message) {
    return [this, message = std::move(message)]() mutable {
        ServerConsole << "Player" << message->clientId << "pressed space" << endl;
    };
}

Task GameModel::createTaskFromMessageDISCONNECTION(std::unique_ptr<GameMessage> message) {
    return [this, message = std::move(message)]() mutable {
    };
}

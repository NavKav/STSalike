//
// Created by NavKav on 26/06/2025.
//

#include "GameModel.h"

using namespace std;

GameModel::GameModel() : _running(false) {
}

GameModel::~GameModel() {
}

void GameModel::run() {
    _running = true;
    while (_running) {
        queue<std::unique_ptr<GameMessage>> currentIncomingMessages;
        {
            lock_guard<mutex> lock(_incomingMutex);
            _incomingMessages.swap(currentIncomingMessages);
        }

        processGameMessage(currentIncomingMessages);

        processGameLogic();

        this_thread::sleep_for(chrono::milliseconds(10));
    }
    ServerConsole::getInstance() << "[GameModel] Thread de logique de jeu arrêté." << endl;
}

void GameModel::stop() {
    _running = false;
}

queue<unique_ptr<GameMessage>> GameModel::getAndClearOutgoingMessages() {
    lock_guard<mutex> lock(_outgoingMutex);
    queue<unique_ptr<GameMessage>> messagesToReturn;
    _outgoingMessages.swap(messagesToReturn);
    return messagesToReturn;
}

void GameModel::addIncomingMessage(unique_ptr<GameMessage> message) {
    lock_guard<mutex> lock(_incomingMutex);
    _incomingMessages.push(move(message));
}

void GameModel::addOutgoingMessage(unique_ptr<GameMessage> message) {
    lock_guard<mutex> lock(_outgoingMutex);
    _outgoingMessages.push(move(message));
}

void GameModel::processGameLogic() {

}

void GameModel::processGameMessage(queue<std::unique_ptr<GameMessage>>& currentIncomingMessages) {
    while (!currentIncomingMessages.empty()) {
        auto message = *currentIncomingMessages.front();
        currentIncomingMessages.pop();
    }
}

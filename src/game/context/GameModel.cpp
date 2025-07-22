//
// Created by NavKav on 26/06/2025.
//

#include "GameModel.h"

GameModel::GameModel() : _running(false) {
}

GameModel::~GameModel() {
}

void GameModel::run() {
    _running = true;
    while (_running) {
        std::queue<std::pair<int, std::string>> currentIncomingMessages;
        {
            std::lock_guard<std::mutex> lock(_incomingMutex);
            _incomingMessages.swap(currentIncomingMessages);
        }

        while (!currentIncomingMessages.empty()) {
            auto message = currentIncomingMessages.front();
            currentIncomingMessages.pop();
            std::cout << "[GameModel] Traitement message entrant de client " << message.first
                      << ": " << message.second << std::endl;

            if (message.second == "demande_map") {
                addOutgoingMessage(message.first, "voici_la_map_...");
            }
        }

        processGameLogic();

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    std::cout << "[GameModel] Thread de logique de jeu arrêté." << std::endl;
}

void GameModel::stop() {
    _running = false;
}

void GameModel::addIncomingMessage(int clientId, const std::string& message) {
    std::lock_guard<std::mutex> lock(_incomingMutex);
    _incomingMessages.push({clientId, message});
}

std::queue<std::pair<int, std::string>> GameModel::getAndClearOutgoingMessages() {
    std::lock_guard<std::mutex> lock(_outgoingMutex);
    std::queue<std::pair<int, std::string>> messagesToReturn;
    _outgoingMessages.swap(messagesToReturn);
    return messagesToReturn;
}

void GameModel::addOutgoingMessage(int clientId, const std::string& message) {
    std::lock_guard<std::mutex> lock(_outgoingMutex);
    _outgoingMessages.push({clientId, message});
}

void GameModel::processGameLogic() {

}
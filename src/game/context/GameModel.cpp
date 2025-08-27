#include "GameModel.h"

using namespace std;

GameModel::GameModel() : _running(false) {
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
    ServerConsole::getInstance() << "[GameModel] Thread de logique de jeu arrêté." << endl;
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

        ThreadPool::getInstance().enqueue(move(task));
    }
}

Task GameModel::createTaskFromMessage(std::unique_ptr<GameMessage> message) {
    return [this, message = std::move(message)]() mutable {
        // TODO: Fill this section with your logic.
        // The 'message' variable (a unique_ptr) is available here.
        // Use a switch statement to handle different message types.
    };
}
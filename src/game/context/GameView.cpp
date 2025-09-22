//
// Created by NavKav on 23/01/2023.
//

#include "GameView.h"



using namespace std;

GameView::GameView() : _client(1998, "127.0.0.1") {
    _client.startNetworkLoop();
}

GameView::~GameView() {
    _client.stopNetworkLoop();
}

void GameView::process() {
    /*************************************************************************/
    /************************** INITIALIZATION  ******************************/
    /*************************************************************************/

    window.changeDrawColor(255, 0, 0, ALPHA_OPAQUE);
    window.clearBackground();

    /*************************************************************************/
    /************************** UNITS TESTS  *********************************/
    /*************************************************************************/



    /*************************************************************************/
    /************************** DISPLAY **************************************/
    /*************************************************************************/

    _mapView.displayMap();

    _frameRate.display();
    window.refresh();

    /*if (user[SDL_SCANCODE_SPACE].pressed) {
        _client.sendTCP("a appuye sur espace");
    }
    if (user[SDL_SCANCODE_Q].pressed) {
        _client.sendUDP("hello world");
    }*/

    processGameMessage();
}

void GameView::processGameMessage() {
    queue<unique_ptr<GameMessage>> messagesToProcess;

    _client.processAllMessages(messagesToProcess);

    while (!messagesToProcess.empty()) {
        unique_ptr<GameMessage> gameMessage = move(messagesToProcess.front());
        messagesToProcess.pop();

        switch (gameMessage->type) {
        case MessageType::NODE_UPDATE:
                processMessageNODEUPDATE(gameMessage);
                break;
        default:
            cout << "Message de type inconnu reçu." << endl;
            break;
        }
    }
}

void GameView::processMessageNODEUPDATE(unique_ptr<GameMessage>& gameMessage) {
    Node updatedNode;
    GlobalSerializer.deserialize<Node>(updatedNode, gameMessage->payload);

    _mapView.addNode(updatedNode.x, updatedNode.y, updatedNode);
}
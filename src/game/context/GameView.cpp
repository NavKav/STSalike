//
// Created by NavKav on 23/01/2023.
//

#include "GameView.h"

#include "core/User.h"


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

    window().changeDrawColor(255, 0, 0, ALPHA_OPAQUE);
    window().clearBackground();

    /*************************************************************************/
    /************************** DISPLAY **************************************/
    /*************************************************************************/

    // === Contrôle caméra (en pixels) ===
    static int camPx = 0;
    static int camPy = 0;
    const int step = 50;

    if (user()[SDL_SCANCODE_LEFT].pressed)  camPx -= step;
    if (user()[SDL_SCANCODE_RIGHT].pressed) camPx += step;
    if (user()[SDL_SCANCODE_UP].pressed)    camPy -= step;
    if (user()[SDL_SCANCODE_DOWN].pressed)  camPy += step;

    _mapView.setCameraCenter(camPx, camPy); // caméra en PIXELS

    _mapView.displayMap();

    window().refresh();

    processGameMessage();


    /*
    // Personnage qui se déplace sur une map

    static int paladinX = window().getX() / 2;
    static int paladinY = window().getY() / 2;

    window().drawIMG(0, 0, "image/fond.bmp");

    if (user()[SDL_SCANCODE_LEFT].pressed)
        paladinX -= 50;
    if (user()[SDL_SCANCODE_RIGHT].pressed)
        paladinX += 50;
    if (user()[SDL_SCANCODE_UP].pressed)
        paladinY -= 50;
    if (user()[SDL_SCANCODE_DOWN].pressed)
        paladinY += 50;

    window().drawIMG(paladinX, paladinY, "image/unit/paladin.png");

    window().refresh(); // une fois que l'on a dessiné des images sur la fenêtre, on
                      // rafraîchit celle-ci pour les voir.*/
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
            serverConsole() << "Message de type inconnu reçu." << endl;
            break;
        }
    }
}

void GameView::processMessageNODEUPDATE(unique_ptr<GameMessage>& gameMessage) {
    Node updatedNode;
    serializer().deserialize<Node>(updatedNode, gameMessage->payload);

    _mapView.addNode(updatedNode.x, updatedNode.y, updatedNode);
}
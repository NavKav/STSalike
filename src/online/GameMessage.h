//
// Created by NavKav on 29/08/2025.
//

#ifndef GAMEMESSAGE_H
#define GAMEMESSAGE_H

enum GameMessageType {
    CONNECTION,
    DISCONNECTION,
    PLAYER_INPUT
};

struct GameMessage {
    GameMessageType type;
    int clientId;

    GameMessage(GameMessageType type, int clientId) : type(type), clientId(clientId) {}
};

#endif //GAMEMESSAGE_H

//
// Created by NavKav on 29/08/2025.
//

#ifndef GAMEMESSAGE_H
#define GAMEMESSAGE_H

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

#endif //GAMEMESSAGE_H

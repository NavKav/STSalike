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
    std::vector<char> payload;

    GameMessage(GameMessageType type, int clientId, std::vector<char> payload)
        : type(type), clientId(clientId), payload(std::move(payload)) {}
};

#endif //GAMEMESSAGE_H

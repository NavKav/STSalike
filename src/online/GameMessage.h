//
// Created by NavKav on 29/08/2025.
//

#ifndef GAMEMESSAGE_H
#define GAMEMESSAGE_H

#include <cstdint>

enum class MessageType : uint32_t {
    CONNECTION,
    DISCONNECTION,
    PLAYER_INPUT,
    NODE_UPDATE
};

#pragma pack(push, 1)
struct MessageHeader {
    uint32_t size;
    MessageType type;
};
#pragma pack(pop)

struct GameMessage {
    MessageType type;
    int clientId;
    std::vector<char> payload;

    GameMessage(MessageType type, int clientId, std::vector<char> payload)
        : type(type), clientId(clientId), payload(std::move(payload)) {}
};

#endif //GAMEMESSAGE_H

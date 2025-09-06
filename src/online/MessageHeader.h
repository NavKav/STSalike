//
// Created by NavKav on 05/09/2025.
//

#ifndef MESSAGEHEADER_H
#define MESSAGEHEADER_H

#include <cstdint>

enum class MessageHeaderType : uint32_t {
    NODE_UPDATE = 1,
    PLAYER_POSITION = 2,
    GAME_EVENT = 3
};

#pragma pack(push, 1)
struct MessageHeader {
    uint32_t size;
    MessageHeaderType type;
};
#pragma pack(pop)

#endif //MESSAGEHEADER_H

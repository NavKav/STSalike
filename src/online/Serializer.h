//
// Created by NavKav on 01/09/2025.
//
#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <vector>
#include <stdexcept>
#include <cstring>
#include <cstdint>
#include <string>

#include "OSMultiplayerDependencies.h"
#include "MessageHeader.h"
#include "util/json.hpp"

template <typename T>
concept JSONable = requires(T object) {
    { nlohmann::json(object) };
    { nlohmann::json().get<T>() };
};


class Serializer {
public:
    using SerializedBuffer = std::vector<char>;

    template<JSONable T>
    void serialize(const T& data, MessageHeaderType messageType) {
        std::vector<uint8_t> msgpack_buffer = nlohmann::json::to_msgpack(data);

        MessageHeader header;
        header.type = static_cast<MessageHeaderType>(htonl(static_cast<uint32_t>(messageType)));
        header.size = htonl(static_cast<uint32_t>(msgpack_buffer.size()));

        _buffer.insert(_buffer.end(), reinterpret_cast<char*>(&header), reinterpret_cast<char*>(&header) + sizeof(MessageHeader));
        _buffer.insert(_buffer.end(), msgpack_buffer.begin(), msgpack_buffer.end());
    }

    void loadBuffer(const std::vector<char>& buffer) {
        _buffer = buffer;
        _offset = 0;
    }

    template<JSONable T>
    bool deserialize(T& data, MessageHeaderType& messageType) {
        if (_offset + sizeof(MessageHeader) > _buffer.size()) {
            return false;
        }

        MessageHeader header{};
        std::memcpy(&header, _buffer.data() + _offset, sizeof(MessageHeader));
        _offset += sizeof(MessageHeader);

        auto type = static_cast<MessageHeaderType>(ntohl(static_cast<uint32_t>(header.type)));
        uint32_t size = ntohl(header.size);

        if (_offset + size > _buffer.size()) {
            _offset -= sizeof(MessageHeader);
            return false;
        }

        std::vector<uint8_t> msgpack_buffer(_buffer.data() + _offset, _buffer.data() + _offset + size);
        _offset += size;

        try {
            nlohmann::json j = nlohmann::json::from_msgpack(msgpack_buffer);
            data = j.get<T>();
            messageType = type;
            return true;
        } catch (const nlohmann::json::exception& e) {
            std::cerr << "Erreur de deserialisation: " << e.what() << std::endl;
            return false;
        }
    }

    void clear() {
        _buffer.clear();
        _offset = 0;
    }

    const std::vector<char>& getBuffer() const {
        return _buffer;
    }

    bool hasMoreData() const {
        return _offset < _buffer.size();
    }

private:
    std::vector<char> _buffer;
    size_t _offset = 0;
};

#endif // SERIALIZER_H
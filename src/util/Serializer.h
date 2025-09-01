//
// Created by NavKav on 01/09/2025.
//

#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <vector>
#include <concepts>
#include <cstring>
#include <stdexcept>
#include <type_traits>

template <typename T>
concept TrivialType = std::is_trivially_copyable_v<T>;

class Serializer {
public:
    template<TrivialType T>
    void serialize(const T& data);

    template<TrivialType T>
    void deserialize(T& data);

    template<TrivialType T, TrivialType... Args>
    void serialize(const T& first, const Args&... rest);

    template<TrivialType T, TrivialType... Args>
    void deserialize(T& first, Args&... rest);

    const std::vector<char>& getBuffer() const;

    void clear();

private:
    std::vector<char> _buffer;
    size_t _offset = 0;
};

template<TrivialType T>
void Serializer::serialize(const T& data) {
    size_t current_size = _buffer.size();
    _buffer.resize(current_size + sizeof(T));
    std::memcpy(_buffer.data() + current_size, &data, sizeof(T));
}

template<TrivialType T>
void Serializer::deserialize(T& data) {
    if (_offset + sizeof(T) > _buffer.size()) {
        throw std::runtime_error("Buffer underflow during deserialization.");
    }
    std::memcpy(&data, _buffer.data() + _offset, sizeof(T));
    _offset += sizeof(T);
}

template<TrivialType T, TrivialType... Args>
void Serializer::serialize(const T& first, const Args&... rest) {
    serialize(first);
    if constexpr (sizeof...(Args) > 0) {
        serialize(rest...);
    }
}

template<TrivialType T, TrivialType... Args>
void Serializer::deserialize(T& first, Args&... rest) {
    deserialize(first);
    if constexpr (sizeof...(Args) > 0) {
        deserialize(rest...);
    }
}

inline const std::vector<char>& Serializer::getBuffer() const {
    return _buffer;
}

inline void Serializer::clear() {
    _buffer.clear();
    _offset = 0;
}

#endif // SERIALIZER_H
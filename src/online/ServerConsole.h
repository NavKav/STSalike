//
// Created by NavKav on 21/08/2025.
//

#ifndef SERVERCONSOLE_H
#define SERVERCONSOLE_H

#include <iostream>
#include <string>
#include <mutex>
#include <sstream>

class ServerConsole {
public:
    static ServerConsole& getInstance() {
        static ServerConsole instance;
        return instance;
    }

    template<typename T>
    ServerConsole& operator<<(const T& data) {
        m_oss << data;
        return *this;
    }

    ServerConsole& operator<<(std::ostream& (*manipulator)(std::ostream&)) {
        std::lock_guard<std::mutex> lock(m_mutex);

        std::cout << m_oss.str();

        manipulator(std::cout);

        m_oss.str("");
        m_oss.clear();

        return *this;
    }

    template<typename T, typename... Args>
    void writeLine(T&& first, Args&&... rest) {
        *this << std::forward<T>(first);

        writeLineRecursive(std::forward<Args>(rest)...);
    }

    void writeLineRecursive() {
        *this << std::endl;
    }

private:
    ServerConsole() = default;
    ~ServerConsole() = default;
    ServerConsole(const ServerConsole&) = delete;
    ServerConsole& operator=(const ServerConsole&) = delete;

    std::ostringstream m_oss;
    std::mutex m_mutex;

    template<typename T, typename... Args>
    void writeLineRecursive(T&& first, Args&&... rest) {
        *this << std::forward<T>(first);
        writeLineRecursive(std::forward<Args>(rest)...);
    }
};

inline ServerConsole& ServerConsole = ServerConsole::getInstance();

#endif //SERVERCONSOLE_H
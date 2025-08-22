//
// Created by NavKav on 21/08/2025.
//

#ifndef SERVERCONSOLE_H
#define SERVERCONSOLE_H

#include <iostream>
#include <string>
#include <mutex>

class ServerConsole {
public:
    // Singleton pattern to ensure only one instance exists
    static ServerConsole& getInstance() {
        static ServerConsole instance;
        return instance;
    }

    // Method to write a message to the console in a thread-safe way
    void write(const std::string& message) {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::cout << message;
    }

    template<typename T>
    ServerConsole& operator<<(const T& data) {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::cout << data;
        return *this;
    }

    ServerConsole& operator<<(std::ostream& (*manipulator)(std::ostream&)) {
        std::lock_guard<std::mutex> lock(m_mutex);
        manipulator(std::cout);
        return *this;
    }

private:
    ServerConsole() = default;
    ~ServerConsole() = default;
    ServerConsole(const ServerConsole&) = delete;
    ServerConsole& operator=(const ServerConsole&) = delete;

    std::mutex m_mutex;
};


#endif //SERVERCONSOLE_H

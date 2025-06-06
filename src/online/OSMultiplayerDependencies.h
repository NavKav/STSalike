//
// Created by NavKav on 18/03/2024.
//
#ifndef ARPG_STORYBOARD_OSDEPENDENCIES_H
#define ARPG_STORYBOARD_OSDEPENDENCIES_H

#if defined(_WIN32) || defined(_WIN64)
#define ARPG_OS_WINDOWS
#elif defined(__linux__)
#define ARPG_OS_LINUX
#elif defined(__APPLE__) && defined(__MACH__)
    #define ARPG_OS_MACOS
#else
    #error "Unsupported operating system"
#endif

#ifdef ARPG_OS_WINDOWS
#include <winsock2.h>
#include <windows.h>
#include <iostream>

using Socket = SOCKET;

inline void socketInitialisation() {
    WSADATA wsa;
    std::cout << "Initialising Winsock..." << std::endl;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "Failed. Error Code: " << WSAGetLastError() << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "Initialised." << std::endl;
}

inline int getSocketError() {
    return WSAGetLastError();
}

inline void disconnectSocket(Socket socket) {
    closesocket(socket);
}

inline void cleanupSocket() {
    WSACleanup();
}

#elif defined(ARPG_OS_LINUX) || defined(ARPG_OS_MACOS)
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using Socket = int;

inline void socketInitialisation() {}

inline int getSocketError() {
    return errno;
}

inline void disconnectSocket(Socket socket) {
    close(socket);
}

inline void cleanupSocket() {}

#endif

#endif // ARPG_STORYBOARD_OSDEPENDENCIES_H

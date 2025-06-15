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
#include <ws2tcpip.h>

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

inline void disconnectSocket(SOCKET socket) {
    closesocket(socket);
}

inline void cleanupSocket() {
    WSACleanup();
}

inline void setNonBlocking(SOCKET socket) {
    unsigned long mode = 1;
    ioctlsocket(socket, FIONBIO, &mode);
}

#elif defined(ARPG_OS_LINUX) || defined(ARPG_OS_MACOS)
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)

typedef int SOCKET;

inline void socketInitialisation() {}

inline int getSocketError() {
    return errno;
}

inline void disconnectSocket(SOCKET socket) {
    close(socket);
}

inline void cleanupSocket() {}

inline void setNonBlocking(SOCKET socket) {
    int flags = fcntl(socket, F_GETFL, 0);
    fcntl(socket, F_SETFL, flags | O_NONBLOCK);
}

#endif

#endif // ARPG_STORYBOARD_OSDEPENDENCIES_H
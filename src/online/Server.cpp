//
// Created by navid on 18/03/2024.
//

#include "Server.h"

using namespace std;

Server::Server(int port) {
    // Initialise Winsock
    socketInitialisation();

    // --- Création socket UDP ---
    _udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (_udpSocket == INVALID_SOCKET) {
        std::cout << "Could not create UDP socket: " << getSocketError() << std::endl;
        exit(EXIT_FAILURE);
    }

    // --- Création socket TCP ---
    _tcpSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_tcpSocket == INVALID_SOCKET) {
        std::cout << "Could not create TCP socket: " << getSocketError() << std::endl;
        disconnectSocket(_udpSocket);
        exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse commune
    memset(&_server, 0, sizeof(_server));
    _server.sin_family = AF_INET;
    _server.sin_addr.s_addr = INADDR_ANY;
    _server.sin_port = htons(port);

    // --- Bind UDP ---
    if (bind(_udpSocket, (sockaddr*)&_server, sizeof(_server)) == SOCKET_ERROR) {
        std::cout << "Bind UDP failed with error code: " << getSocketError() << std::endl;
        disconnectSocket(_udpSocket);
        disconnectSocket(_tcpSocket);
        exit(EXIT_FAILURE);
    }

    // --- Bind TCP ---
    if (bind(_tcpSocket, (sockaddr*)&_server, sizeof(_server)) == SOCKET_ERROR) {
        std::cout << "Bind TCP failed with error code: " << getSocketError() << std::endl;
        disconnectSocket(_udpSocket);
        disconnectSocket(_tcpSocket);
        exit(EXIT_FAILURE);
    }

    // --- Mise en écoute TCP ---
    if (listen(_tcpSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cout << "Listen failed with error code: " << getSocketError() << std::endl;
        disconnectSocket(_udpSocket);
        disconnectSocket(_tcpSocket);
        exit(EXIT_FAILURE);
    }

    std::cout << "Server ready: listening on port " << port << " (TCP & UDP)" << std::endl;
}

Server::~Server() {
    disconnectSocket(_tcpSocket);
    disconnectSocket(_udpSocket);
}

void Server::start() const {
    /*************************************************************************/
    /*************************  INDEX  ***************************************/
    /*************************************************************************/

    /*************************************************************************/
    /*************************  INITIALIZATION  ******************************/
    /*************************************************************************/

    /*************************************************************************/
    /*************************  UNITS TESTS  *********************************/
    /*************************************************************************/


    /*************************************************************************/
    /*************************  RUN SERVER  **********************************/
    /*************************************************************************/
    /*bool exitRequested = false;
    while (!exitRequested) {


    }*/

    fd_set readfds;
    char buffer[512];
    sockaddr_in clientAddr;
    int addrLen = sizeof(clientAddr);

    std::cout << "Serveur en attente de messages..." << std::endl;

    while (true) {
        FD_ZERO(&readfds);
        FD_SET(_udpSocket, &readfds);
        FD_SET(_tcpSocket, &readfds);
        SOCKET maxSocket = std::max(_udpSocket, _tcpSocket);

        // Attente d'activité sur l'un des deux sockets
        int activity = select(0, &readfds, nullptr, nullptr, nullptr);
        if (activity == SOCKET_ERROR) {
            std::cerr << "select() error: " << getSocketError() << std::endl;
            break;
        }

        // --- UDP ---
        if (FD_ISSET(_udpSocket, &readfds)) {
            memset(buffer, 0, sizeof(buffer));
            int bytesReceived = recvfrom(_udpSocket, buffer, sizeof(buffer) - 1, 0,
                                         (sockaddr*)&clientAddr, &addrLen);
            if (bytesReceived > 0) {
                buffer[bytesReceived] = '\0';
                std::cout << "[UDP] " << buffer << std::endl;
            }
        }

        // --- TCP (nouvelle connexion) ---
        if (FD_ISSET(_tcpSocket, &readfds)) {
            SOCKET clientSocket = accept(_tcpSocket, (sockaddr*)&clientAddr, &addrLen);
            if (clientSocket == INVALID_SOCKET) {
                std::cerr << "accept() failed: " << getSocketError() << std::endl;
                continue;
            }

            std::cout << "[TCP] Client connecté." << std::endl;

            // Lire les messages du client TCP
            while (true) {
                memset(buffer, 0, sizeof(buffer));
                int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
                if (bytesReceived <= 0) break; // client déconnecté
                buffer[bytesReceived] = '\0';
                std::cout << "[TCP] " << buffer << std::endl;
            }

            std::cout << "[TCP] Client déconnecté." << std::endl;
            disconnectSocket(clientSocket);
        }
    }
}
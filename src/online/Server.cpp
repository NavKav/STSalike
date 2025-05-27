//
// Created by NavKav on 18/03/2024.
//

#include <vector>
#include "Server.h"

using namespace std;

Server::Server(int port) {
    // Initialise Winsock
    socketInitialisation();

    // --- UDP ---
    _udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (_udpSocket == INVALID_SOCKET) {
        std::cout << "Could not create UDP socket: " << getSocketError() << std::endl;
        exit(EXIT_FAILURE);
    }

    // --- TCP ---
    _tcpSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_tcpSocket == INVALID_SOCKET) {
        std::cout << "Could not create TCP socket: " << getSocketError() << std::endl;
        disconnectSocket(_udpSocket);
        exit(EXIT_FAILURE);
    }

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

void Server::start() {
    fd_set readfds;
    char buffer[512];
    sockaddr_in clientAddr{};
    int addrLen;

    std::cout << "Serveur en attente de messages..." << std::endl;

    while (true) {
        addrLen = sizeof(clientAddr);
        memset(&clientAddr, 0, addrLen);

        /*************************************************************************/
        /***********************  READINESS SOCKET  ******************************/
        /*************************************************************************/

        FD_ZERO(&readfds);
        FD_SET(_udpSocket, &readfds);
        FD_SET(_tcpSocket, &readfds);

        SOCKET maxSocket = std::max(_udpSocket, _tcpSocket);

        for (auto const& pair : _connectedTcpClients) {
            SOCKET clientSock = pair.first;
            FD_SET(clientSock, &readfds);
            if (clientSock > maxSocket) {
                maxSocket = clientSock;
            }
        }

        int activity = select(maxSocket + 1, &readfds, nullptr, nullptr, nullptr);

        if (activity == SOCKET_ERROR) {
            std::cerr << "select() error: " << getSocketError() << std::endl;
            continue;
        }


        /*************************************************************************/
        /*************************  UDP HANDLING  ********************************/
        /*************************************************************************/

        if (FD_ISSET(_udpSocket, &readfds)) {
            memset(buffer, 0, sizeof(buffer));
            int bytesReceived = recvfrom(_udpSocket, buffer, sizeof(buffer) - 1, 0,(sockaddr*)&clientAddr, &addrLen);
            udpPacketHandling(clientAddr, bytesReceived, buffer);
        }

        /*************************************************************************/
        /*************************  TCP ACCEPTANCE  ******************************/
        /*************************************************************************/

        if (FD_ISSET(_tcpSocket, &readfds)) {
            SOCKET newClientSocket = accept(_tcpSocket, (sockaddr*)&clientAddr, &addrLen);
            if(tcpAcceptanceHandling(newClientSocket, clientAddr))
                continue;
        }

        /*************************************************************************/
        /*************************  TCP HANDLING  ********************************/
        /*************************************************************************/

        for (auto const& pair : _connectedTcpClients) {
            _clientsToProcess.push_back(pair.first);
        }

        for (SOCKET clientSock : _clientsToProcess) {
            if (FD_ISSET(clientSock, &readfds)) {
                memset(buffer, 0, sizeof(buffer));
                int bytesReceived = recv(clientSock, buffer, sizeof(buffer) - 1, 0);
                tcpPacketHandling(clientSock, bytesReceived, buffer);
            }
        }
    }
}

void Server::udpPacketHandling(sockaddr_in clientAddr, int bytesReceived, char* buffer) {
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        std::cout << "[UDP] Message de " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << " : " << buffer << std::endl;
    } else if (bytesReceived == 0) {
        std::cout << "[UDP] Erreur ou message vide reçu. (" << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << ")" << std::endl;
    } else {
        std::cerr << "[UDP] recvfrom() error: " << getSocketError() << " (" << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << ")" << std::endl;
    }
}

void Server::tcpPacketHandling(SOCKET clientSock, int bytesReceived, char *buffer) {
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        if (_connectedTcpClients.count(clientSock)) {
            std::cout << "[TCP] Message de " << _connectedTcpClients[clientSock]->_name << " ("
                      << inet_ntoa(_connectedTcpClients[clientSock]->_tcpAddr.sin_addr) << ":"
                      << ntohs(_connectedTcpClients[clientSock]->_tcpAddr.sin_port) << ") : "
                      << buffer << std::endl;
        }
    } else if (bytesReceived == 0) {
        std::cout << "[TCP] Client " << _connectedTcpClients[clientSock]->_name << " déconnecté." << std::endl;
        disconnectSocket(clientSock);
        _connectedTcpClients.erase(clientSock);
    } else {
        string errCode = getSocketError();
        if (errCode == "0") {
            std::cout << "[TCP] Client " << _connectedTcpClients[clientSock]->_name << " déconnecté de force (reset)." << std::endl;
        } else {
            std::cerr << "[TCP] recv() error on socket " << clientSock << ": " << errCode << std::endl;
        }
        disconnectSocket(clientSock);
        _connectedTcpClients.erase(clientSock);
    }
}

bool Server::tcpAcceptanceHandling(SOCKET newClientSocket, sockaddr_in clientAddr) {
    if (newClientSocket == INVALID_SOCKET) {
        std::cerr << "accept() failed: " << getSocketError() << std::endl;
        return false;
    }

    std::cout << "[TCP] Nouvelle connexion acceptée depuis "
              << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << std::endl;

    _connectedTcpClients[newClientSocket] = std::make_unique<ClientSocket>(
            newClientSocket, clientAddr, _udpSocket, sockaddr_in{}, "Client" + std::to_string(newClientSocket)
    );

    _connectedTcpClients[newClientSocket]->displayClientInfo();
    return true;
}

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
    cleanupSocket();
}

void Server::start() {
    fd_set readfds;
    sockaddr_in clientAddr{};

    std::cout << "Serveur en attente de messages..." << std::endl;

    while (_serverToggle) {
        _addrLen = sizeof(clientAddr);
        memset(&clientAddr, 0, _addrLen);
        _clientsToProcess.clear();

        /*************************************************************************/
        /*************************  MULTIPLEXING  ********************************/
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
            memset(_buffer, 0, sizeof(_buffer));
            udpPacketHandling(clientAddr);
        }

        /*************************************************************************/
        /*************************  TCP ACCEPTANCE  ******************************/
        /*************************************************************************/

        if (FD_ISSET(_tcpSocket, &readfds)) {
            if(tcpAcceptanceHandling(clientAddr))
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
                memset(_buffer, 0, sizeof(_buffer));
                int bytesReceived = recv(clientSock, _buffer, BUFFER_SIZE - 1, 0);
                tcpPacketHandling(clientSock, bytesReceived);
            }
        }
    }
}

void Server::udpPacketHandling(sockaddr_in& clientAddr) {
    int bytesReceived = recvfrom(_udpSocket, _buffer, BUFFER_SIZE - 1, 0,(sockaddr*)&clientAddr, &_addrLen);

    if (bytesReceived > 0) {
        _buffer[bytesReceived] = '\0';
        char ipBuffer[INET_ADDRSTRLEN];
        if (inet_ntop(AF_INET, &(clientAddr.sin_addr), ipBuffer, sizeof(ipBuffer)) != nullptr) {
            std::cout << "[UDP] Message de " << ipBuffer << ":" << ntohs(clientAddr.sin_port) << " : " << _buffer << std::endl;
        } else {
            std::cerr << "[UDP] Message reçu d'une adresse inconnue (conversion IP échouée)." << std::endl;
        }
    } else if (bytesReceived == 0) {
        char ipBuffer[INET_ADDRSTRLEN];
        if (inet_ntop(AF_INET, &(clientAddr.sin_addr), ipBuffer, sizeof(ipBuffer)) != nullptr) {
            std::cout << "[UDP] Erreur ou message vide reçu. (" << ipBuffer << ":" << ntohs(clientAddr.sin_port) << ")" << std::endl;
        } else {
            std::cerr << "[UDP] Erreur ou message vide reçu (adresse IP non convertie)." << std::endl;
        }
    } else {
        char ipBuffer[INET_ADDRSTRLEN];
        std::cerr << "[UDP] recvfrom() error: " << getSocketError();
        if (inet_ntop(AF_INET, &(clientAddr.sin_addr), ipBuffer, sizeof(ipBuffer)) != nullptr) {
            std::cerr << " (" << ipBuffer << ":" << ntohs(clientAddr.sin_port) << ")" << std::endl;
        } else {
            std::cerr << " (adresse inconnue)" << std::endl;
        }
    }
}

void Server::tcpPacketHandling(SOCKET clientSock, int bytesReceived) {
    if (bytesReceived > 0) {
        _buffer[bytesReceived] = '\0';
        if (_connectedTcpClients.count(clientSock)) {
            char ipBuffer[INET_ADDRSTRLEN];
            if (inet_ntop(AF_INET, &(_connectedTcpClients[clientSock]->_tcpAddr.sin_addr), ipBuffer, sizeof(ipBuffer)) != nullptr) {
                std::cout << "[TCP] Message de " << _connectedTcpClients[clientSock]->_id
                          << " (" << ipBuffer << ":"
                          << ntohs(_connectedTcpClients[clientSock]->_tcpAddr.sin_port) << ") : "
                          << _buffer << std::endl;
            } else {
                std::cerr << "[TCP] Message de " << _connectedTcpClients[clientSock]->_id
                          << " (adresse IP non convertie) : " << _buffer << std::endl;
            }
        }
    } else if (bytesReceived == 0) {
        std::cout << "[TCP] Client " << _connectedTcpClients[clientSock]->_id << " déconnecté." << std::endl;
        disconnectSocket(clientSock);
        _connectedTcpClients.erase(clientSock);
    } else {
        int errCode = getSocketError();
        if (errCode == 0) {
            std::cout << "[TCP] Client " << _connectedTcpClients[clientSock]->_id << " déconnecté de force (reset)." << std::endl;
        } else {
            std::cerr << "[TCP] recv() error on socket " << clientSock << ": " << errCode << std::endl;
        }
        disconnectSocket(clientSock);
        _connectedTcpClients.erase(clientSock);
        if (_connectedTcpClients.empty()) _serverToggle = false;
    }
}

bool Server::tcpAcceptanceHandling(sockaddr_in& clientAddr) {
    SOCKET newClientSocket = accept(_tcpSocket, (sockaddr*)&clientAddr, &_addrLen);

    if (newClientSocket == INVALID_SOCKET) {
        std::cerr << "accept() failed: " << getSocketError() << std::endl;
        return false;
    }

    char ipBuffer[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &(clientAddr.sin_addr), ipBuffer, sizeof(ipBuffer)) != nullptr) {
        std::cout << "[TCP] Nouvelle connexion acceptée depuis "
                  << ipBuffer << ":" << ntohs(clientAddr.sin_port) << std::endl;
    } else {
        std::cerr << "[TCP] Nouvelle connexion acceptée depuis une adresse inconnue." << std::endl;
    }

    _connectedTcpClients[newClientSocket] = std::make_unique<ClientSocket>(
            newClientSocket, clientAddr, _idCount
    );
    _idCount ++;

    _connectedTcpClients[newClientSocket]->displayClientInfo();
    return true;
}

//
// Created by NavKav on 24/03/2024.
//

#include "Client.h"

using namespace std;

Client::Client(int port, const string& ip) {
    socketInitialisation();

    // ---------- SOCKET UDP ----------
    _udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (_udpSocket == INVALID_SOCKET) {
        std::cerr << "Échec de la création du socket UDP : " << getSocketError() << std::endl;
        exit(EXIT_FAILURE);
    }

    // Création du socket UDP
    _udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (_udpSocket == INVALID_SOCKET) {
        std::cerr << "Échec création socket UDP : " << getSocketError() << std::endl;
        exit(EXIT_FAILURE);
    }

    // Création du socket TCP
    _tcpSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_tcpSocket == INVALID_SOCKET) {
        std::cerr << "Échec création socket TCP : " << getSocketError() << std::endl;
        disconnectSocket(_udpSocket);
        exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse du serveur (même pour TCP et UDP)
    memset(&_server, 0, sizeof(_server));
    _server.sin_family = AF_INET;
    _server.sin_port = htons(port);
    if (inet_pton(AF_INET, ip.c_str(), &(_server.sin_addr)) <= 0) {
        std::cerr << "Adresse IP invalide ou non supportée : " << ip << std::endl;
        disconnectSocket(_udpSocket);
        disconnectSocket(_tcpSocket);
        exit(EXIT_FAILURE);
    }

    // Connexion TCP
    int attempts = 0;
    while (attempts < 5) {
        if (connect(_tcpSocket, (sockaddr*)&_server, sizeof(_server)) != SOCKET_ERROR) {
            break; // Succès
        }

        int err = getSocketError();
        if (err != 0) {
            std::cout << "Connexion refusée, nouvelle tentative..." << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            attempts++;
        } else {
            std::cerr << "Erreur connect(): " << err << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    if (attempts == 5) {
        std::cerr << "Impossible de se connecter après plusieurs tentatives." << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Sockets TCP et UDP initialisés avec succès sur le même serveur." << std::endl;
}

Client::~Client() {
    disconnectSocket(_tcpSocket);
    disconnectSocket(_udpSocket);
}

void Client::sendUDP(const std::string& s) {
    const char* message = s.c_str();
    int msgLen = static_cast<int>(strlen(message));
    int slen = sizeof(_server);

    while (sendto(_udpSocket, message, msgLen, 0, (sockaddr*)&_server, slen) == SOCKET_ERROR) {
        std::cout << "sendto() failed with error code: " << getSocketError() << std::endl;
        exit(EXIT_FAILURE);
    }
}

void Client::sendTCP(const std::string& s) const {
    const char* message = s.c_str();
    int msgLen = static_cast<int>(strlen(message));
    int totalSent = 0;

    while (totalSent < msgLen) {
        int sent = send(_tcpSocket, message + totalSent, msgLen - totalSent, 0);
        if (sent == SOCKET_ERROR) {
            std::cout << "send() failed with error code: " << getSocketError() << std::endl;
            exit(EXIT_FAILURE);
        }
        totalSent += sent;
    }
}
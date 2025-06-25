//
// Created by NavKav on 18/03/2024.
//

#include <vector>
#include <algorithm> // Pour find_if et remove_if
#include <iostream>  // Pour cerr, cout
#include <cstring>   // Pour memset, strlen
#include <string>    // Pour string
#include <memory>    // Pour unique_ptr
#include <set>       // Si toujours utilisé pour d'autres raisons, sinon peut être retiré

#include "Server.h"
#include "OSMultiplayerDependencies.h"
#include "ClientSocket.h" // Assurez-vous que c'est inclus

using namespace std;

Server::Server(int port) {
    socketInitialisation();

    _udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (_udpSocket == INVALID_SOCKET) {
        cout << "Could not create UDP socket: " << getSocketError() << endl;
        exit(EXIT_FAILURE);
    }

    _tcpSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_tcpSocket == INVALID_SOCKET) {
        cout << "Could not create TCP socket: " << getSocketError() << endl;
        disconnectSocket(_udpSocket);
        exit(EXIT_FAILURE);
    }

    memset(&_server, 0, sizeof(_server));
    _server.sin_family = AF_INET;
    _server.sin_addr.s_addr = INADDR_ANY;
    _server.sin_port = htons(port);

    if (::bind(_udpSocket, (sockaddr*)&_server, sizeof(_server)) == SOCKET_ERROR) {
        cout << "Bind UDP failed with error code: " << getSocketError() << endl;
        disconnectSocket(_udpSocket);
        disconnectSocket(_tcpSocket);
        exit(EXIT_FAILURE);
    }

    if (::bind(_tcpSocket, (sockaddr*)&_server, sizeof(_server)) == SOCKET_ERROR) {
        cout << "Bind TCP failed with error code: " << getSocketError() << endl;
        disconnectSocket(_udpSocket);
        disconnectSocket(_tcpSocket);
        exit(EXIT_FAILURE);
    }

    if (::listen(_tcpSocket, SOMAXCONN) == SOCKET_ERROR) {
        cout << "Listen failed with error code: " << getSocketError() << endl;
        disconnectSocket(_udpSocket);
        disconnectSocket(_tcpSocket);
        exit(EXIT_FAILURE);
    }

    cout << "Server ready: listening on port " << port << " (TCP & UDP)" << endl;
}

Server::~Server() {
    disconnectSocket(_tcpSocket);
    disconnectSocket(_udpSocket);
    cleanupSocket();
}

void Server::start() {
    fd_set readfds;
    sockaddr_in clientAddr{};

    cout << "Serveur en attente de messages..." << endl;

    while (_serverToggle) {
        _addrLen = sizeof(clientAddr);
        memset(&clientAddr, 0, _addrLen);
        _clientsToProcess.clear();

        /*************************************************************************/
        /************************* MULTIPLEXING  ********************************/
        /*************************************************************************/

        FD_ZERO(&readfds);
        FD_SET(_udpSocket, &readfds);
        FD_SET(_tcpSocket, &readfds);

        SOCKET maxSocket = max(_udpSocket, _tcpSocket);

        for (const auto& clientPtr : _connectedTcpClients) {
            SOCKET clientSock = clientPtr->_tcpSocket;
            FD_SET(clientSock, &readfds);
            if (clientSock > maxSocket) {
                maxSocket = clientSock;
            }
        }

        int activity = select(maxSocket + 1, &readfds, nullptr, nullptr, nullptr);

        if (activity == SOCKET_ERROR) {
            cerr << "select() error: " << getSocketError() << endl;
            continue;
        }

        /************************************************************************/
        /************************* TCP ACCEPTANCE  ******************************/
        /************************************************************************/

        if (FD_ISSET(_tcpSocket, &readfds)) {
            if(tcpAcceptanceHandling(clientAddr)) {
                continue;
            }
        }

        /************************************************************************/
        /************************* UDP HANDLING  ********************************/
        /************************************************************************/

        if (FD_ISSET(_udpSocket, &readfds)) {
            memset(_buffer, 0, sizeof(_buffer));
            udpPacketHandling(clientAddr);
        }

        /************************************************************************/
        /************************* TCP HANDLING  ********************************/
        /************************************************************************/

        for (const auto& clientPtr : _connectedTcpClients) {
            SOCKET clientSock = clientPtr->_tcpSocket;
            if (FD_ISSET(clientSock, &readfds)) {
                _clientsToProcess.push_back(clientSock);
            }
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
    int bytesReceived = recvfrom(_udpSocket, _buffer, BUFFER_SIZE - 1, 0, (sockaddr*)&clientAddr, &_addrLen);

    char ipBuffer[INET_ADDRSTRLEN];

    if (bytesReceived >= 0) {
        _buffer[bytesReceived] = '\0';

        if (inet_ntop(AF_INET, &(clientAddr.sin_addr), ipBuffer, sizeof(ipBuffer)) != nullptr) {
            if (bytesReceived > 0) {
                cout << "[Chat] Message de " << ipBuffer << ":" << ntohs(clientAddr.sin_port) << " : " << _buffer << endl;
            }
        } else {
            cerr << "[UDP] Message ou paquet vide reçu d'une adresse inconnue (conversion IP échouée)." << endl;
        }
    } else {
        cerr << "[UDP] Erreur recvfrom(): " << getSocketError() << " (";
        if (inet_ntop(AF_INET, &(clientAddr.sin_addr), ipBuffer, sizeof(ipBuffer)) != nullptr) {
            cerr << string(ipBuffer) + ":" + to_string(ntohs(clientAddr.sin_port));
        } else {
            cerr << "adresse inconnue";
        }
          cerr << ")" << endl;
    }
}

void Server::tcpPacketHandling(SOCKET clientSock, int bytesReceived) {
    if (bytesReceived > 0) {
        _buffer[bytesReceived] = '\0';
        auto it = find_if(_connectedTcpClients.begin(), _connectedTcpClients.end(),
                               [clientSock](const unique_ptr<ClientSocket>& clientPtr) {
                                   return clientPtr->_tcpSocket == clientSock;
                               });

        if (it != _connectedTcpClients.end()) {
            ClientSocket* currentClient = (*it).get();
            char ipBuffer[INET_ADDRSTRLEN];
            if (inet_ntop(AF_INET, &(currentClient->_tcpAddr.sin_addr), ipBuffer, sizeof(ipBuffer)) != nullptr) {
                cout << "[TCP] Message de " << currentClient->_id
                          << " (" << ipBuffer << ":"
                          << ntohs(currentClient->_tcpAddr.sin_port) << ") : "
                          << _buffer << endl;
            } else {
                cerr << "[TCP] Message de " << currentClient->_id
                          << " (adresse IP non convertie) : " << _buffer << endl;
            }
        } else {
            cerr << "[TCP] Erreur: ClientSocket introuvable pour la socket " << clientSock << " lors de la réception d'un message." << endl;
        }
    } else {
        auto it = remove_if(_connectedTcpClients.begin(), _connectedTcpClients.end(),
                                 [clientSock](const unique_ptr<ClientSocket>& clientPtr) {
                                     return clientPtr->_tcpSocket == clientSock;
                                 });

        if (it != _connectedTcpClients.end()) {
            cout << "[TCP] Client ID " << (*it)->_id;
            if (bytesReceived == 0) {
                cout << " déconnecté." << endl;
            } else {
                 int errCode = getSocketError();
                 if (errCode == 0) {
                     cout << " déconnecté de force." << endl;
                 } else {
                     cerr << " déconnecté (erreur recv: " << errCode << ")." << endl;
                 }
            }
            _connectedTcpClients.erase(it, _connectedTcpClients.end());
        } else {
            cerr << "[TCP] Erreur: Tentative de déconnexion d'une socket (" << clientSock << ") non trouvée dans _connectedTcpClients." << endl;
        }

        ::closesocket(clientSock);
    }
}

bool Server::tcpAcceptanceHandling(sockaddr_in& clientAddr) {
    SOCKET newClientSocket = ::accept(_tcpSocket, (sockaddr*)&clientAddr, &_addrLen);

    if (newClientSocket == INVALID_SOCKET) {
        cerr << "accept() failed: " << getSocketError() << endl;
        return false;
    }

    char ipBuffer[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &(clientAddr.sin_addr), ipBuffer, sizeof(ipBuffer)) != nullptr) {
        cout << "[TCP] Nouvelle connexion acceptée depuis "
                  << ipBuffer << ":" << ntohs(clientAddr.sin_port) << endl;
    } else {
        cerr << "[TCP] Nouvelle connexion acceptée avec une erreur de conversion IP : "
                          << "Code d'erreur inet_ntop (errno) : " << getSocketError()
                          << ". Vérifiez la famille d'adresses ou la taille du buffer." << endl;
    }

    _connectedTcpClients.push_back(make_unique<ClientSocket>(
            newClientSocket, clientAddr, _idCount
    ));
    _idCount ++;

    _connectedTcpClients.back()->displayClientInfo();

    sendToTcpClient(_idCount-1, "bienvenu !");

    return true;
}

void Server::sendToTcpClient(int clientId, const string& message) {
    SOCKET targetSocket = INVALID_SOCKET;

    auto it = find_if(_connectedTcpClients.begin(), _connectedTcpClients.end(),
                           [clientId](const unique_ptr<ClientSocket>& clientPtr) {
                               return clientPtr->_id == clientId;
                           });

    if (it == _connectedTcpClients.end()) {
        cerr << "[Server] Erreur: Client TCP avec ID " << clientId
                  << " non trouvé ou déjà déconnecté." << endl;
        return;
    }

    targetSocket = (*it)->_tcpSocket;

    const char* data = message.c_str();
    int dataSize = message.length();

    cout << "[Server] Envoi du message TCP à client " << clientId
              << " : \"" << message << "\"" << endl;

    int bytesSent = ::send(targetSocket, data, dataSize, 0);

    if (bytesSent == SOCKET_ERROR) {
        int errCode = getSocketError();
        cerr << "[TCP] Erreur lors de l'envoi au client " << clientId
                  << " (Socket: " << targetSocket << "). Code d'erreur : " << errCode << endl;
    } else if (bytesSent == 0) {
        cout << "[TCP] send() pour client " << clientId << " a envoyé 0 octets. Socket fermée ?" << endl;
    } else if (bytesSent < dataSize) {
        cerr << "[TCP] send() pour client " << clientId << " : Seulement "
                  << bytesSent << " octets envoyés sur " << dataSize << ". (Message tronqué ou buffer plein)." << endl;
    } else {
        cout << "[TCP] Message envoyé à client " << clientId << " (" << bytesSent << " octets)." << endl;
    }
}
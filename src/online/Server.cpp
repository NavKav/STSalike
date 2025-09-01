#include "Server.h"

using namespace std;

Server::Server(int port) : _gameModel() {
    socketInitialisation();

    _udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (_udpSocket == INVALID_SOCKET) {
        ServerConsole << "Could not create UDP socket: " << getSocketError() << endl;
        exit(EXIT_FAILURE);
    }

    _tcpSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_tcpSocket == INVALID_SOCKET) {
        ServerConsole << "Could not create TCP socket: " << getSocketError() << endl;
        disconnectSocket(_udpSocket);
        exit(EXIT_FAILURE);
    }

    memset(&_server, 0, sizeof(_server));
    _server.sin_family = AF_INET;
    _server.sin_addr.s_addr = INADDR_ANY;
    _server.sin_port = htons(port);

    if (::bind(_udpSocket, (sockaddr*)&_server, sizeof(_server)) == SOCKET_ERROR) {
        ServerConsole << "Bind UDP failed with error code: " << getSocketError() << endl;
        disconnectSocket(_udpSocket);
        disconnectSocket(_tcpSocket);
        exit(EXIT_FAILURE);
    }

    if (::bind(_tcpSocket, (sockaddr*)&_server, sizeof(_server)) == SOCKET_ERROR) {
        ServerConsole << "Bind TCP failed with error code: " << getSocketError() << endl;
        disconnectSocket(_udpSocket);
        disconnectSocket(_tcpSocket);
        exit(EXIT_FAILURE);
    }

    if (::listen(_tcpSocket, SOMAXCONN) == SOCKET_ERROR) {
        ServerConsole << "Listen failed with error code: " << getSocketError() << endl;
        disconnectSocket(_udpSocket);
        disconnectSocket(_tcpSocket);
        exit(EXIT_FAILURE);
    }

    ServerConsole << "Server ready: listening on port " << port << " (TCP & UDP)" << endl;
}

Server::~Server() {
    _serverToggle = false;
    _gameModel.stop();

    if (_gameModelThread.joinable()) {
        _gameModelThread.join();
    }

    disconnectSocket(_tcpSocket);
    disconnectSocket(_udpSocket);
    cleanupSocket();
}

void Server::start() {
    _gameModelThread = thread(&GameModel::run, &_gameModel);

    runNetworkLoop();
}

void Server::runNetworkLoop() {
    fd_set readfds;
    sockaddr_in clientAddr{};
    timeval timeout{};

    while (_serverToggle) {
        _addrLen = sizeof(clientAddr);
        memset(&clientAddr, 0, _addrLen);
        _clientsToProcess.clear();

        /*************************************************************************/
        /************************* MULTIPLEXING  *********************************/
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

        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;

        int activity = select(maxSocket + 1, &readfds, nullptr, nullptr, &timeout);

        if (activity == SOCKET_ERROR) {
            ServerConsole << "select() error: " << getSocketError() << endl;
            if (!_serverToggle) break;
            continue;
        }

        if (activity == 0) {
            processOutgoingMessages();
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

        auto it = _connectedTcpClients.begin();
        while (it != _connectedTcpClients.end()) {
            SOCKET clientSock = (*it)->_tcpSocket;
            if (FD_ISSET(clientSock, &readfds)) {
                if (tcpPacketHandling(it, clientSock)) {
                    continue;
                }
            }
            ++it;
        }

        processOutgoingMessages();
    }
    ServerConsole << "[Server] Thread réseau arrêté." << endl;
}

bool Server::tcpPacketHandling(std::vector<std::unique_ptr<ClientSession>>::iterator& clientIt, SOCKET clientSock) {
    memset(_buffer, 0, sizeof(_buffer));
    int bytesReceived = recv(clientSock, _buffer, BUFFER_SIZE - 1, 0);

    if (bytesReceived > 0) {
        _buffer[bytesReceived] = '\0';

        _gameModel.addIncomingMessage(make_unique<GameMessage>(PLAYER_INPUT, (*clientIt)->_id));
        return false;
    } else {
        int clientId = (*clientIt)->_id;
        int errCode = getSocketError();

        if (bytesReceived == 0) {
            ServerConsole << "[TCP] Client ID " << clientId << " déconnecté gracieusement." << endl;
            _gameModel.addIncomingMessage(make_unique<GameMessage>(DISCONNECTION, clientId));
        } else {
            if (errCode == SOCK_ERR_WOULDBLOCK) {
            } else if (errCode == SOCK_ERR_CONNRESET) {
                ServerConsole << "[TCP] Client ID " << clientId << " déconnecté de force (Connexion réinitialisée)." << endl;
                _gameModel.addIncomingMessage(make_unique<GameMessage>(DISCONNECTION, clientId));
            } else {
                ServerConsole << "[TCP] Erreur FATALE sur socket " << clientSock << " (Client ID " << clientId << "): " << errCode << endl;
                _gameModel.addIncomingMessage(make_unique<GameMessage>(DISCONNECTION, clientId));
            }
        }

        if (errCode != SOCK_ERR_WOULDBLOCK) {
            disconnectSocket(clientSock);
            clientIt = _connectedTcpClients.erase(clientIt);
            _connectedTotal--;
            if (_connectedTotal == 0) {
                _serverToggle = false;
            }
            return true;
        }
    }
    return false;
}

void Server::udpPacketHandling(sockaddr_in& clientAddr) {
    int bytesReceived = recvfrom(_udpSocket, _buffer, BUFFER_SIZE - 1, 0, (sockaddr*)&clientAddr, &_addrLen);

    if (bytesReceived > 0) {
        _buffer[bytesReceived] = '\0';
    } else if (bytesReceived < 0) {
        ServerConsole << "[UDP] Erreur recvfrom(): " << getSocketError() << endl;
    }
}

bool Server::tcpAcceptanceHandling(sockaddr_in& clientAddr) {
    SOCKET newClientSocket = ::accept(_tcpSocket, (sockaddr*)&clientAddr, &_addrLen);

    if (newClientSocket == INVALID_SOCKET) {
        int errCode = getSocketError();
        if (errCode == SOCK_ERR_WOULDBLOCK) {
            return false;
        }
        ServerConsole << "accept() failed: " << errCode << endl;
        return false;
    }

    char ipBuffer[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &(clientAddr.sin_addr), ipBuffer, sizeof(ipBuffer)) != nullptr) {
        ServerConsole << "[TCP] Nouvelle connexion acceptée depuis "
                                     << ipBuffer << ":" << ntohs(clientAddr.sin_port) << endl;
    } else {
        ServerConsole << "[TCP] Nouvelle connexion acceptée avec une erreur de conversion IP : "
                                     << "Code d'erreur (errno) : " << getSocketError()
                                     << ". Vérifiez la famille d'adresses ou la taille du buffer." << endl;
    }

    _connectedTcpClients.push_back(make_unique<ClientSession>(
            newClientSocket, clientAddr, _idCount
    ));

    _gameModel.addIncomingMessage(make_unique<GameMessage>(CONNECTION, _idCount));

    _idCount ++;
    _connectedTotal ++;

    _connectedTcpClients.back()->displayClientInfo();

    return true;
}

void Server::processOutgoingMessages() {
    queue<unique_ptr<GameMessage>> messagesToSend = _gameModel.getAndClearOutgoingMessages();

    while (!messagesToSend.empty()) {
        auto message = *messagesToSend.front();
        messagesToSend.pop();
    }
}

void Server::sendToTcpClient(int clientId, const string& message) {
    SOCKET targetSocket = INVALID_SOCKET;

    auto it = find_if(_connectedTcpClients.begin(), _connectedTcpClients.end(),
                      [clientId](const unique_ptr<ClientSession>& clientPtr) {
                          return clientPtr->_id == clientId;
                      });

    if (it == _connectedTcpClients.end()) {
        ServerConsole << "[Server] Erreur: Client TCP avec ID " << clientId
                                     << " non trouvé ou déjà déconnecté." << endl;
        return;
    }

    targetSocket = (*it)->_tcpSocket;

    const char* data = message.c_str();
    int dataSize = message.length();

    int bytesSent = ::send(targetSocket, data, dataSize, 0);

    if (bytesSent == SOCKET_ERROR) {
        int errCode = getSocketError();
        ServerConsole << "[TCP] Erreur lors de l'envoi au client " << clientId
                                     << " (Socket: " << targetSocket << "). Code d'erreur : " << errCode << endl;
    } else if (bytesSent == 0) {
        ServerConsole << "[TCP] send() pour client " << clientId << " a envoyé 0 octets. Socket fermée ?" << endl;
    } else if (bytesSent < dataSize) {
        ServerConsole << "[TCP] send() pour client " << clientId << " : Seulement "
                                     << bytesSent << " octets envoyés sur " << dataSize << ". (Message tronqué ou buffer plein)." << endl;
    } else {
        ServerConsole << "[TCP] Message envoyé à client " << clientId << " (" << bytesSent << " octets)." << endl;
    }
}
#include "Server.h"

using namespace std;

Server::Server(int port) : _gameModel() {
    socketInitialisation();

    _udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (_udpSocket == INVALID_SOCKET) {
        serverConsole() << "Could not create UDP socket: " << getSocketError() << endl;
        exit(EXIT_FAILURE);
    }

    _tcpSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_tcpSocket == INVALID_SOCKET) {
        serverConsole() << "Could not create TCP socket: " << getSocketError() << endl;
        disconnectSocket(_udpSocket);
        exit(EXIT_FAILURE);
    }

    memset(&_server, 0, sizeof(_server));
    _server.sin_family = AF_INET;
    _server.sin_addr.s_addr = INADDR_ANY;
    _server.sin_port = htons(port);

    if (::bind(_udpSocket, (sockaddr*)&_server, sizeof(_server)) == SOCKET_ERROR) {
        serverConsole() << "Bind UDP failed with error code: " << getSocketError() << endl;
        disconnectSocket(_udpSocket);
        disconnectSocket(_tcpSocket);
        exit(EXIT_FAILURE);
    }

    if (::bind(_tcpSocket, (sockaddr*)&_server, sizeof(_server)) == SOCKET_ERROR) {
        serverConsole() << "Bind TCP failed with error code: " << getSocketError() << endl;
        disconnectSocket(_udpSocket);
        disconnectSocket(_tcpSocket);
        exit(EXIT_FAILURE);
    }

    if (::listen(_tcpSocket, SOMAXCONN) == SOCKET_ERROR) {
        serverConsole() << "Listen failed with error code: " << getSocketError() << endl;
        disconnectSocket(_udpSocket);
        disconnectSocket(_tcpSocket);
        exit(EXIT_FAILURE);
    }

    serverConsole() << "Server ready: listening on port " << port << " (TCP & UDP)" << endl;
}

Server::~Server() {
    _serverToggle.store(false);
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

void Server::stop() {
    _serverToggle.store(false);
}

void Server::runNetworkLoop() {
    fd_set readfds;
    sockaddr_in clientAddr{};
    timeval timeout{};

    while (_serverToggle.load()) {
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
            SOCKET clientSock = clientPtr.second->_tcpSocket;
            FD_SET(clientSock, &readfds);
            if (clientSock > maxSocket) {
                maxSocket = clientSock;
            }
        }

        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;

        int activity = select(maxSocket + 1, &readfds, nullptr, nullptr, &timeout);

        if (activity == SOCKET_ERROR) {
            serverConsole() << "select() error: " << getSocketError() << endl;
            if (!_serverToggle.load()) break;
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
            SOCKET clientSock = it->second->_tcpSocket;
            if (FD_ISSET(clientSock, &readfds)) {
                if (tcpPacketHandling(it)) {
                    continue;
                }
            }
            ++it;
        }

        processOutgoingMessages();
    }
    serverConsole() << "[Server] Thread réseau arrêté." << endl;
}
bool Server::tcpPacketHandling(map<int, unique_ptr<ClientSession>>::iterator& clientIt) {
    auto& clientSession = clientIt->second;
    SOCKET clientSock = clientSession->_tcpSocket;

    vector<char> tempBuffer(BUFFER_SIZE);
    int bytesReceived = recv(clientSock, tempBuffer.data(), tempBuffer.size(), 0);

    if (bytesReceived > 0) {
        auto& clientBuffer = clientSession->_incomingBuffer;
        clientBuffer.insert(clientBuffer.end(), tempBuffer.begin(), tempBuffer.begin() + bytesReceived);

        while (clientBuffer.size() >= sizeof(MessageHeader)) {
            MessageHeader header;
            memcpy(&header, clientBuffer.data(), sizeof(MessageHeader));

            if (clientBuffer.size() >= sizeof(MessageHeader) + header.size) {
                vector<char> payload(
                        clientBuffer.begin() + sizeof(MessageHeader),
                        clientBuffer.begin() + sizeof(MessageHeader) + header.size
                );

                _gameModel.addIncomingMessage(make_unique<GameMessage>(
                        header.type,
                        clientSession->_id,
                        std::move(payload)
                ));

                clientBuffer.erase(clientBuffer.begin(), clientBuffer.begin() + sizeof(MessageHeader) + header.size);
            } else {
                break;
            }
        }
    } else { // DECONNEXION
        int clientId = clientSession->_id;
        int errCode = getSocketError();

        if (bytesReceived == 0) {
            serverConsole() << "[TCP] Client ID " << clientId << " déconnecté gracieusement." << endl;
        } else if (errCode == SOCK_ERR_CONNRESET) {
            serverConsole() << "[TCP] Client ID " << clientId << " déconnecté de force (Connexion réinitialisée)." << endl;
        } else {
            serverConsole() << "[TCP] Erreur FATALE sur socket " << clientSock << " (Client ID " << clientId << "): " << errCode << endl;
        }

        if (errCode != SOCK_ERR_WOULDBLOCK) {
            _gameModel.addIncomingMessage(make_unique<GameMessage>(MessageType::DISCONNECTION, clientId, vector<char>{}));
            disconnectSocket(clientSock);
            clientIt = _connectedTcpClients.erase(clientIt);
            _connectedTotal--;
            if (_connectedTotal == 0) {
                _serverToggle.store(false);
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
        serverConsole() << "[UDP] Erreur recvfrom(): " << getSocketError() << endl;
    }
}

bool Server::tcpAcceptanceHandling(sockaddr_in& clientAddr) {
    SOCKET newClientSocket = ::accept(_tcpSocket, (sockaddr*)&clientAddr, &_addrLen);

    if (newClientSocket == INVALID_SOCKET) {
        int errCode = getSocketError();
        if (errCode == SOCK_ERR_WOULDBLOCK) {
            return false;
        }
        serverConsole() << "accept() failed: " << errCode << endl;
        return false;
    }

    char ipBuffer[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &(clientAddr.sin_addr), ipBuffer, sizeof(ipBuffer)) != nullptr) {
        serverConsole() << "[TCP] Nouvelle connexion acceptée depuis "
                                     << ipBuffer << ":" << ntohs(clientAddr.sin_port) << endl;
    } else {
        serverConsole() << "[TCP] Nouvelle connexion acceptée avec une erreur de conversion IP : "
                                     << "Code d'erreur (errno) : " << getSocketError()
                                     << ". Vérifiez la famille d'adresses ou la taille du buffer." << endl;
    }

    _connectedTcpClients.emplace(_idCount,
                                 make_unique<ClientSession>(
                                         newClientSocket, clientAddr, _idCount
                                 ));

    vector<char> v;

    _gameModel.addIncomingMessage(make_unique<GameMessage>(MessageType::CONNECTION, _idCount, std::move(v)));

    _connectedTcpClients.at(_idCount)->displayClientInfo();

    _idCount ++;
    _connectedTotal ++;

    return true;
}

void Server::processOutgoingMessages() {
    queue<pair<int, vector<char>>> messagesToSend;
    _gameModel.getAndClearOutgoingMessages(messagesToSend);

    while (!messagesToSend.empty()) {
        auto message = messagesToSend.front();
        messagesToSend.pop();
        switch(message.first) {
            case -1:
                break;
            default :
                sendToTcpClient(message.first, message.second);
                break;
        }
    }
}

void Server::sendToTcpClient(int clientId, const vector<char>& buffer) {
    auto it = _connectedTcpClients.find(clientId);
    if (it == _connectedTcpClients.end()) {
        serverConsole() << "[Server] Erreur: Client TCP avec ID " << clientId
                      << " non trouvé ou déjà déconnecté." << endl;
        return;
    }

    SOCKET targetSocket = it->second->_tcpSocket;
    const char* data = buffer.data();
    int dataSize = static_cast<int>(buffer.size());
    int totalBytesSent = 0;

    while (totalBytesSent < dataSize) {
        int bytesSent = ::send(targetSocket, data + totalBytesSent, dataSize - totalBytesSent, 0);

        if (bytesSent == SOCKET_ERROR) {
            int errCode = getSocketError();
            if (errCode == SOCK_ERR_WOULDBLOCK) {
                this_thread::sleep_for(chrono::milliseconds(1));
                continue;
            }

            serverConsole() << "[TCP] Erreur lors de l'envoi au client " << clientId
                          << ". Code d'erreur : " << errCode << endl;
            return;
        }

        if (bytesSent == 0) {
            serverConsole() << "[TCP] send() pour client " << clientId << " a envoyé 0 octets. Socket fermée ?" << endl;
            return;
        }

        totalBytesSent += bytesSent;
    }

    serverConsole() << "[TCP] Message envoyé à client " << clientId << " (" << totalBytesSent << " octets)." << endl;
}
//
// Created by NavKav on 24/03/2024.
//

#include "Client.h"

using namespace std;

Client::Client(int port, const string& ip) {
    socketInitialisation();

    // Création du socket UDP
    _udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (_udpSocket == INVALID_SOCKET) {
        cerr << "Échec création socket UDP : " << getSocketError() << endl;
        exit(EXIT_FAILURE);
    }

    // Création du socket TCP
    _tcpSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_tcpSocket == INVALID_SOCKET) {
        cerr << "Échec création socket TCP : " << getSocketError() << endl;
        disconnectSocket(_udpSocket);
        exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse du serveur (même pour TCP et UDP)
    memset(&_server, 0, sizeof(_server));
    _server.sin_family = AF_INET;
    _server.sin_port = htons(port);
    if (inet_pton(AF_INET, ip.c_str(), &(_server.sin_addr)) <= 0) {
        cerr << "Adresse IP invalide ou non supportée : " << ip << endl;
        disconnectSocket(_udpSocket);
        disconnectSocket(_tcpSocket);
        exit(EXIT_FAILURE);
    }

    // Connexion TCP
    int attempts = 0;
    while (attempts < 5) {
        if (connect(_tcpSocket, (sockaddr*)&_server, sizeof(_server)) != SOCKET_ERROR) {
            setNonBlocking(_tcpSocket);
            break; // Succès
        }

        int err = getSocketError();
        if (err != 0) {
            cout << "Connexion refusée, nouvelle tentative..." << endl;
            this_thread::sleep_for(chrono::milliseconds(500));
            attempts++;
        } else {
            cerr << "Erreur connect(): " << err << endl;
            exit(EXIT_FAILURE);
        }
    }

    if (attempts == 5) {
        cerr << "Impossible de se connecter après plusieurs tentatives." << endl;
        exit(EXIT_FAILURE);
    }
}

Client::~Client() {
    disconnectSocket(_tcpSocket);
    disconnectSocket(_udpSocket);
}


void Client::startNetworkLoop() {
    _networkThread = std::thread(&Client::networkLoop, this);
}

void Client::stopNetworkLoop() {
    _isRunning = false;
    if (_networkThread.joinable()) {
        _networkThread.join();
    }
}

void Client::networkLoop() {
    while (_isRunning) {
        std::vector<std::unique_ptr<GameMessage>> msgs = receiveTCP();

        if (!msgs.empty()) {
            std::lock_guard<std::mutex> lock(_messageMutex);
            for (auto& msg : msgs) {
                _messageQueue.push(std::move(msg));
            }
        }
    }
}

void Client::processAllMessages(std::queue<std::unique_ptr<GameMessage>>& destinationQueue) {
    std::lock_guard<std::mutex> lock(_messageMutex);

    _messageQueue.swap(destinationQueue);
}

// sendUDP outdated/deprecated par rapport aux GameMessage
void Client::sendUDP(const string& s) {
    const char* message = s.c_str();
    int msgLen = static_cast<int>(strlen(message));
    int slen = sizeof(_server);

    while (sendto(_udpSocket, message, msgLen, 0, (sockaddr*)&_server, slen) == SOCKET_ERROR) {
        cout << "sendto() failed with error code: " << getSocketError() << endl;
        exit(EXIT_FAILURE);
    }
}

// Retour par valeur optimisé grâce au RVO (et NRVO)
std::vector<std::unique_ptr<GameMessage>> Client::receiveTCP() {
    std::vector<char> tempBuffer(CLIENT_BUFFER_SIZE);
    int bytesReceived = ::recv(_tcpSocket, tempBuffer.data(), CLIENT_BUFFER_SIZE, 0);

    // Traitement de la réception
    if (bytesReceived > 0) {
        _incomingBuffer.insert(_incomingBuffer.end(), tempBuffer.begin(), tempBuffer.begin() + bytesReceived);
    } else {
        int errCode = getSocketError();
        if (bytesReceived == 0) {
            std::cout << "[Client TCP] Serveur déconnecté" << std::endl;
        } else if (errCode != SOCK_ERR_WOULDBLOCK && errCode != SOCK_ERR_INTR) {
            std::cerr << "[Client TCP] Erreur grave lors de la réception du serveur. Déconnexion. Code: " << errCode << std::endl;
        }
        if (errCode != SOCK_ERR_WOULDBLOCK && errCode != SOCK_ERR_INTR) {
            disconnectSocket(_tcpSocket);
            _tcpSocket = INVALID_SOCKET;
        }
    }

    std::vector<std::unique_ptr<GameMessage>> messages;

    while (_incomingBuffer.size() >= sizeof(MessageHeader)) {
        MessageHeader header;
        memcpy(&header, _incomingBuffer.data(), sizeof(MessageHeader));
        uint32_t messageSize = ntohl(header.size);

        if (_incomingBuffer.size() >= sizeof(MessageHeader) + messageSize) {
            std::vector<char> payload(
                _incomingBuffer.begin() + sizeof(MessageHeader),
                _incomingBuffer.begin() + sizeof(MessageHeader) + messageSize
            );

            _incomingBuffer.erase(_incomingBuffer.begin(), _incomingBuffer.begin() + sizeof(MessageHeader) + messageSize);

            messages.push_back(std::make_unique<GameMessage>(static_cast<MessageType>(header.type), -1, std::move(payload)));
        } else {
            break;
        }
    }

    return messages;
}

void Client::sendTCP(const std::vector<char>& serializedMessage) {
    int totalSent = 0;
    while (totalSent < serializedMessage.size()) {
        int sent = send(_tcpSocket, serializedMessage.data() + totalSent, serializedMessage.size() - totalSent, 0);
        if (sent == SOCKET_ERROR) {
            std::cout << "send() failed with error code: " << getSocketError() << std::endl;
            exit(EXIT_FAILURE);
        }
        totalSent += sent;
    }
}

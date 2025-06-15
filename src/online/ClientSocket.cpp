//
// Created by NavKav on 25/05/2025.
//

#include "ClientSocket.h"

ClientSocket::ClientSocket(SOCKET tcpSocket, const sockaddr_in& tcpAddr, int id) :
_tcpSocket(tcpSocket),
_tcpAddr(tcpAddr),
_id(id) {

}

void ClientSocket::displayClientInfo() const {
    std::cout << "Client ID: " << _id << std::endl;
    std::cout << "  TCP Socket: " << _tcpSocket << std::endl;

    // --- Correction pour la portabilité de l'affichage de l'adresse IP ---
    // inet_ntoa est dépréciée et non thread-safe. Utiliser inet_ntop.
    char ipBuffer[INET_ADDRSTRLEN]; // INET_ADDRSTRLEN est défini dans <arpa/inet.h> (POSIX) ou <ws2tcpip.h> (Windows)
    if (inet_ntop(AF_INET, &(_tcpAddr.sin_addr), ipBuffer, sizeof(ipBuffer)) != nullptr) {
        std::cout << "  TCP IP: " << ipBuffer << ":" << ntohs(_tcpAddr.sin_port) << std::endl;
    } else {
        std::cerr << "  Erreur lors de la conversion de l'adresse IP TCP." << std::endl;
    }
}
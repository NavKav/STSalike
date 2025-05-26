//
// Created by NavKav on 25/05/2025.
//

#include "ClientSocket.h"

ClientSocket::ClientSocket(SOCKET tcpSocket, const sockaddr_in& tcpAddr, SOCKET udpSocket, const sockaddr_in& udpAddr, const std::string& name) : _tcpSocket(tcpSocket),
_tcpAddr(tcpAddr),
_udpSocket(udpSocket),
_udpAddr(udpAddr),
_name("") {

}

void ClientSocket::displayClientInfo() const {
    std::cout << "Client Name: " << _name << std::endl;
    std::cout << "  TCP Socket: " << _tcpSocket << std::endl;
    std::cout << "  TCP IP: " << inet_ntoa(_tcpAddr.sin_addr) << ":" << ntohs(_tcpAddr.sin_port) << std::endl;
    std::cout << "  UDP Socket: " << _udpSocket << std::endl;
    std::cout << "  UDP IP: " << inet_ntoa(_udpAddr.sin_addr) << ":" << ntohs(_udpAddr.sin_port) << std::endl;
}
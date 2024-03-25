#include "TCP/Server.h"

#include <iostream>

namespace MB::TCP {

Server::Server(int port)
{
#ifdef _WIN32
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        throw std::runtime_error("Failed to initialize Winsock");
    }
#endif

    _port = port;

    _serverfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_serverfd == -1) {
        throw std::runtime_error("Cannot create socket");
    }

    _server.sin_family = AF_INET;
    _server.sin_port = htons(_port);
    _server.sin_addr.s_addr = INADDR_ANY;

    if (bind(_serverfd, (struct sockaddr *)&_server, sizeof(_server)) < 0) {
        throw std::runtime_error("Cannot bind socket");
    }

    if (listen(_serverfd, SOMAXCONN) == -1) {
        throw std::runtime_error("Error listening on socket");
    }
}

Server::~Server()
{
    if (_serverfd >= 0) {
#ifdef _WIN32
        closesocket(_serverfd);
#else
        close(_serverfd);
#endif
    }

    _serverfd = -1;

#ifdef _WIN32
    WSACleanup();
#endif
}

std::optional<Connection> Server::await_connection() const
{
    sockaddr_in client_address;

#ifdef _WIN32
    int client_len = sizeof(client_address);
    const SOCKET client_sock = accept(_serverfd, (struct sockaddr *)&client_address, &client_len);
#else
    socklen_t client_len = sizeof(client_address);
    const int client_sock = accept(_serverfd, (struct sockaddr *)&client_address, &client_len);
#endif

    if (client_sock == -1) {
        throw std::runtime_error("Error accepting connection");
    }

    std::cout << "Accepted connection from: " << inet_ntoa(client_address.sin_addr) << '\n';

    return Connection(client_sock);
}

}  // namespace MB::TCP

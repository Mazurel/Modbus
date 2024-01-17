#include "TCP/Server.hpp"

#include <iostream>

namespace MB::TCP {

Server::Server(int port)
{
#ifdef _WIN32
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data)) {
        std::cerr << "WinSock cannot be initialized\n";
        WSACleanup();
        return;
    }
#endif

    _port = port;
    _serverfd = socket(AF_INET, SOCK_STREAM, 0);

    if (_serverfd == -1) {
        throw std::runtime_error("Cannot create socket");
    }

    setsockopt(_serverfd, SOL_SOCKET, SO_REUSEADDR, (const char *)new int(1), sizeof(int));

#ifdef _WIN32
    setsockopt(_serverfd, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (const char *)new int(1), sizeof(int));
#else
    setsockopt(_serverfd, SOL_SOCKET, SO_REUSEPORT, new int(1), sizeof(int));
#endif

    _server = {};

    _server.sin_family = AF_INET;
    _server.sin_addr.s_addr = INADDR_ANY;
    _server.sin_port = ::htons(_port);

    if (::bind(_serverfd, reinterpret_cast<struct sockaddr *>(&_server), sizeof(_server)) < 0) {
        throw std::runtime_error("Cannot bind socket");
    }

    ::listen(_serverfd, 255);
}

Server::~Server()
{
    if (_serverfd >= 0) {
#ifdef _WIN32
        closesocket(_serverfd);
#else
        ::close(_serverfd);
#endif
    }

    _serverfd = -1;
}

std::optional<Connection> Server::await_connection()
{
#ifdef _WIN32
    int addr_len = sizeof(_server);
    auto connfd = accept(_serverfd, reinterpret_cast<struct sockaddr *>(&_server), &addr_len);
#else
    socklen_t addr_len = sizeof(_server);
    auto connfd = ::accept(_serverfd, reinterpret_cast<struct sockaddr *>(&_server), &addr_len);
#endif

    if (connfd < 0) {
        throw;
    }

    std::cout << "Server accepts client connection request successfully!\n";

    return Connection(connfd);
}

}  // namespace MB::TCP

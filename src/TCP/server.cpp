// Modbus for c++ <https://github.com/Mazurel/Modbus>
// Copyright (c) 2020 Mateusz Mazur aka Mazurel
// Licensed under: MIT License <http://opensource.org/licenses/MIT>

#include "TCP/server.hpp"

using namespace MB::TCP;

Server::Server(int port) {
    _port     = port;
    _serverfd = socket(AF_INET, SOCK_STREAM, 0);

    if (_serverfd == -1)
        throw std::runtime_error("Cannot create socket");

    setsockopt(_serverfd, SOL_SOCKET, SO_REUSEADDR, new int(1), sizeof(int));
    setsockopt(_serverfd, SOL_SOCKET, SO_REUSEPORT, new int(1), sizeof(int));

    _server = {};

    _server.sin_family      = AF_INET;
    _server.sin_addr.s_addr = INADDR_ANY;
    _server.sin_port        = ::htons(_port);

    if (::bind(_serverfd, reinterpret_cast<struct sockaddr *>(&_server),
               sizeof(_server)) < 0)
        throw std::runtime_error("Cannot bind socket");

    ::listen(_serverfd, 255);
}

Server::~Server() {
    if (_serverfd >= 0)
        ::close(_serverfd);

    _serverfd = -1;
}

std::optional<Connection> Server::awaitConnection() {
    socklen_t addrLen = sizeof(_server);

    auto connfd =
        ::accept(_serverfd, reinterpret_cast<struct sockaddr *>(&_server), &addrLen);

    if (connfd < 0)
        throw;

    return Connection(connfd);
}

// Modbus for c++ <https://github.com/Mazurel/Modbus>
// Copyright (c) 2020 Mateusz Mazur aka Mazurel
// Licensed under: MIT License <http://opensource.org/licenses/MIT>

#pragma once

#include <string>
#include <optional>
#include <stdexcept>

#include <sys/socket.h>
#include <netinet/in.h>
#include <libnet.h>

#include "connection.hpp"

namespace MB::TCP
{
class Server
{
private:
    int _serverfd;
    int _port;
    sockaddr_in _server;

public:
    explicit Server(int port);
    ~Server();

    Server(const Server &) = delete;
    Server(Server && moved)
    {
        _serverfd = moved._serverfd;
        _port = moved._port;
        moved._serverfd = -1;
    }
    Server& operator=(Server && moved)
    {
        if (this == &moved)
            return *this;

        _serverfd = moved._serverfd;
        _port = moved._port;
        moved._serverfd = -1;
        return *this;
    }

    [[nodiscard]] int nativeHandle() { return _serverfd; }

    std::optional<Connection> awaitConnection();
};
}

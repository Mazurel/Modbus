// Modbus for c++ <https://github.com/Mazurel/Modbus>
// Copyright (c) 2020 Mateusz Mazur aka Mazurel
// Licensed under: MIT License <http://opensource.org/licenses/MIT>

#pragma once

#include <optional>
#include <stdexcept>
#include <string>

#ifdef _WIN32
#include <Winsock2.h>
#include <Ws2tcpip.h>
#define poll(a, b, c)  WSAPoll((a), (b), (c))
#else
#include <libnet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif

#include "connection.hpp"

namespace MB {
namespace TCP {
class Server {
private:
  int _serverfd;
  int _port;
  sockaddr_in _server;

public:
  explicit Server(int port);
  ~Server();

  Server(const Server &) = delete;
  Server(Server &&moved) {
    _serverfd = moved._serverfd;
    _port = moved._port;
    moved._serverfd = -1;
  }
  Server &operator=(Server &&moved) {
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
}} // namespace MB::TCP

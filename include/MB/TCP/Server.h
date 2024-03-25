#pragma once

#include <optional>
#include <stdexcept>
#include <string>

#ifdef _WIN32
#    include <winsock2.h>
#    pragma comment(lib, "ws2_32.lib")
#else
#    include <netinet/in.h>
#    include <sys/socket.h>
#endif

#include "Connection.h"
#include "Export.h"

namespace MB::TCP {

class MODBUS_EXPORT Server
{
  public:
    explicit Server(int port);
    Server(const Server &) = delete;
    Server(Server &&moved) noexcept
    {
        _serverfd = moved._serverfd;
        _port = moved._port;
        moved._serverfd = -1;
    }

    ~Server();

    Server &operator=(Server &&moved) noexcept
    {
        if (this == &moved) {
            return *this;
        }

        _serverfd = moved._serverfd;
        _port = moved._port;
        moved._serverfd = -1;
        return *this;
    }

    [[nodiscard]] int native_handle() const { return (int)_serverfd; }

    [[nodiscard]] std::optional<Connection> await_connection() const;

  private:
#ifdef _WIN32
    SOCKET _serverfd;
#else
    int _serverfd;
#endif

    int _port;
    sockaddr_in _server;
};

}  // namespace MB::TCP

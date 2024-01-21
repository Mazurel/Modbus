#pragma once

#include <cerrno>
#include <memory>
#include <type_traits>

#ifdef _WIN32
#    include <winsock2.h>
#    include <ws2tcpip.h>
#else
#    include <arpa/inet.h>
#    include <fcntl.h>
#    include <netinet/in.h>
#    include <poll.h>
#    include <sys/select.h>
#    include <sys/socket.h>
#    include <unistd.h>
#endif

#include "Export.h"
#include "MB/ModbusException.h"
#include "MB/ModbusRequest.h"
#include "MB/ModbusResponse.h"

namespace MB::TCP {

class MODBUS_EXPORT Connection
{
  public:
    static const unsigned int default_tcp_timeout = 500;

  public:
    explicit Connection() noexcept = default;
    explicit Connection(uint16_t sockfd) noexcept;
    Connection(const Connection &copy) = delete;
    Connection(Connection &&moved) noexcept;

    ~Connection();

    Connection &operator=(Connection &&other) noexcept
    {
        if (this == &other) {
            return *this;
        }

        if (_sockfd != -1 && _sockfd != other._sockfd) {
#ifdef _WIN32
            closesocket(_sockfd);
#else
            close(_sockfd);
#endif
        }

        _sockfd = other._sockfd;
        _message_id = other._message_id;
        other._sockfd = -1;

        return *this;
    }

    [[nodiscard]] uint64_t get_sockfd() const { return _sockfd; }

    static Connection with(const std::string &addr, int port);

    std::vector<uint8_t> send_request(const MB::ModbusRequest &req);
    std::vector<uint8_t> send_response(const MB::ModbusResponse &res);
    std::vector<uint8_t> send_exception(const MB::ModbusException &ex);

    [[nodiscard]] MB::ModbusRequest await_request();
    [[nodiscard]] MB::ModbusResponse await_response();

    [[nodiscard]] std::vector<uint8_t> await_raw_message();

    [[nodiscard]] uint16_t get_message_id() const { return _message_id; }

    void set_message_id(uint16_t message_id) { _message_id = message_id; }

  private:
#ifdef _WIN32
    SOCKET _sockfd;
#else
    int _sockfd;
#endif

    uint16_t _message_id{0};
    int _timeout = Connection::default_tcp_timeout;
};

}  // namespace MB::TCP

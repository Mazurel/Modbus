#include "TCP/Connection.h"

namespace MB::TCP {

Connection::Connection(uint16_t sockfd) noexcept
{
    _sockfd = sockfd;
    _message_id = 0;
}

Connection::~Connection()
{
    if (_sockfd == -1) {
        return;
    }

#ifdef _WIN32
    closesocket(_sockfd);
#else
    close(_sockfd);
#endif
    _sockfd = (uint64_t)(-1);
}

std::vector<uint8_t> Connection::send_request(const MB::ModbusRequest &req)
{
    std::vector<uint8_t> raw_req;
    raw_req.reserve(6);

    Utils::push_uint16(raw_req, _message_id);
    raw_req.push_back(0x00);
    raw_req.push_back(0x00);

    std::vector<uint8_t> dat = req.to_raw();

    Utils::push_uint16(raw_req, (uint16_t)dat.size());

    raw_req.insert(raw_req.end(), dat.begin(), dat.end());

    send(_sockfd, (const char *)raw_req.data(), (int)raw_req.size(), 0);

    return raw_req;
}

std::vector<uint8_t> Connection::send_response(const MB::ModbusResponse &res)
{
    std::vector<uint8_t> raw_req;
    raw_req.reserve(6);

    Utils::push_uint16(raw_req, _message_id);
    raw_req.push_back(0x00);
    raw_req.push_back(0x00);

    std::vector<uint8_t> dat = res.to_raw();

    Utils::push_uint16(raw_req, (uint16_t)dat.size());

    raw_req.insert(raw_req.end(), dat.begin(), dat.end());

    send(_sockfd, (const char *)raw_req.data(), (int)raw_req.size(), 0);

    return raw_req;
}

std::vector<uint8_t> Connection::send_exception(const MB::ModbusException &ex)
{
    std::vector<uint8_t> raw_req;
    raw_req.reserve(6);

    Utils::push_uint16(raw_req, _message_id);
    raw_req.push_back(0x00);
    raw_req.push_back(0x00);

    std::vector<uint8_t> dat = ex.to_raw();

    Utils::push_uint16(raw_req, (uint16_t)dat.size());

    raw_req.insert(raw_req.end(), dat.begin(), dat.end());

    send(_sockfd, (const char *)raw_req.data(), (int)raw_req.size(), 0);

    return raw_req;
}

std::vector<uint8_t> Connection::await_raw_message()
{
    pollfd pfd = {_sockfd, POLLIN, POLLIN};
#ifdef _WIN32
    if (WSAPoll(&pfd, 1, 60 * 1000 /* 1 minute means the connection has died */) <= 0) {
        throw MB::ModbusException(MB::Utils::Timeout);
    }
#else
    if (poll(&pfd, 1, 60 * 1000 /* 1 minute means the connection has died */) <= 0) {
        throw MB::ModbusException(MB::Utils::Timeout);
    }
#endif

    std::vector<uint8_t> r(1024);

    auto size = recv(_sockfd, (char *)r.data(), (int)r.size(), 0);

    if (size == 0) {
        throw MB::ModbusException(MB::Utils::ConnectionClosed);
    }
    if (size == -1) {
        throw MB::ModbusException(MB::Utils::ProtocolError);
    }

    r.resize(size);  // Set vector to proper shape
    r.shrink_to_fit();

    return r;
}

MB::ModbusRequest Connection::await_request()
{
    pollfd pfd = {_sockfd, POLLIN, POLLIN};
#ifdef _WIN32
    if (WSAPoll(&pfd, 1, 60 * 1000 /* 1 minute means the connection has died */) <= 0) {
        throw MB::ModbusException(MB::Utils::Timeout);
    }
#else
    if (poll(&pfd, 1, 60 * 1000 /* 1 minute means the connection has died */) <= 0) {
        throw MB::ModbusException(MB::Utils::Timeout);
    }
#endif

    std::vector<uint8_t> r(1024);

    auto size = recv(_sockfd, (char *)r.data(), (int)r.size(), 0);

    if (size == 0) {
        throw MB::ModbusException(MB::Utils::ConnectionClosed);
    }
    if (size == -1) {
        throw MB::ModbusException(MB::Utils::ProtocolError);
    }

    r.resize(size);  // Set vector to proper shape
    r.shrink_to_fit();

    const uint16_t result_message_id = (uint16_t)r[0] << 8 | r[1];

    _message_id = result_message_id;

    r.erase(r.begin(), r.begin() + 6);

    return MB::ModbusRequest::from_raw(r);
}

MB::ModbusResponse Connection::await_response()
{
    pollfd pfd = {_sockfd, POLLIN, POLLIN};
#ifdef _WIN32
    if (WSAPoll(&pfd, 1, 60 * 1000 /* 1 minute means the connection has died */) <= 0) {
        throw MB::ModbusException(MB::Utils::Timeout);
    }
#else
    if (poll(&pfd, 1, 60 * 1000 /* 1 minute means the connection has died */) <= 0) {
        throw MB::ModbusException(MB::Utils::Timeout);
    }
#endif

    std::vector<uint8_t> r(1024);

    auto size = recv(_sockfd, (char *)r.data(), (int)r.size(), 0);

    if (size == 0) {
        throw MB::ModbusException(MB::Utils::ConnectionClosed);
    }
    if (size == -1) {
        throw MB::ModbusException(MB::Utils::ProtocolError);
    }

    r.resize(size);  // Set vector to proper shape
    r.shrink_to_fit();

    const uint16_t result_message_id = (uint16_t)r[0] << 8 | r[1];

    if (result_message_id != _message_id) {
        throw MB::ModbusException(MB::Utils::InvalidMessageID);
    }

    r.erase(r.begin(), r.begin() + 6);

    if (MB::ModbusException::exist(r)) {
        throw MB::ModbusException(r);
    }

    return MB::ModbusResponse::from_raw(r);
}

Connection::Connection(Connection &&moved) noexcept
{
    if (_sockfd != -1 && moved._sockfd != _sockfd) {
#ifdef _WIN32
        closesocket(_sockfd);
#else
        ::close(_sockfd);
#endif
    }

    _sockfd = moved._sockfd;
    _message_id = moved._message_id;
    moved._sockfd = -1;
}

Connection Connection::with(const std::string &addr, int port)
{
    auto sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        throw std::runtime_error("Cannot open socket, errno = " + std::to_string(errno));
    }

    sockaddr_in server = {AF_INET, htons(port), {(unsigned char)inet_addr(addr.c_str())}, {}};

    if (connect(sock, reinterpret_cast<struct sockaddr *>(&server), sizeof(server)) < 0) {
        throw std::runtime_error("Cannot connect, errno = " + std::to_string(errno));
    }

    return Connection((uint64_t)sock);
}

}  // namespace MB::TCP

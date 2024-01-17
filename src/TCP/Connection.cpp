#include "TCP/Connection.hpp"

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
    ::close(_sockfd);
#endif
    _sockfd = (uint64_t)(-1);
}

std::vector<uint8_t> Connection::send_request(const MB::ModbusRequest &req)
{
    std::vector<uint8_t> raw_req;
    raw_req.reserve(6);

    raw_req.push_back(reinterpret_cast<const uint8_t *>(&_message_id)[1]);
    raw_req.push_back(reinterpret_cast<const uint8_t *>(&_message_id)[0]);
    raw_req.push_back(0x00);
    raw_req.push_back(0x00);

    std::vector<uint8_t> dat = req.to_raw();

    size_t size = dat.size();
    raw_req.push_back(reinterpret_cast<uint16_t *>(&size)[1]);
    raw_req.push_back(reinterpret_cast<uint16_t *>(&size)[0]);

    raw_req.insert(raw_req.end(), dat.begin(), dat.end());

#ifdef _WIN32
    send(_sockfd, (const char *)raw_req.data(), (int)raw_req.size(), 0);
#else
    ::send(_sockfd, rawReq.data(), rawReq.size(), 0);
#endif

    return raw_req;
}

std::vector<uint8_t> Connection::send_response(const MB::ModbusResponse &res)
{
    std::vector<uint8_t> raw_req;
    raw_req.reserve(6);

    raw_req.push_back(reinterpret_cast<const uint8_t *>(&_message_id)[1]);
    raw_req.push_back(reinterpret_cast<const uint8_t *>(&_message_id)[0]);
    raw_req.push_back(0x00);
    raw_req.push_back(0x00);

    std::vector<uint8_t> dat = res.to_raw();

    size_t size = dat.size();
    raw_req.push_back(reinterpret_cast<uint16_t *>(&size)[1]);
    raw_req.push_back(reinterpret_cast<uint16_t *>(&size)[0]);

    raw_req.insert(raw_req.end(), dat.begin(), dat.end());

#ifdef _WIN32
    send(_sockfd, (const char *)raw_req.data(), (int)raw_req.size(), 0);
#else
    ::send(_sockfd, rawReq.data(), rawReq.size(), 0);
#endif

    return raw_req;
}

std::vector<uint8_t> Connection::send_exception(const MB::ModbusException &ex)
{
    std::vector<uint8_t> raw_req;
    raw_req.reserve(6);

    raw_req.push_back(reinterpret_cast<const uint8_t *>(&_message_id)[1]);
    raw_req.push_back(reinterpret_cast<const uint8_t *>(&_message_id)[0]);
    raw_req.push_back(0x00);
    raw_req.push_back(0x00);

    std::vector<uint8_t> dat = ex.to_raw();

    size_t size = dat.size();
    raw_req.push_back(reinterpret_cast<uint16_t *>(&size)[1]);
    raw_req.push_back(reinterpret_cast<uint16_t *>(&size)[0]);

    raw_req.insert(raw_req.end(), dat.begin(), dat.end());

#ifdef _WIN32
    send(_sockfd, (const char *)raw_req.data(), (int)raw_req.size(), 0);
#else
    ::send(_sockfd, rawReq.data(), rawReq.size(), 0);
#endif

    return raw_req;
}

std::vector<uint8_t> Connection::await_raw_message()
{
    pollfd pfd = {_sockfd, POLLIN, POLLIN};
    if (
#ifdef _WIN32
        WSAPoll(&pfd, 1, 60 * 1000 /* 1 minute means the connection has died */) <= 0
#else
        ::poll(&_pfd, 1, 60 * 1000 /* 1 minute means the connection has died */) <= 0
#endif
    ) {
        throw MB::ModbusException(MB::Utils::ConnectionClosed);
    }

    std::vector<uint8_t> r(1024);

#ifdef _WIN32
    auto size = recv(_sockfd, (char *)r.data(), (int)r.size(), 0);
#else
    auto size = ::recv(_sockfd, r.data(), r.size(), 0);
#endif

    if (size == -1) {
        throw MB::ModbusException(MB::Utils::ProtocolError);
    }
    if (size == 0) {
        throw MB::ModbusException(MB::Utils::ConnectionClosed);
    }

    r.resize(size);  // Set vector to proper shape
    r.shrink_to_fit();

    return r;
}

MB::ModbusRequest Connection::await_request()
{
#ifdef _WIN32
//  WSAPoll(&pfd, 1, 60 * 1000 /* 1 minute means the connection has died */) <= 0
#else
    pollfd pfd = {_sockfd, POLLIN, POLLIN};
    if (::poll(&_pfd, 1, 60 * 1000 /* 1 minute means the connection has died */) <= 0) {
        throw MB::ModbusException(MB::Utils::Timeout);
    }
#endif

    std::vector<uint8_t> r;
    r.reserve(1024);

#ifdef _WIN32
    auto size = recv(_sockfd, (char *)r.data(), (int)r.size(), 0);
#else
    auto size = ::recv(_sockfd, r.data(), r.size(), 0);
#endif

    if (size == -1) {
        throw MB::ModbusException(MB::Utils::ProtocolError);
    }
    if (size == 0) {
        throw MB::ModbusException(MB::Utils::ConnectionClosed);
    }

    r.resize(size);  // Set vector to proper shape
    r.shrink_to_fit();

    const auto result_message_id = *reinterpret_cast<uint16_t *>(&r[0]);

    _message_id = result_message_id;

    r.erase(r.begin(), r.begin() + 6);

    return MB::ModbusRequest::from_raw(r);
}

MB::ModbusResponse Connection::await_response()
{
#ifdef _WIN32
//  WSAPoll(&pfd, 1, 60 * 1000 /* 1 minute means the connection has died */) <= 0
#else
    pollfd pfd = {_sockfd, POLLIN, POLLIN};
    if (::poll(&_pfd, 1, 60 * 1000 /* 1 minute means the connection has died */) <= 0) {
        throw MB::ModbusException(MB::Utils::Timeout);
    }
#endif

    std::vector<uint8_t> r(1024);

#ifdef _WIN32
    auto size = recv(_sockfd, (char *)r.data(), (int)r.size(), 0);
#else
    auto size = ::recv(_sockfd, r.data(), r.size(), 0);
#endif

    if (size == -1) {
        throw MB::ModbusException(MB::Utils::ProtocolError);
    }
    if (size == 0) {
        throw MB::ModbusException(MB::Utils::ConnectionClosed);
    }

    r.resize(size);  // Set vector to proper shape
    r.shrink_to_fit();

    const auto result_message_id = *reinterpret_cast<uint16_t *>(&r[0]);

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

Connection Connection::with(std::string addr, int port)
{
    auto sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        throw std::runtime_error("Cannot open socket, errno = " + std::to_string(errno));
    }

    sockaddr_in server = {AF_INET, htons(port), {(unsigned char)inet_addr(addr.c_str())}, {}};

    if (::connect(sock, reinterpret_cast<struct sockaddr *>(&server), sizeof(server)) < 0) {
        throw std::runtime_error("Cannot connect, errno = " + std::to_string(errno));
    }

    return Connection((uint64_t)sock);
}

}  // namespace MB::TCP

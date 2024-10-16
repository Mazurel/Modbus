// Modbus for c++ <https://github.com/Mazurel/Modbus>
// Copyright (c) 2020 Mateusz Mazur aka Mazurel
// Licensed under: MIT License <http://opensource.org/licenses/MIT>

#include "TCP/connection.hpp"
#include <sys/poll.h>
#include <sys/socket.h>

using namespace MB::TCP;

Connection::Connection(const int sockfd) noexcept {
    _sockfd    = sockfd;
    _messageID = 0;
}

Connection::~Connection() {
    if (_sockfd == -1)
        return;

    ::close(_sockfd);
    _sockfd = -1;
}

std::vector<uint8_t> Connection::sendRequest(const MB::ModbusRequest &req) {
    std::vector<uint8_t> rawReq;
    rawReq.reserve(6);

    rawReq.push_back(reinterpret_cast<const uint8_t *>(&_messageID)[1]);
    rawReq.push_back(reinterpret_cast<const uint8_t *>(&_messageID)[0]);
    rawReq.push_back(0x00);
    rawReq.push_back(0x00);

    std::vector<uint8_t> dat = req.toRaw();

    uint32_t size = dat.size();
    rawReq.push_back(reinterpret_cast<uint16_t *>(&size)[1]);
    rawReq.push_back(reinterpret_cast<uint16_t *>(&size)[0]);

    rawReq.insert(rawReq.end(), dat.begin(), dat.end());

    ::send(_sockfd, rawReq.begin().base(), rawReq.size(), 0);

    return rawReq;
}

std::vector<uint8_t> Connection::sendResponse(const MB::ModbusResponse &res) {
    std::vector<uint8_t> rawReq;
    rawReq.reserve(6);

    rawReq.push_back(reinterpret_cast<const uint8_t *>(&_messageID)[1]);
    rawReq.push_back(reinterpret_cast<const uint8_t *>(&_messageID)[0]);
    rawReq.push_back(0x00);
    rawReq.push_back(0x00);

    std::vector<uint8_t> dat = res.toRaw();

    uint32_t size = dat.size();
    rawReq.push_back(reinterpret_cast<uint16_t *>(&size)[1]);
    rawReq.push_back(reinterpret_cast<uint16_t *>(&size)[0]);

    rawReq.insert(rawReq.end(), dat.begin(), dat.end());

    ::send(_sockfd, rawReq.begin().base(), rawReq.size(), 0);

    return rawReq;
}

std::vector<uint8_t> Connection::sendException(const MB::ModbusException &ex) {
    std::vector<uint8_t> rawReq;
    rawReq.reserve(6);

    rawReq.push_back(reinterpret_cast<const uint8_t *>(&_messageID)[1]);
    rawReq.push_back(reinterpret_cast<const uint8_t *>(&_messageID)[0]);
    rawReq.push_back(0x00);
    rawReq.push_back(0x00);

    std::vector<uint8_t> dat = ex.toRaw();

    uint32_t size = dat.size();
    rawReq.push_back(reinterpret_cast<uint16_t *>(&size)[1]);
    rawReq.push_back(reinterpret_cast<uint16_t *>(&size)[0]);

    rawReq.insert(rawReq.end(), dat.begin(), dat.end());

    ::send(_sockfd, rawReq.begin().base(), rawReq.size(), 0);

    return rawReq;
}

std::vector<uint8_t> Connection::awaitRawMessage() {
    pollfd pfd;
    pfd.fd = this->_sockfd;
    pfd.events = POLLIN;
    pfd.revents = POLLIN;
    if (::poll(&pfd, 1, 60 * 1000 /* 1 minute means the connection has died */) <= 0) {
        throw MB::ModbusException(MB::utils::ConnectionClosed);
    }

    std::vector<uint8_t> r(1024);

    auto size = ::recv(_sockfd, r.begin().base(), r.size(), 0);

    if (size == -1)
        throw MB::ModbusException(MB::utils::ProtocolError);
    else if (size == 0) {
        throw MB::ModbusException(MB::utils::ConnectionClosed);
    }

    r.resize(size); // Set vector to proper shape
    r.shrink_to_fit();

    return r;
}

MB::ModbusRequest Connection::awaitRequest() {
    pollfd pfd;
    pfd.fd = this->_sockfd;
    pfd.events = POLLIN;
    pfd.revents = POLLIN;
    if (::poll(&pfd, 1, 60 * 1000 /* 1 minute means the connection has died */) <= 0) {
        throw MB::ModbusException(MB::utils::Timeout);
    }

    std::vector<uint8_t> r(1024);

    auto size = ::recv(_sockfd, r.begin().base(), r.size(), 0);

    if (size == -1)
        throw MB::ModbusException(MB::utils::ProtocolError);
    else if (size == 0) {
        throw MB::ModbusException(MB::utils::ConnectionClosed);
    }

    r.resize(size); // Set vector to proper shape
    r.shrink_to_fit();

    const auto resultMessageID = *reinterpret_cast<uint16_t *>(&r[0]);

    _messageID = resultMessageID;

    r.erase(r.begin(), r.begin() + 6);

    return MB::ModbusRequest::fromRaw(r);
}

MB::ModbusResponse Connection::awaitResponse() {
    pollfd pfd;
    pfd.fd = this->_sockfd;
    pfd.events = POLLIN;
    pfd.revents = POLLIN;

    if (::poll(&pfd, 1, this->_timeout) <= 0) {
        throw MB::ModbusException(MB::utils::Timeout);
    }

    std::vector<uint8_t> r(1024);
    auto size = ::recv(this->_sockfd, r.begin().base(), r.size(), 0);

    if (size == -1)
        throw MB::ModbusException(MB::utils::ProtocolError);
    else if (size == 0) {
        throw MB::ModbusException(MB::utils::ConnectionClosed);
    }

    r.resize(size); // Set vector to proper shape
    r.shrink_to_fit();

    const auto resultMessageID = *reinterpret_cast<uint16_t *>(&r[0]);

    if (resultMessageID != this->_messageID)
        throw MB::ModbusException(MB::utils::InvalidMessageID);

    r.erase(r.begin(), r.begin() + 6);

    if (MB::ModbusException::exist(r))
        throw MB::ModbusException(r);

    return MB::ModbusResponse::fromRaw(r);
}

Connection::Connection(Connection &&moved) noexcept {
    if (_sockfd != -1 && moved._sockfd != _sockfd)
        ::close(_sockfd);

    _sockfd       = moved._sockfd;
    _messageID    = moved._messageID;
    moved._sockfd = -1;
}

Connection Connection::with(std::string addr, int port) {
    auto sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        throw std::runtime_error("Cannot open socket, errno = " + std::to_string(errno));

    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = ::htons(port);
    server.sin_addr = { inet_addr(addr.c_str()) };

    if (::connect(sock, reinterpret_cast<struct sockaddr *>(&server), sizeof(server)) < 0)
        throw std::runtime_error("Cannot connect, errno = " + std::to_string(errno));

    return Connection(sock);
}

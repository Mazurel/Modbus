// Modbus for c++ <https://github.com/Mazurel/Modbus>
// Copyright (c) 2020 Mateusz Mazur aka Mazurel
// Licensed under: MIT License <http://opensource.org/licenses/MIT>

#include "TCP/connection.hpp"

using namespace MB::TCP;

Connection::Connection(const int sockfd) noexcept {
  _sockfd = sockfd;
  _messageID = 0;
}

Connection::~Connection() {
  if (_sockfd == -1)
    return;

#ifdef _WIN32
  closesocket(_sockfd);
#else
  ::close(_sockfd);
#endif
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

  uint32_t size = (uint32_t)dat.size();
  rawReq.push_back((uint8_t)reinterpret_cast<uint16_t *>(&size)[1]);
  rawReq.push_back((uint8_t)reinterpret_cast<uint16_t *>(&size)[0]);

  rawReq.insert(rawReq.end(), dat.begin(), dat.end());

  ::send(_sockfd, (const char*)rawReq.data(), (int)rawReq.size(), 0);

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

  uint32_t size = (uint32_t)dat.size();
  rawReq.push_back((uint8_t)reinterpret_cast<uint16_t *>(&size)[1]);
  rawReq.push_back((uint8_t)reinterpret_cast<uint16_t *>(&size)[0]);

  rawReq.insert(rawReq.end(), dat.begin(), dat.end());

  ::send(_sockfd, (const char*)rawReq.data(), (int)rawReq.size(), 0);

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

  uint32_t size = (uint32_t)dat.size();
  rawReq.push_back((uint8_t)reinterpret_cast<uint16_t *>(&size)[1]);
  rawReq.push_back((uint8_t)reinterpret_cast<uint16_t *>(&size)[0]);

  rawReq.insert(rawReq.end(), dat.begin(), dat.end());

  ::send(_sockfd, (const char*)rawReq.data(), (int)rawReq.size(), 0);

  return rawReq;
}

std::vector<uint8_t> Connection::awaitRawMessage() {
  pollfd _pfd = {.fd = (SOCKET)_sockfd, .events = POLLIN, .revents = POLLIN};
  if (::poll(&_pfd, 1,
             60 * 1000 /* 1 minute means the connection has died */) <= 0) {
    throw MB::ModbusException(MB::utils::ConnectionClosed);
  }

  std::vector<uint8_t> r(1024);

  auto size = ::recv(_sockfd, (char*)r.data(), (int)r.size(), 0);

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
  pollfd _pfd = {.fd = (SOCKET)_sockfd, .events = POLLIN, .revents = POLLIN};
  if (::poll(&_pfd, 1,
             60 * 1000 /* 1 minute means the connection has died */) <= 0) {
    throw MB::ModbusException(MB::utils::Timeout);
  }

  std::vector<uint8_t> r(1024);

  auto size = ::recv(_sockfd, (char*)r.data(), (int)r.size(), 0);

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
  pollfd _pfd = {.fd = (SOCKET)_sockfd, .events = POLLIN, .revents = POLLIN};
  if (::poll(&_pfd, 1, _timeout) <= 0) {
    throw MB::ModbusException(MB::utils::Timeout);
  }

  std::vector<uint8_t> r(1024);
  auto size = ::recv(_sockfd, (char*)r.data(), (int)r.size(), 0);

  if (size == -1)
    throw MB::ModbusException(MB::utils::ProtocolError);
  else if (size == 0) {
    throw MB::ModbusException(MB::utils::ConnectionClosed);
  }

  r.resize(size); // Set vector to proper shape
  r.shrink_to_fit();

  const auto resultMessageID = *reinterpret_cast<uint16_t *>(&r[0]);

  if (resultMessageID != _messageID)
    throw MB::ModbusException(MB::utils::InvalidMessageID);

  r.erase(r.begin(), r.begin() + 6);

  if (MB::ModbusException::exist(r))
    throw MB::ModbusException(r);

  return MB::ModbusResponse::fromRaw(r);
}

Connection::Connection(Connection &&moved) noexcept {
  if (_sockfd != -1 && moved._sockfd != _sockfd) {
#ifdef _WIN32
        closesocket(_sockfd);
#else
        ::close(_sockfd);
#endif

  }

  _sockfd = moved._sockfd;
  _messageID = moved._messageID;
  moved._sockfd = -1;
}

Connection Connection::with(std::string addr, int port) {
  auto sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1)
    throw std::runtime_error("Cannot open socket, errno = " +
                             std::to_string(errno));

#ifdef _WIN32
  sockaddr_in server = { .sin_family = AF_INET,
                         .sin_port = htons(port),
                         .sin_addr = {},
                         .sin_zero = {} };
  inet_pton(AF_INET, addr.c_str(), &server.sin_addr);
#else
  sockaddr_in server = {.sin_family = AF_INET,
                        .sin_port = htons(port),
                        .sin_addr = {inet_addr(addr.c_str())},
                        .sin_zero = {}};
#endif

  if (::connect(sock, reinterpret_cast<struct sockaddr *>(&server),
                sizeof(server)) < 0)
    throw std::runtime_error("Cannot connect, errno = " +
                             std::to_string(errno));

  return Connection((int)sock);
}

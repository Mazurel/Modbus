// Modbus for c++ <https://github.com/Mazurel/Modbus>
// Copyright (c) 2020 Mateusz Mazur aka Mazurel
// Licensed under: MIT License <http://opensource.org/licenses/MIT>

#pragma once

#include <string>
#include <vector>

#include "../modbusException.hpp"
#include "../modbusRequest.hpp"
#include "../modbusResponse.hpp"

namespace MB {
namespace TCP {

class Connection {
public:
  static const unsigned int DefaultTCPTimeout = 500;

private:
  int _sockfd = -1;
  uint16_t _messageID = 0;
  int _timeout = Connection::DefaultTCPTimeout;

  void closeSockfd(void);

public:
  explicit Connection() noexcept : _sockfd(-1), _messageID(0){};
  explicit Connection(int sockfd) noexcept;
  Connection(const Connection &copy) = delete;
  Connection(Connection &&moved) noexcept;
  Connection& operator=(Connection&& other) noexcept;

  [[nodiscard]] int getSockfd() const { return _sockfd; }

  static Connection with(const std::string &addr, int port);

  ~Connection();

  std::vector<uint8_t> sendRequest(const MB::ModbusRequest &req);
  std::vector<uint8_t> sendResponse(const MB::ModbusResponse &res);
  std::vector<uint8_t> sendException(const MB::ModbusException &ex);

  [[nodiscard]] MB::ModbusRequest awaitRequest();
  [[nodiscard]] MB::ModbusResponse awaitResponse();

  [[nodiscard]] std::vector<uint8_t> awaitRawMessage();

  [[nodiscard]] uint16_t getMessageId() const { return _messageID; }

  void setMessageId(uint16_t messageId) { _messageID = messageId; }
};
}} // namespace MB::TCP

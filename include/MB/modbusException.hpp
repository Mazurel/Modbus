// Modbus for c++ <https://github.com/Mazurel/Modbus>
// Copyright (c) 2020 Mateusz Mazur aka Mazurel
// Licensed under: MIT License <http://opensource.org/licenses/MIT>

#pragma once

#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <vector>

#include "modbusUtils.hpp"

/**
 * Namespace that contains whole project
 */
namespace MB {
/**
 * Thic class represent Modbus exception and is
 * derived form std::exception. It is just a wrapper
 * around standard error codes and some custom codes.
 */
class ModbusException : public std::exception {
private:
  uint8_t _slaveId;
  bool _validSlave;
  utils::MBErrorCode _errorCode;
  utils::MBFunctionCode _functionCode;

public:
  /**
   * @brief Constructs Exception from raw data
   * @param inputData is vector of bytes that will be be interpreted
   * @param CRC based on this param method performs CRC calculation and throws
   * exception if it is invalid
   * @note if CRC = true input data needs to contain 2 CRC bytes on back (used
   * in RS)
   * */
  explicit ModbusException(const std::vector<uint8_t> &inputData,
                           bool CRC = false) noexcept;

  //! Constructs Exception based on error code, function code and slaveId
  explicit ModbusException(
      utils::MBErrorCode errorCode, uint8_t slaveId = 0xFF,
      utils::MBFunctionCode functionCode = utils::Undefined) noexcept
      : _slaveId(slaveId), _validSlave(true), _errorCode(errorCode),
        _functionCode(functionCode) {}

  /*
   * Check if there is Modbus error in raw modbus input
   * NOTE: this method doesn't detect invalid byte order, byte order is
   * checked at ModbusRequest/ModbusResponse
   * */
  static bool exist(const std::vector<uint8_t> &inputData) noexcept {
    if (inputData.size() < 2) // TODO Figure out better solution to such mistake
      return false;

    return inputData[1] & 0b10000000;
  }

  /*
   *  Returns attached SlaveID
   *  NOTE: it is worth to check if slaveId is specified with isSlaveValid()
   * */
  [[nodiscard]] uint8_t slaveID() const noexcept { return _slaveId; }

  //! Checks if SlaveID is specified
  [[nodiscard]] bool isSlaveValid() const noexcept { return _validSlave; }

  //! Sets SlaveID
  void setSlaveID(uint8_t slaveId) noexcept {
    _validSlave = true;
    _slaveId = slaveId;
  }

  //! Returns detected error code
  [[nodiscard]] utils::MBErrorCode getErrorCode() const noexcept {
    return _errorCode;
  }

  /**
   * This function is less optimal, it is just to be compatible with
   * std::excepetion You should preferably use toString()
   */
  [[nodiscard]] const char *what() const noexcept override {
    auto og = toString();
    char *str = new char[og.size()];
    stpcpy(str, og.c_str());
    return str;
  }

  //! Returns string representation of object
  [[nodiscard]] std::string toString() const noexcept;
  //! Converts object to modbus byte representation
  [[nodiscard]] std::vector<uint8_t> toRaw() const noexcept;

  [[nodiscard]] utils::MBFunctionCode functionCode() const noexcept {
    return _functionCode;
  }

  void setFunctionCode(utils::MBFunctionCode functionCode) noexcept {
    _functionCode = functionCode;
  }
};
} // namespace MB

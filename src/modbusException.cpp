// Modbus for c++ <https://github.com/Mazurel/Modbus>
// Copyright (c) 2020 Mateusz Mazur aka Mazurel
// Licensed under: MIT License <http://opensource.org/licenses/MIT>

#include "modbusException.hpp"

using namespace MB;

// Construct Modbus exception from raw data
ModbusException::ModbusException(const std::vector<uint8_t> &inputData,
                                 bool CRC) noexcept {
  if (inputData.size() != ((CRC) ? 5 : 3)) {
    _slaveId = 0xFF;
    _functionCode = utils::Undefined;
    _validSlave = false;
    _errorCode = utils::InvalidByteOrder;
    return;
  }

  _slaveId = inputData[0];
  _functionCode = static_cast<utils::MBFunctionCode>(0b01111111 & inputData[1]);
  _validSlave = true;
  _errorCode = static_cast<utils::MBErrorCode>(inputData[2]);

  if (CRC) {
    auto CRC = *reinterpret_cast<const uint16_t *>(&inputData[3]);
    auto calculatedCRC = utils::calculateCRC(inputData.begin().base(), 3);

    if (CRC != calculatedCRC) {
      _errorCode = utils::ErrorCodeCRCError;
    }
  }
}

// Returns string representation of exception
std::string ModbusException::toString() const noexcept {
  std::string res;
  res += "Error on slave ";
  if (_validSlave)
    res += std::to_string(_slaveId);
  else
    res += "Unknown";
  res += " - ";
  res += utils::mbErrorCodeToStr(_errorCode);
  if (_functionCode != MB::utils::Undefined) {
    res += " ( on function: ";
    res += utils::mbFunctionToStr(_functionCode);
    res += " )";
  }
  return res;
}

std::vector<uint8_t> ModbusException::toRaw() const noexcept {
  std::vector<uint8_t> result(3);

  result[0] = _slaveId;
  result[1] = static_cast<uint8_t>(_errorCode | 0b10000000);
  result[2] = static_cast<uint8_t>(_functionCode);

  return result;
}

// Modbus for c++ <https://github.com/Mazurel/Modbus>
// Copyright (c) 2020 Mateusz Mazur aka Mazurel
// Licensed under: MIT License <http://opensource.org/licenses/MIT>

// This header files contain various utilities for Modbus Core library

#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>
#include <tuple>

/*!
 * Namespace that contains many useful utility functions and enums
 * that are used in the whole project.
 */
namespace MB::utils {
/*! All possible modbus error codes
 * @note Contains custom, non standard codes
 */
enum MBErrorCode : uint8_t {
  // Documentation modbus errors
  IllegalFunction = 0x01,
  IllegalDataAddress = 0x02,
  IllegalDataValue = 0x03,
  SlaveDeviceFailure = 0x04,
  Acknowledge = 0x05,
  SlaveDeviceBusy = 0x06,
  NegativeAcknowledge = 0x07,
  MemoryParityError = 0x08,
  GatewayPathUnavailable = 0x10,
  GatewayTargetDeviceFailedToRespond = 0x11,

  // Custom modbus errors
  ErrorCodeCRCError = 0b0111111,
  InvalidCRC = 0b01111110,
  InvalidByteOrder = 0b01111101,
  InvalidMessageID = 0b01111100,
  ProtocolError = 0b01111011,
  ConnectionClosed = 0b01111010,
  Timeout = 0b01111001
};

//! Checks if error code is modbus standard error code
inline bool isStandardErrorCode(MBErrorCode code) {
  switch (code) {
  case IllegalFunction:
  case IllegalDataAddress:
  case IllegalDataValue:
  case SlaveDeviceFailure:
  case Acknowledge:
  case SlaveDeviceBusy:
  case NegativeAcknowledge:
  case MemoryParityError:
  case GatewayPathUnavailable:
  case GatewayTargetDeviceFailedToRespond:
    return true;
  case ErrorCodeCRCError:
  case InvalidCRC:
  case InvalidByteOrder:
  case InvalidMessageID:
  case ProtocolError:
  case ConnectionClosed:
  case Timeout:
  default:
    return false;
  }
}

//! Converts Modbus error code to it's string representation
inline std::string mbErrorCodeToStr(MBErrorCode code) noexcept {
  switch (code) {
  case IllegalFunction:
    return "Illegal function";
  case IllegalDataAddress:
    return "Illegal data address";
  case IllegalDataValue:
    return "Illegal data value";
  case SlaveDeviceFailure:
    return "Slave device failure";
  case Acknowledge:
    return "Acknowledge";
  case SlaveDeviceBusy:
    return "Slave device busy";
  case NegativeAcknowledge:
    return "Negative acknowledge";
  case MemoryParityError:
    return "Memory parity error";
  case GatewayPathUnavailable:
    return "Gateway path unavailable";
  case GatewayTargetDeviceFailedToRespond:
    return "Gateway target device failed to respond";
  case ErrorCodeCRCError:
    return "CRC error code error";
  case InvalidCRC:
    return "Invalid CRC";
  case InvalidByteOrder:
    return "Invalid byte order";
  case InvalidMessageID:
    return "Invalid messageID (TCP/IP)";
  case ProtocolError:
    return "Protocol error (may be timeout)";
  case ConnectionClosed:
    return "Connection is closed";
  case Timeout:
    return "Timeout";
  default:
    return "Undefined Error";
  }
}

//! All modbus standard function codes + Undefined one
enum MBFunctionCode : uint8_t {
  // Reading functions
  ReadDiscreteOutputCoils = 0x01,
  ReadDiscreteInputContacts = 0x02,
  ReadAnalogOutputHoldingRegisters = 0x03,
  ReadAnalogInputRegisters = 0x04,

  // Single write functions
  WriteSingleDiscreteOutputCoil = 0x05,
  WriteSingleAnalogOutputRegister = 0x06,

  // Multiple write functions
  WriteMultipleDiscreteOutputCoils = 0x0F,
  WriteMultipleAnalogOutputHoldingRegisters = 0x10,

  // User defined
  Undefined = 0x00
};

//! Simplified function types
enum MBFunctionType { Read, WriteSingle, WriteMultiple };

//! Checks "Function type", according to MBFunctionType
inline MBFunctionType functionType(const MBFunctionCode code) {
  switch (code) {
  case ReadDiscreteOutputCoils:
  case ReadAnalogInputRegisters:
  case ReadDiscreteInputContacts:
  case ReadAnalogOutputHoldingRegisters:
    return Read;
  case WriteSingleAnalogOutputRegister:
  case WriteSingleDiscreteOutputCoil:
    return WriteSingle;
  case WriteMultipleAnalogOutputHoldingRegisters:
  case WriteMultipleDiscreteOutputCoils:
    return WriteMultiple;
  default:
    throw std::runtime_error("The function code is undefined");
  }
}

//! Simplified register types
enum MBFunctionRegisters {
  OutputCoils,
  InputContacts,
  HoldingRegisters,
  InputRegisters
};

//! Get register type based on function code
inline MBFunctionRegisters functionRegister(const MBFunctionCode code) {
  switch (code) {
  case ReadDiscreteOutputCoils:
  case WriteSingleDiscreteOutputCoil:
  case WriteMultipleDiscreteOutputCoils:
    return OutputCoils;
  case ReadDiscreteInputContacts:
    return InputContacts;
  case ReadAnalogOutputHoldingRegisters:
  case WriteSingleAnalogOutputRegister:
  case WriteMultipleAnalogOutputHoldingRegisters:
    return HoldingRegisters;
  case ReadAnalogInputRegisters:
    return InputRegisters;
  default:
    throw std::runtime_error("The function code is undefined");
  }
}

//! Converts modbus function code to its string represenatiton
inline std::string mbFunctionToStr(MBFunctionCode code) noexcept {
  switch (code) {
  case ReadDiscreteOutputCoils:
    return "Read from output coils";
  case ReadDiscreteInputContacts:
    return "Read from input contacts";
  case ReadAnalogOutputHoldingRegisters:
    return "Read from output registers";
  case ReadAnalogInputRegisters:
    return "Read from input registers";
  case WriteSingleAnalogOutputRegister:
    return "Write to single analog register";
  case WriteSingleDiscreteOutputCoil:
    return "Write to single coil";
  case WriteMultipleAnalogOutputHoldingRegisters:
    return "Write to multiple holding registers";
  case WriteMultipleDiscreteOutputCoils:
    return "Write to multiple output coils";
  default:
    return "Undefined";
  }
}

//! Create uint16_t from buffer of two bytes, ex. { 0x01, 0x02 } => 0x0102
inline uint16_t bigEndianConv(const uint8_t *buf) {
  return static_cast<uint16_t>(buf[1]) +
         (static_cast<uint16_t>(buf[0]) << 8); // NOLINT(hicpp-signed-bitwise)
}

//! Calculates CRC
inline uint16_t calculateCRC(const uint8_t *buff, size_t len) {
  static const uint16_t wCRCTable[] = {
      0X0000, 0XC0C1, 0XC181, 0X0140, 0XC301, 0X03C0, 0X0280, 0XC241, 0XC601,
      0X06C0, 0X0780, 0XC741, 0X0500, 0XC5C1, 0XC481, 0X0440, 0XCC01, 0X0CC0,
      0X0D80, 0XCD41, 0X0F00, 0XCFC1, 0XCE81, 0X0E40, 0X0A00, 0XCAC1, 0XCB81,
      0X0B40, 0XC901, 0X09C0, 0X0880, 0XC841, 0XD801, 0X18C0, 0X1980, 0XD941,
      0X1B00, 0XDBC1, 0XDA81, 0X1A40, 0X1E00, 0XDEC1, 0XDF81, 0X1F40, 0XDD01,
      0X1DC0, 0X1C80, 0XDC41, 0X1400, 0XD4C1, 0XD581, 0X1540, 0XD701, 0X17C0,
      0X1680, 0XD641, 0XD201, 0X12C0, 0X1380, 0XD341, 0X1100, 0XD1C1, 0XD081,
      0X1040, 0XF001, 0X30C0, 0X3180, 0XF141, 0X3300, 0XF3C1, 0XF281, 0X3240,
      0X3600, 0XF6C1, 0XF781, 0X3740, 0XF501, 0X35C0, 0X3480, 0XF441, 0X3C00,
      0XFCC1, 0XFD81, 0X3D40, 0XFF01, 0X3FC0, 0X3E80, 0XFE41, 0XFA01, 0X3AC0,
      0X3B80, 0XFB41, 0X3900, 0XF9C1, 0XF881, 0X3840, 0X2800, 0XE8C1, 0XE981,
      0X2940, 0XEB01, 0X2BC0, 0X2A80, 0XEA41, 0XEE01, 0X2EC0, 0X2F80, 0XEF41,
      0X2D00, 0XEDC1, 0XEC81, 0X2C40, 0XE401, 0X24C0, 0X2580, 0XE541, 0X2700,
      0XE7C1, 0XE681, 0X2640, 0X2200, 0XE2C1, 0XE381, 0X2340, 0XE101, 0X21C0,
      0X2080, 0XE041, 0XA001, 0X60C0, 0X6180, 0XA141, 0X6300, 0XA3C1, 0XA281,
      0X6240, 0X6600, 0XA6C1, 0XA781, 0X6740, 0XA501, 0X65C0, 0X6480, 0XA441,
      0X6C00, 0XACC1, 0XAD81, 0X6D40, 0XAF01, 0X6FC0, 0X6E80, 0XAE41, 0XAA01,
      0X6AC0, 0X6B80, 0XAB41, 0X6900, 0XA9C1, 0XA881, 0X6840, 0X7800, 0XB8C1,
      0XB981, 0X7940, 0XBB01, 0X7BC0, 0X7A80, 0XBA41, 0XBE01, 0X7EC0, 0X7F80,
      0XBF41, 0X7D00, 0XBDC1, 0XBC81, 0X7C40, 0XB401, 0X74C0, 0X7580, 0XB541,
      0X7700, 0XB7C1, 0XB681, 0X7640, 0X7200, 0XB2C1, 0XB381, 0X7340, 0XB101,
      0X71C0, 0X7080, 0XB041, 0X5000, 0X90C1, 0X9181, 0X5140, 0X9301, 0X53C0,
      0X5280, 0X9241, 0X9601, 0X56C0, 0X5780, 0X9741, 0X5500, 0X95C1, 0X9481,
      0X5440, 0X9C01, 0X5CC0, 0X5D80, 0X9D41, 0X5F00, 0X9FC1, 0X9E81, 0X5E40,
      0X5A00, 0X9AC1, 0X9B81, 0X5B40, 0X9901, 0X59C0, 0X5880, 0X9841, 0X8801,
      0X48C0, 0X4980, 0X8941, 0X4B00, 0X8BC1, 0X8A81, 0X4A40, 0X4E00, 0X8EC1,
      0X8F81, 0X4F40, 0X8D01, 0X4DC0, 0X4C80, 0X8C41, 0X4400, 0X84C1, 0X8581,
      0X4540, 0X8701, 0X47C0, 0X4680, 0X8641, 0X8201, 0X42C0, 0X4380, 0X8341,
      0X4100, 0X81C1, 0X8081, 0X4040};

  uint8_t nTemp;
  uint16_t wCRCWord = 0xFFFF;

  while (len--) {
    nTemp = *buff++ ^ wCRCWord;
    wCRCWord >>= 8;
    wCRCWord ^= wCRCTable[nTemp];
  }
  return wCRCWord;
}

//! Calculate CRC wrapper
inline uint16_t calculateCRC(const std::vector<uint8_t> &buffer) {
  return calculateCRC(buffer.begin().base(), buffer.size());
}

//! Split uint16_t to two uint8_t in big endian form
//!
//! Example usage:
//! auto [highByte, lowByte] = utils::splitUint16(val);
inline std::pair<uint8_t, uint8_t> splitUint16(const uint16_t val) {
  return std::make_pair((val >> 8) & 0xFF, val & 0xFF);
}

//! Insert uint16_t into buffer of uint8_t's. Preserve big endianess.
inline void pushUint16(std::vector<uint8_t>& buffer, const uint16_t val) {
  auto [high, low] = splitUint16(val);
  buffer.push_back(high);
  buffer.push_back(low);
}

} // namespace MB::utils

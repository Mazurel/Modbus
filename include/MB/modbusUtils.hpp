// Modbus for c++ <https://github.com/Mazurel/Modbus>
// Copyright (c) 2024 Mateusz Mazur aka Mazurel
// Licensed under: MIT License <http://opensource.org/licenses/MIT>

// This header files contain various utilities for Modbus Core library

#pragma once

#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

#include "MB/crc.hpp"

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
    IllegalFunction                    = 0x01,
    IllegalDataAddress                 = 0x02,
    IllegalDataValue                   = 0x03,
    SlaveDeviceFailure                 = 0x04,
    Acknowledge                        = 0x05,
    SlaveDeviceBusy                    = 0x06,
    NegativeAcknowledge                = 0x07,
    MemoryParityError                  = 0x08,
    GatewayPathUnavailable             = 0x10,
    GatewayTargetDeviceFailedToRespond = 0x11,

    // Custom modbus errors for Modbus for C++ library
    ErrorCodeCRCError = 0b0111111,
    InvalidCRC        = 0b01111110,
    InvalidByteOrder  = 0b01111101,
    InvalidMessageID  = 0b01111100,
    ProtocolError     = 0b01111011,
    ConnectionClosed  = 0b01111010,
    Timeout           = 0b01111001,
    // Specific for modbus response, when number of registers is too big
    // See issue: https://github.com/Mazurel/Modbus/issues/3
    NumberOfRegistersInvalid = 0b01111000,
    NumberOfValuesInvalid    = 0b01110111,
    InputDataLengthInvalid        = 0b01110110,
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
    case NumberOfRegistersInvalid:
    case InputDataLengthInvalid:
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
    case NumberOfRegistersInvalid:
        return "Number of registers in response is too big - cannot be serialized";
    case NumberOfValuesInvalid:
        return "Number of values is not valid";
    case InputDataLengthInvalid:
        return "Encountered end of data during parsing";
    }

    return "Unknown";
}

//! All modbus standard function codes + Undefined one
enum MBFunctionCode : uint8_t {
    // Reading functions
    ReadDiscreteOutputCoils          = 0x01,
    ReadDiscreteInputContacts        = 0x02,
    ReadAnalogOutputHoldingRegisters = 0x03,
    ReadAnalogInputRegisters         = 0x04,

    // Single write functions
    WriteSingleDiscreteOutputCoil   = 0x05,
    WriteSingleAnalogOutputRegister = 0x06,

    // Multiple write functions
    WriteMultipleDiscreteOutputCoils          = 0x0F,
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
    case Undefined:
        throw std::runtime_error("The function code is undefined");
    }
    throw std::runtime_error("The function code is undefined");
}

//! Simplified register types
enum MBFunctionRegisters { OutputCoils, InputContacts, HoldingRegisters, InputRegisters };

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
    case Undefined:
        throw std::runtime_error("The function code is undefined");
    }
    throw std::runtime_error("The function code is undefined");
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
    case Undefined:
        return "Undefined";
    }
    return "Undefined";
}

//! Create uint16_t from buffer of two bytes, ex. { 0x01, 0x02 } => 0x0102
inline uint16_t bigEndianConv(const uint8_t *buf) {
    return static_cast<uint16_t>(buf[1]) + (static_cast<uint16_t>(buf[0]) << 8u);
}

//! @deprecated Calculates CRC - please use functions from `MB::CRC`
inline uint16_t calculateCRC(const uint8_t *buff, size_t len) {
    return MB::CRC::calculateCRC(buff, len);
}

//! @deprecated Calculates CRC - please use functions from `MB::CRC`
inline uint16_t calculateCRC(const std::vector<uint8_t> &buffer) {
    return MB::CRC::calculateCRC(buffer);
}

//! Split uint16_t to two uint8_t in big endian form
//!
//! Example usage:
//! auto [highByte, lowByte] = utils::splitUint16(val);
inline std::pair<uint8_t, uint8_t> splitUint16(const uint16_t val) {
    return std::make_pair((val >> 8) & 0xFF, val & 0xFF);
}

//! Insert uint16_t into buffer of uint8_t's. Preserve big endianess.
inline void pushUint16(std::vector<uint8_t> &buffer, const uint16_t val) {
    auto [high, low] = splitUint16(val);
    buffer.push_back(high);
    buffer.push_back(low);
}

//! Ignore some value explicitly
template <typename T> inline void ignore_result(T &&v) { (void)v; }

} // namespace MB::utils

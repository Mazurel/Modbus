// Modbus for c++ <https://github.com/Mazurel/Modbus>
// Copyright (c) 2020 Mateusz Mazur aka Mazurel
// Licensed under: MIT License <http://opensource.org/licenses/MIT>

#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

#include "modbusCell.hpp"
#include "modbusException.hpp"
#include "modbusRequest.hpp"
#include "modbusUtils.hpp"

/**
 * Namespace that contains whole project
 */
namespace MB {
/**
 * This class represents Modbus response and allows
 * user to manipulate and represent it in various ways
 */
class ModbusResponse {
private:
  uint8_t _slaveID;
  utils::MBFunctionCode _functionCode;

  uint16_t _address;
  uint16_t _registersNumber;

  std::vector<ModbusCell> _values;

public:
  /**
   * @brief
   * Constructs Response from raw data
   * @note
   * 1) if CRC = true input data needs to contain 2 CRC bytes on back (used in
   *RS)
   * @note
   * 2) This is private constructor, you need to use fromRaw or fromRawCRC
   * @param inputData - Is vector of bytes that will be be interpreted,
   *whereas based on CRC parameter method performs CRC calculation and throws
   *exception if it is invalid
   * @throws ModbusException
   **/
  explicit ModbusResponse(std::vector<uint8_t> inputData, bool CRC = false);

  /*
   * @description Constructs Response from raw data
   * @params inputData is a vector of bytes that will be interpreted
   * @throws ModbusException
   **/
  static ModbusResponse fromRaw(std::vector<uint8_t> inputData) {
    return ModbusResponse(inputData);
  }
  /*
   * @description Constructs Request from raw data and checks it's CRC
   * @params inputData is a vector of bytes that will be interpreted
   * @throws ModbusException
   * @note This methods performs CRC check that may throw ModbusException on
   * invalid CRC
   **/
  static ModbusResponse fromRawCRC(std::vector<uint8_t> inputData) {
    return ModbusResponse(inputData, true);
  }

  /**
   * Simple constructor, that allows to create "dummy" ModbusResponse
   * object. May be useful in some cases.
   */
  ModbusResponse(uint8_t slaveId = 0,
                 utils::MBFunctionCode functionCode =
                     static_cast<utils::MBFunctionCode>(0),
                 uint16_t address = 0, uint16_t registersNumber = 0,
                 std::vector<ModbusCell> values = {});

  ModbusResponse(const ModbusResponse &) = default;

  //! Converts object to it's string representation
  [[nodiscard]] std::string toString() const;
  [[nodiscard]] std::vector<uint8_t> toRaw() const;
  //! Fills all data from associated request
  void from(const ModbusRequest &);

  [[nodiscard]] utils::MBFunctionType functionType() const {
    return utils::functionType(_functionCode);
  }

  [[nodiscard]] utils::MBFunctionRegisters functionRegisters() const {
    return utils::functionRegister(_functionCode);
  }

  [[nodiscard]] uint8_t slaveID() const { return _slaveID; }
  [[nodiscard]] utils::MBFunctionCode functionCode() const {
    return _functionCode;
  }
  [[nodiscard]] uint16_t registerAddress() const { return _address; }
  [[nodiscard]] uint16_t numberOfRegisters() const { return _registersNumber; }
  [[nodiscard]] const std::vector<ModbusCell> &registerValues() const {
    return _values;
  }

  void setSlaveId(uint8_t slaveId) { _slaveID = slaveId; }
  void setFunctionCode(utils::MBFunctionCode functionCode) {
    _functionCode = functionCode;
  }
  void setAddress(uint16_t address) { _address = address; }
  void setRegistersNumber(uint16_t registersNumber) {
    _registersNumber = registersNumber;
    _values.resize(registersNumber);
  }
  void setValues(const std::vector<ModbusCell> &values) { _values = values; }
};
} // namespace MB

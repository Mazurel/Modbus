// Modbus for c++ <https://github.com/Mazurel/Modbus>
// Copyright (c) 2020 Mateusz Mazur aka Mazurel
// Licensed under: MIT License <http://opensource.org/licenses/MIT>

#include "modbusRequest.hpp"
#include "modbusUtils.hpp"

#include <sstream>
#include <algorithm>

using namespace MB;

ModbusRequest::ModbusRequest(uint8_t slaveId,
                             utils::MBFunctionCode functionCode,
                             uint16_t address, uint16_t registersNumber,
                             std::vector<ModbusCell> values) noexcept
    : _slaveID(slaveId), _functionCode(functionCode), _address(address),
      _registersNumber(registersNumber), _values(std::move(values)) {
  // Force proper modbuscell type
  switch (functionRegisters()) {
  case utils::OutputCoils:
  case utils::InputContacts:
    std::for_each(_values.begin(), _values.end(),
                  [](ModbusCell &cell) -> void { cell.coil(); });
    break;
  case utils::HoldingRegisters:
  case utils::InputRegisters:
    std::for_each(_values.begin(), _values.end(),
                  [](ModbusCell &cell) -> void { cell.reg(); });
    break;
  }
}

ModbusRequest::ModbusRequest(const std::vector<uint8_t> &inputData, bool CRC) {
  try {
    if (inputData.size() < 3)
      throw ModbusException(utils::InvalidByteOrder);

    _slaveID = inputData[0];
    _functionCode = static_cast<utils::MBFunctionCode>(inputData[1]);
    _address = utils::bigEndianConv(&inputData[2]);

    int crcIndex = -1;
    int8_t follow;

    switch (_functionCode) {
    case utils::ReadDiscreteOutputCoils:
    case utils::ReadDiscreteInputContacts:
    case utils::ReadAnalogOutputHoldingRegisters:
    case utils::ReadAnalogInputRegisters:
      _registersNumber = utils::bigEndianConv(&inputData[4]);
      _values = {};
      crcIndex = 3 * 2;
      break;
    case utils::WriteSingleDiscreteOutputCoil:
      _registersNumber = 1;
      _values = {ModbusCell::initCoil(inputData[4] == 0xFF)};
      crcIndex = 3 * 2;
      break;
    case utils::WriteSingleAnalogOutputRegister:
      _registersNumber = 1;
      _values = {utils::bigEndianConv(&inputData[4])};
      crcIndex = 3 * 2;
      break;
    case utils::WriteMultipleDiscreteOutputCoils:
      _registersNumber = utils::bigEndianConv(&inputData[4]);
      follow = inputData[6];
      _values = std::vector<ModbusCell>(_registersNumber);
      for (int8_t i = 0; i < _registersNumber; i++) {
        _values[i].coil() = inputData[7 + (i / 8)] & (1 << (i % 8));
      }
      crcIndex = 6 + follow + 1;
      break;
    case utils::WriteMultipleAnalogOutputHoldingRegisters:
      _registersNumber = utils::bigEndianConv(&inputData[4]);
      follow = inputData[6];
      _values = std::vector<ModbusCell>(_registersNumber);
      for (int8_t i = 0; i < _registersNumber; i++) {
        _values[i].reg() = utils::bigEndianConv(&inputData[i * 2 + 7]);
      }
      crcIndex = 6 + follow + 1;
      break;
    default:
      throw ModbusException(utils::InvalidByteOrder);
    }

    _values.resize(_registersNumber);

    if (CRC) {
      if (crcIndex == -1 ||
          static_cast<size_t>(crcIndex) + 2 > inputData.size())
        throw ModbusException(utils::InvalidByteOrder);

      auto recvCRC = *reinterpret_cast<const uint16_t *>(&inputData[crcIndex]);
      auto myCRC = utils::calculateCRC(inputData.begin().base(), crcIndex);

      if (recvCRC != myCRC) {
        throw ModbusException(utils::InvalidCRC, _slaveID);
      }
    }
  } catch (const ModbusException &ex) {
    throw ex;
  } catch (const std::exception &ex) {
    throw ModbusException(utils::InvalidByteOrder);
  }
}

std::string ModbusRequest::toString() const noexcept {
  std::stringstream result;

  result << utils::mbFunctionToStr(_functionCode)
         << ", from slave " + std::to_string(_slaveID);

  if (functionType() != utils::WriteSingle) {
    result << ", starting from address " + std::to_string(_address)
           << ", on " + std::to_string(_registersNumber) + " registers";
    if (functionType() == utils::WriteMultiple) {
      result << "\n values = { ";
      for (std::size_t i = 0; i < _values.size(); i++) {
        result << _values[i].toString() + " , ";
        if (i >= 3) {
          result << " , ... ";
          break;
        }
      }
      result << "}";
    }
  } else {
    result << ", starting from address " + std::to_string(_address)
           << "\nvalue = " + (*_values.begin()).toString();
  }

  return result.str();
}

std::vector<uint8_t> ModbusRequest::toRaw() const noexcept {
  std::vector<uint8_t> result;
  result.reserve(6);

  result.push_back(_slaveID);
  result.push_back(_functionCode);
  utils::pushUint16(result, _address);

  if (functionType() != utils::WriteSingle) {
    utils::pushUint16(result, _registersNumber);
  }

  if (_functionCode == utils::WriteMultipleAnalogOutputHoldingRegisters) {
    result.push_back(numberOfRegisters() * 2);
  } else if (_functionCode == utils::WriteMultipleDiscreteOutputCoils) {
    result.push_back((_registersNumber / 8) +
                     (_registersNumber % 8 == 0 ? 0 : 1));
  }

  if (functionType() == utils::WriteMultiple) {
    if (_values[0].isReg()) {
      for (auto value : _values) {
        utils::pushUint16(result, value.reg());
      }
    } else {
      int end = result.size() - 1;
      for (std::size_t i = 0; i < _values.size(); i++) {
        if (i % 8 == 0) {
          result.push_back(0x00);
          end++;
        }
        result[end] |= _values[i].coil() << (i % 8);
      }
    }
  } else if (functionType() == utils::WriteSingle) {
    if (_values[0].isReg()) {
      utils::pushUint16(result, _values[0].reg());
    } else {
      result.push_back(_values[0].coil() ? 0xFF : 0x00);
      result.push_back(0x00);
    }
  }

  return result;
}

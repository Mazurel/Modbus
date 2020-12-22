// Modbus for c++ <https://github.com/Mazurel/Modbus>
// Copyright (c) 2020 Mateusz Mazur aka Mazurel
// Licensed under: MIT License <http://opensource.org/licenses/MIT>

#include "modbusResponse.hpp"

using namespace MB;

ModbusResponse::ModbusResponse(uint8_t slaveId,
                               utils::MBFunctionCode functionCode,
                               uint16_t address, uint16_t registersNumber,
                               std::vector<ModbusCell> values)
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

ModbusResponse::ModbusResponse(std::vector<uint8_t> inputData, bool CRC) {
  try {
    if (inputData.size() < 3)
      throw ModbusException(utils::InvalidByteOrder);

    _slaveID = inputData[0];
    _functionCode = static_cast<utils::MBFunctionCode>(inputData[1]);

    if (functionType() != utils::Read)
      _address = utils::bigEndianConv(&inputData[2]);

    int crcIndex = -1;
    uint8_t bytes;

    switch (_functionCode) {
    case utils::ReadDiscreteOutputCoils:
    case utils::ReadDiscreteInputContacts:
      bytes = inputData[2];
      _registersNumber = bytes * 8;
      _values = std::vector<ModbusCell>(_registersNumber);
      for (auto i = 0; i < _registersNumber; i++) {
        _values[i].coil() = inputData[3 + (i / 8)] & (1 << (i % 8));
      }
      crcIndex = 2 + bytes + 1;
      break;
    case utils::ReadAnalogOutputHoldingRegisters:
    case utils::ReadAnalogInputRegisters:
      bytes = inputData[2];
      _registersNumber = bytes / 2;
      for (auto i = 0; i < bytes / 2; i++) {
        _values.emplace_back(utils::bigEndianConv(&inputData[3 + (i * 2)]));
      }
      crcIndex = 2 + bytes + 1;
      break;
    case utils::WriteSingleDiscreteOutputCoil:
      _registersNumber = 1;
      _address = utils::bigEndianConv(&inputData[2]);
      _values = {ModbusCell::initCoil(inputData[4] == 0xFF)};
      crcIndex = 6;
      break;
    case utils::WriteSingleAnalogOutputRegister:
      _registersNumber = 1;
      _address = utils::bigEndianConv(&inputData[2]);
      _values = {ModbusCell::initReg(utils::bigEndianConv(&inputData[4]))};
      crcIndex = 6;
      break;
    case utils::WriteMultipleDiscreteOutputCoils:
    case utils::WriteMultipleAnalogOutputHoldingRegisters:
      _address = utils::bigEndianConv(&inputData[2]);
      _registersNumber = utils::bigEndianConv(&inputData[4]);
      crcIndex = 6;
      break;
    default:
      throw ModbusException(utils::InvalidByteOrder);
    }

    _values.resize(_registersNumber);

    if (CRC) {
      if (crcIndex == -1 ||
          static_cast<size_t>(crcIndex) + 2 > inputData.size())
        throw ModbusException(utils::InvalidByteOrder);

      auto recievedCRC =
          *reinterpret_cast<const uint16_t *>(&inputData[crcIndex]);
      auto myCRC = utils::calculateCRC(inputData.begin().base(), crcIndex);

      if (recievedCRC != myCRC) {
        throw ModbusException(utils::InvalidCRC, _slaveID);
      }
    }
  } catch (const ModbusException &ex) {
    throw ex;
  } catch (const std::exception &ex) {
    throw ModbusException(utils::InvalidByteOrder);
  }
}

std::string ModbusResponse::toString() const {
  std::string result;

  result.append(utils::mbFunctionToStr(_functionCode));
  result.append(", from slave " + std::to_string(_slaveID));

  if (functionType() != utils::WriteSingle) {
    result.append(", starting from address " + std::to_string(_address));
    result.append(", on " + std::to_string(_registersNumber) + " registers");
    if (functionType() == utils::WriteMultiple) {
      result.append("\n values = { ");
      for (decltype(_values)::size_type i = 0; i < _values.size(); i++) {
        result.append(_values[i].toString() + " , ");
        if (i >= 3) {
          result.append(" , ... ");
          break;
        }
      }
      result.append("}");
    }
  } else {
    result.append(", starting from address " + std::to_string(_address));
    result.append("\nvalue = " + (*_values.begin()).toString());
  }

  return result;
}

std::vector<uint8_t> ModbusResponse::toRaw() const {
  std::vector<uint8_t> result;
  result.reserve(6);

  result.push_back(_slaveID);
  result.push_back(_functionCode);

  if (functionType() == utils::Read) {
    if (_values[0].isCoil()) {
      result.push_back(
          (_registersNumber / 8) +
          (_registersNumber % 8 == 0 ? 0 : 1)); // number of bytes to follow
      auto end = result.size() - 1;
      for (std::size_t i = 0; i < _values.size(); i++) {
        if (i % 8 == 0) {
          result.push_back(0x00);
          end++;
        }
        result[end] |= _values[i].coil() << (i % 8);
      }
    } else {
      result.push_back(_registersNumber * 2); // number of bytes to follow
      for (auto _value : _values) {
        const auto raw = reinterpret_cast<const uint8_t *>(&_value.reg());
        result.push_back(raw[1]);
        result.push_back(raw[0]);
      }
    }
  } else {
    auto raw = reinterpret_cast<const uint8_t *>(&_address);
    result.push_back(raw[1]);
    result.push_back(raw[0]);

    if (functionType() == utils::WriteSingle) {
      if (_values[0].isCoil()) {
        result.push_back(_values[0].coil() ? 0xFF : 0x00);
        result.push_back(0x00);
      } else {
        raw = reinterpret_cast<const uint8_t *>(&_values[0].reg());
        result.push_back(raw[1]);
        result.push_back(raw[0]);
      }
    } else {
      raw = reinterpret_cast<const uint8_t *>(&_registersNumber);
      result.push_back(raw[1]);
      result.push_back(raw[0]);
    }
  }

  return result;
}

void ModbusResponse::from(const ModbusRequest &req) {
  if (functionType() == utils::Read) {
    _address = req.registerAddress();
    if (_registersNumber > req.numberOfRegisters()) {
      _registersNumber = req.numberOfRegisters();
      _values.resize(req.numberOfRegisters());
    }
  } else if (functionType() == utils::WriteMultiple) {
    _values = req.registerValues();
    _values.resize(_registersNumber);
  }
}

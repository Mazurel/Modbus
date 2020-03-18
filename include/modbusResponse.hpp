//
// Created by mateusz on 06.01.2020.
//

#ifndef PROTOCOLCONVERTER_MODBUSRESPONSE_HPP
#define PROTOCOLCONVERTER_MODBUSRESPONSE_HPP

#include <cstdint>
#include <vector>
#include <stdexcept>
#include <string>
#include <algorithm>

#include "modbusRequest.hpp"
#include "modbusUtils.hpp"
#include "modbusCell.hpp"
#include "modbusException.hpp"

namespace MB
{
class ModbusResponse
{
private:
    uint8_t _slaveID;
    utils::MBFunctionCode _functionCode;

    uint16_t _address;
    uint16_t _registersNumber;

    std::vector<ModbusCell> _values;

    /**
     * @brief
     * Constructs Response from raw data
     * @note
     * 1) if CRC = true input data needs to contain 2 CRC bytes on back (used in RS)
     * @note
     * 2) This is private constructor, you need to use fromRaw or fromRawCRC
     * @param inputData - Is vector of bytes that will be be interpreted, whereas
     * based on CRC parameter method performs CRC calculation and throws exception if it is invalid
     * @throws ModbusException
     **/
    explicit ModbusResponse(std::vector<uint8_t> inputData, bool CRC = false);

public:
    static ModbusResponse fromRaw(std::vector<uint8_t> inputData) { return ModbusResponse(inputData); }
    static ModbusResponse fromRawCRC(std::vector<uint8_t> inputData) { return ModbusResponse(inputData, true); }

    ModbusResponse(uint8_t slaveId = 0, utils::MBFunctionCode functionCode = static_cast<utils::MBFunctionCode>(0),
                   uint16_t address = 0, uint16_t registersNumber = 0,
                   std::vector<ModbusCell> values = {});

    ModbusResponse(const ModbusResponse &) = default;

    [[nodiscard]] std::string toString() const;
    [[nodiscard]] std::vector<uint8_t> toRaw() const;
    // Fills all data from associated request
    void from(const ModbusRequest&);

    [[nodiscard]] utils::MBFunctionType functionType() const{
        return utils::functionType(_functionCode);
    }

    [[nodiscard]] utils::MBFunctionRegisters functionRegisters() const {
        return utils::functionRegister(_functionCode);
    }

    [[nodiscard]] uint8_t slaveID() const {
        return _slaveID;
    }
    [[nodiscard]] utils::MBFunctionCode functionCode() const {
        return _functionCode;
    }
    [[nodiscard]] uint16_t registerAddress() const {
        return _address;
    }
    [[nodiscard]] uint16_t numberOfRegisters() const {
        return _registersNumber;
    }
    [[nodiscard]] const std::vector<ModbusCell> & registerValues() const {
        return _values;
    }

    void setSlaveId(uint8_t slaveId) {
        _slaveID = slaveId;
    }
    void setFunctionCode(utils::MBFunctionCode functionCode) {
        _functionCode = functionCode;
    }
    void setAddress(uint16_t address) {
        _address = address;
    }
    void setRegistersNumber(uint16_t registersNumber) {
        _registersNumber = registersNumber;
        _values.resize(registersNumber);
    }
    void setValues(const std::vector<ModbusCell> &values) {
        _values = values;
    }
};
}


#endif //PROTOCOLCONVERTER_MODBUSRESPONSE_HPP

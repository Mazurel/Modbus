//
// Created by mateusz on 07.01.2020.
//

#ifndef PROTOCOLCONVERTER_MODBUSEXCEPTION_HPP
#define PROTOCOLCONVERTER_MODBUSEXCEPTION_HPP

#include <cstdint>
#include <vector>
#include <stdexcept>
#include <cstring>
#include <optional>
#include <memory>

#include "modbusUtils.hpp"

namespace MB
{
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
     * @param CRC based on this param method performs CRC calculation and throws exception if it is invalid
     * @note if CRC = true input data needs to contain 2 CRC bytes on back (used in RS)
     * */
    explicit ModbusException(const std::vector<uint8_t>& inputData, bool CRC = false) noexcept;


    // Constructs Exception based on error code, function code and slaveId
    explicit ModbusException(utils::MBErrorCode errorCode, uint8_t slaveId, utils::MBFunctionCode functionCode = utils::Undefined)
                                                                                        noexcept :
                                                                                        _slaveId(slaveId),
                                                                                        _validSlave(true),
                                                                                        _errorCode(errorCode),
                                                                                        _functionCode(functionCode)
                                                                                        {}


    // Constructs Exception based on error code (with no slaveId specified)
    explicit ModbusException(utils::MBErrorCode errorCode) noexcept : _slaveId(0xFF), _validSlave(false),
                                                                      _errorCode(errorCode), _functionCode(utils::Undefined){}

    /*
     * Check if there is Modbus error in raw modbus input
     * NOTE: this method doesn't detect invalid byte order, byte order is checked at ModbusRequest/ModbusResponse
     * */
    static bool exist(const std::vector<uint8_t>& inputData) noexcept
    {
        if (inputData.size() < 2) //todo Figure out better solution to such mistake
            return false;

        return inputData[1] & 0b10000000;
    }

    /*
     *  Returns attached SlaveID
     *  NOTE: it is worth to check if slaveId is specified with isSlaveValid()
     * */
    [[nodiscard]] uint8_t slaveID() const noexcept {
        return _slaveId;
    }


    // Checks if SlaveID is specified
    [[nodiscard]] bool isSlaveValid() const noexcept {
        return _validSlave;
    }

    // Sets SlaveID
    void setSlaveID(uint8_t slaveId) noexcept
    {
        _validSlave = true;
        _slaveId = slaveId;
    }

    // Returns detected error code
    [[nodiscard]] utils::MBErrorCode getErrorCode() const noexcept {
        return _errorCode;
    }

    /*
     * This function is less optimal, it is just to be compatible with std::excepetion
     * You should preferably use toString()
     */
    [[nodiscard]] const char * what () const noexcept override {
        auto og = toString();
        char* str = new char[og.size()];
        stpcpy(str , og.c_str());
        return str;
    }

    //Returns string representation of object
    [[nodiscard]] std::string toString() const noexcept;
    // Converts object to modbus byte representation
    [[nodiscard]] std::vector<uint8_t> toRaw() const noexcept;

    [[nodiscard]] utils::MBFunctionCode functionCode() const noexcept
    {
        return _functionCode;
    }

    void setFunctionCode(utils::MBFunctionCode functionCode) noexcept
    {
        _functionCode = functionCode;
    }

};
}


#endif //PROTOCOLCONVERTER_MODBUSEXCEPTION_HPP
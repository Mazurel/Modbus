#pragma once

#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <vector>

#include "ModbusUtils.h"

namespace MB {

/// @brief ModbusException类表示Modbus异常，派生自std::exception。它只是标准错误代码和一些自定义代码的包装。
class ModbusException : public std::exception
{
  public:
    /// @brief Constructs Exception from raw data
    /// @param inputData is vector of bytes that will be be interpreted
    /// @param CRC based on this param method performs CRC calculation and throws exception if it is invalid
    /// @note if CRC = true input data needs to contain 2 CRC bytes on back (used in RS)
    explicit ModbusException(const std::vector<uint8_t> &input_data, bool crc = false) noexcept;

    //! Constructs Exception based on error code, function code and slaveId
    explicit ModbusException(Utils::MBErrorCode error_code, uint8_t slave_id = 0xFF,
                             Utils::MBFunctionCode function_code = Utils::Undefined) noexcept
        : _slave_id(slave_id), _valid_slave(true), _error_code(error_code), _function_code(function_code)
    {
    }

    /// @brief Check if there is Modbus error in raw modbus input
    /// @note this method doesn't detect invalid byte order, byte order is checked at @see ModbusRequest/ @see
    /// ModbusResponse
    /// @param input_data
    /// @return bool
    static bool exist(const std::vector<uint8_t> &input_data) noexcept
    {
        if (input_data.size() < 2) {  // TODO Figure out better solution to such mistake
            return false;
        }

        return input_data[1] & 0b10000000;
    }

    /// @brief 返回附加的从属id
    /// @note it is worth to check if slave id is specified with is_slave_valid()
    /// @return uint8_t
    [[nodiscard]] uint8_t slave_id() const noexcept { return _slave_id; }

    /// @brief Checks if slave id is specified
    /// @return bool
    [[nodiscard]] bool is_slave_valid() const noexcept { return _valid_slave; }

    /// @brief  Sets slave id
    /// @param slave_id
    void set_slave_id(uint8_t slave_id) noexcept
    {
        _valid_slave = true;
        _slave_id = slave_id;
    }

    /// @brief Returns detected error code
    /// @return
    [[nodiscard]] Utils::MBErrorCode get_error_code() const noexcept { return _error_code; }

    /// @brief 这个函数不是最优的，它只是为了与std:: exception兼容。您最好使用to_string()
    /// @return
    [[nodiscard]] const char *what() const noexcept override;

    /// @brief 返回对象的字符串表示形式
    /// @return
    [[nodiscard]] std::string to_string() const noexcept;

    /// @brief 将对象转换为modbus字节表示
    /// @return
    [[nodiscard]] std::vector<uint8_t> to_raw() const noexcept;

    [[nodiscard]] Utils::MBFunctionCode function_code() const noexcept { return _function_code; }

    void set_function_code(Utils::MBFunctionCode function_code) noexcept { _function_code = function_code; }

  private:
    uint8_t _slave_id{0};
    bool _valid_slave{false};
    Utils::MBErrorCode _error_code;
    Utils::MBFunctionCode _function_code{Utils::MBFunctionCode::Undefined};
};

}  // namespace MB

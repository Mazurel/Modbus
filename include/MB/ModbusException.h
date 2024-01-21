#pragma once

#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>

#include "ModbusUtils.h"

namespace MB {

/// @brief ModbusException类表示Modbus异常，派生自std::exception。它只是标准错误代码和一些自定义代码的包装。
class ModbusException : public std::exception
{
  public:
    /// @brief 从原始数据构造Exception
    /// @param[in] input_data 将被解释的字节向量
    /// @param[in] crc 基于此参数方法执行CRC计算，如果无效则抛出异常
    /// @note 如果CRC = true输入数据需要包含2个CRC字节在背面(在RS中使用)
    explicit ModbusException(const std::vector<uint8_t> &input_data, bool crc = false) noexcept;

    /// @brief 基于错误码、函数码和从站id构造Exception
    /// @param[in] error_code
    /// @param[in] slave_id
    /// @param[in] function_code
    explicit ModbusException(Utils::MBErrorCode error_code, uint8_t slave_id = 0xFF,
                             Utils::MBFunctionCode function_code = Utils::Undefined) noexcept
        : _slave_id(slave_id), _valid_slave(true), _error_code(error_code), _function_code(function_code)
    {
    }

    /// @brief 检查Modbus输入是否有Modbus错误
    /// @note 这个方法不会检测到无效的字节顺序，字节顺序在 @see ModbusRequest / @see ModbusResponse中检查。
    /// @param input_data
    /// @return bool
    /// @retval true 存在
    /// @retval false 不存在
    static bool exist(const std::vector<uint8_t> &input_data) noexcept
    {
        if (input_data.size() < 2) {  // TODO 找出解决这种错误的更好办法
            return false;
        }

        return input_data[1] & 0b10000000;
    }

    /// @brief 返回附加的从属id
    /// @note it is worth to check if slave id is specified with is_slave_valid()
    /// @return uint8_t
    [[nodiscard]] uint8_t slave_id() const noexcept { return _slave_id; }

    /// @brief 检查从站是否有效
    /// @return bool
    [[nodiscard]] bool is_slave_valid() const noexcept { return _valid_slave; }

    /// @brief 设置从站id
    /// @param slave_id
    void set_slave_id(uint8_t slave_id) noexcept
    {
        _valid_slave = true;
        _slave_id = slave_id;
    }

    /// @brief 返回检测到的错误代码
    /// @return Utils::MBErrorCode
    [[nodiscard]] Utils::MBErrorCode get_error_code() const noexcept { return _error_code; }

    /// @brief 返回错误消息内容
    /// @return const char*
    [[nodiscard]] const char *what() const noexcept override;

    /// @brief 返回异常的字符串表示形式
    /// @return std::string
    [[nodiscard]] std::string to_string() const noexcept;

    /// @brief 将对象转换为modbus字节表示
    /// @return std::vector<uint8_t>
    [[nodiscard]] std::vector<uint8_t> to_raw() const noexcept;

    /// @brief
    /// @return Utils::MBFunctionCode
    [[nodiscard]] Utils::MBFunctionCode function_code() const noexcept { return _function_code; }

    /// @brief Set the function code object
    /// @param[in] function_code
    void set_function_code(Utils::MBFunctionCode function_code) noexcept { _function_code = function_code; }

  private:
    uint8_t _slave_id{0};
    bool _valid_slave{false};
    Utils::MBErrorCode _error_code;
    Utils::MBFunctionCode _function_code{Utils::MBFunctionCode::Undefined};

    mutable std::string _error_message{};
};

}  // namespace MB

#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

#include "ModbusCell.hpp"
#include "ModbusException.hpp"

namespace MB {

/// @brief 这个类表示Modbus响应，它允许用户以各种方式操作和显示它。
class ModbusRequest
{
  public:
    /// @brief 从原始数据构造请求
    /// @note 1) 如果CRC = true输入数据需要包含2个CRC字节在背面(在RS中使用)
    /// @note 2) 这是私有构造函数，您需要使用 @see from_raw或 @see from_raw_crc
    /// @param input_data 是将被解释的字节向量，而基于CRC参数方法执行CRC计算并抛出异常，如果它是无效的
    /// @param crc
    /// @throws ModbusException
    explicit ModbusRequest(const std::vector<uint8_t> &input_data, bool crc = false) noexcept(false);

    /// @brief 从原始数据构造请求
    /// @param input_data 是一个字节向量，将被解释
    /// @throws ModbusException
    static ModbusRequest from_raw(const std::vector<uint8_t> &input_data) noexcept(false)
    {
        return ModbusRequest(input_data);
    }

    /// @brief 从原始数据构造请求并检查它的CRC
    /// @param input_data 是一个字节向量，将被解释
    /// @throws ModbusException
    /// @note 此方法执行CRC检查，可能会对无效的CRC抛出 @see ModbusException
    static ModbusRequest from_raw_crc(const std::vector<uint8_t> &input_data)
    {
        return ModbusRequest(input_data, true);
    }

    /// @brief 简单的构造函数，允许创建“虚拟”ModbusResponse对象。在某些情况下可能有用。
    /// @param slave_id
    /// @param function_code
    /// @param address
    /// @param registers_number
    /// @param values
    explicit ModbusRequest(uint8_t slave_id = 0,
                           Utils::MBFunctionCode function_code = static_cast<Utils::MBFunctionCode>(0),
                           uint16_t address = 0, uint16_t registers_number = 0,
                           std::vector<ModbusCell> values = {}) noexcept;

    ModbusRequest(const ModbusRequest &) = default;

    /// @brief 返回对象的字符串表示形式
    [[nodiscard]] std::string to_string() const noexcept;

    /// @brief 返回对象的原始字节表示，为modbus通信做好准备
    [[nodiscard]] std::vector<uint8_t> to_raw() const noexcept;

    /// @brief Returns function type based on Modbus function code
    [[nodiscard]] Utils::MBFunctionType function_type() const noexcept { return Utils::function_type(_function_code); }
    /// @brief Returns register type based on Modbus function code
    [[nodiscard]] Utils::MBFunctionRegisters function_registers() const noexcept
    {
        return Utils::function_register(_function_code);
    }

    [[nodiscard]] uint8_t slave_id() const { return _slave_id; }
    [[nodiscard]] Utils::MBFunctionCode function_code() const { return _function_code; }
    [[nodiscard]] uint16_t register_address() const { return _address; }
    [[nodiscard]] uint16_t number_of_registers() const { return _registers_number; }
    [[nodiscard]] const std::vector<ModbusCell> &register_values() const { return _values; }

    void set_slave_id(uint8_t slave_id) { _slave_id = slave_id; }
    void set_function_code(Utils::MBFunctionCode function_code) { _function_code = function_code; }
    void set_address(uint16_t address) { _address = address; }
    void set_registers_number(uint16_t registers_number)
    {
        _registers_number = registers_number;
        _values.resize(registers_number);
    }
    void set_values(const std::vector<ModbusCell> &values) { _values = values; }

  private:
    uint8_t _slave_id;
    Utils::MBFunctionCode _function_code;

    uint16_t _address;
    uint16_t _registers_number;

    std::vector<ModbusCell> _values;
};

}  // namespace MB

#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

#include "ModbusCell.hpp"
#include "ModbusException.hpp"
#include "ModbusRequest.hpp"
#include "ModbusUtils.hpp"

namespace MB {

/// @brief 该类表示Modbus响应，并允许用户以各种方式对其进行操作和表示
class ModbusResponse
{
  public:
    /// @brief 从原始数据构造响应
    /// @note 1) 如果CRC = true输入数据需要包含2个CRC字节在背面(在RS中使用)
    /// @note 2) 这是私有构造函数，你需要使用from_raw或from_raw_crc
    /// @param[in] input_data 是将被解释的字节向量，而基于CRC参数方法执行CRC计算并抛出异常，如果它是无效的
    /// @param[in] crc
    /// @throws ModbusException
    explicit ModbusResponse(std::vector<uint8_t> input_data, bool crc = false);

    /// @brief 从原始数据构造响应
    /// @param[in] input_data 是一个将被解释的字节向量
    /// @return
    /// @throws ModbusException
    static ModbusResponse from_raw(std::vector<uint8_t> input_data) { return ModbusResponse(std::move(input_data)); }

    /// @brief 从原始数据构造请求并检查它的CRC
    /// @note 此方法执行CRC检查，可能会对无效的CRC抛出ModbusException
    /// @param[in] input_data 是一个将被解释的字节向量
    /// @return
    /// @throws ModbusException
    static ModbusResponse from_raw_crc(std::vector<uint8_t> input_data)
    {
        return ModbusResponse(std::move(input_data), true);
    }

    /// @brief 简单的构造函数，允许创建“虚拟”ModbusResponse对象。在某些情况下可能有用。
    /// @param[in] slave_id
    /// @param[in] function_code
    /// @param[in] address
    /// @param[in] registers_number
    /// @param[in] values
    ModbusResponse(uint8_t slave_id = 0, Utils::MBFunctionCode function_code = static_cast<Utils::MBFunctionCode>(0),
                   uint16_t address = 0, uint16_t registers_number = 0, std::vector<ModbusCell> values = {});

    ModbusResponse(const ModbusResponse &) = default;

    /// @brief Converts object to it's string representation
    /// @return std::string
    [[nodiscard]] std::string to_string() const;

    /// @brief
    /// @return std::vector<uint8_t>
    [[nodiscard]] std::vector<uint8_t> to_raw() const;

    /// @brief 填充来自关联请求的所有数据
    void from(const ModbusRequest &);

    [[nodiscard]] Utils::MBFunctionType function_type() const { return Utils::function_type(_function_code); }

    [[nodiscard]] Utils::MBFunctionRegisters function_registers() const
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

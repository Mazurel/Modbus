#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>
#include <variant>

#include "Export.h"

namespace MB {

/// @brief 类表示单个modbus cell，它是coil(bool)或reg(uint16_t)。
class MODBUS_EXPORT ModbusCell
{
  public:
    /// @brief 构造寄存器类型为0的单元格。
    constexpr ModbusCell() = default;

    /// @brief 用寄存器值构造单元格。
    /// @param reg 寄存器值
    constexpr ModbusCell(uint16_t reg) : _value(reg){};

    /// @brief 用线圈值构造单元。
    /// @param coil 线圈值
    constexpr ModbusCell(bool coil) : _value(coil){};

    ModbusCell(const ModbusCell &other) noexcept = default;

    ModbusCell &operator=(const ModbusCell &other) noexcept = default;
    ModbusCell &operator=(ModbusCell &&other) noexcept = default;

    /// @brief 单元格构造的静态包装器
    /// @param reg Requested value.
    /// @return New cell.
    [[nodiscard]] static ModbusCell init_reg(uint16_t reg) { return {reg}; }

    /// @brief Static wrapper for cell construction.
    /// @param coil Requested value.
    /// @return New cell.
    [[nodiscard]] static ModbusCell init_coil(bool coil) { return {coil}; }

    /// @brief Checks if cell is coil.
    /// @return Boolean representing result.
    [[nodiscard]] bool is_coil() const { return std::holds_alternative<bool>(_value); };

    /// @brief Checks if cell is register.
    /// @return Boolean representing result.
    [[nodiscard]] bool is_reg() const { return std::holds_alternative<uint16_t>(_value); };

    /// @brief Returns coil value, changes cell type if necessary.
    /// @return Reference to coil value.
    bool &coil()
    {
        if (is_reg()) {
            _value = static_cast<bool>(std::get<uint16_t>(_value));
        }
        return std::get<bool>(_value);
    };

    /// @brief Returns register value, changes cell type if necessary.
    /// @return Reference to coil value.
    uint16_t &reg()
    {
        if (is_coil()) {
            _value = static_cast<uint16_t>(std::get<bool>(_value));
        }
        return std::get<uint16_t>(_value);
    };

    /// @brief Returns coil value.
    /// @return Reference to coil value.
    /// @throws bad_variant_access - When cell is of different type as requested.
    [[nodiscard]] const bool &coil() const { return std::get<bool>(_value); };

    /// @brief Returns register value.
    /// @return Reference to coil value.
    /// @throws bad_variant_access - When cell is of different type as requested.
    [[nodiscard]] const uint16_t &reg() const { return std::get<uint16_t>(_value); };

    /// @brief Creates string representation of the cell.
    /// @return String representation.
    [[nodiscard]] std::string to_string() const noexcept
    {
        return is_coil() ? ((coil()) ? "true" : "false") : std::to_string(reg());
    }

  private:
    std::variant<uint16_t, bool> _value;
};

}  // namespace MB

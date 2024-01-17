#include "ModbusException.hpp"

namespace MB {

// Construct Modbus exception from raw data
ModbusException::ModbusException(const std::vector<uint8_t> &input_data, bool crc) noexcept
{
    if (input_data.size() != ((crc) ? 5 : 3)) {
        _slave_id = 0xFF;
        _function_code = Utils::Undefined;
        _valid_slave = false;
        _error_code = Utils::InvalidByteOrder;
        return;
    }

    _slave_id = input_data[0];
    _function_code = static_cast<Utils::MBFunctionCode>(0b01111111 & input_data[1]);
    _valid_slave = true;
    _error_code = static_cast<Utils::MBErrorCode>(input_data[2]);

    if (crc) {
        auto crc = *reinterpret_cast<const uint16_t *>(&input_data[3]);
        auto calculated_crc = Utils::calculate_crc(input_data.data(), 3);

        if (crc != calculated_crc) {
            _error_code = Utils::ErrorCodeCRCError;
        }
    }
}

// Returns string representation of exception
std::string ModbusException::to_string() const noexcept
{
    std::string res;
    res += "Error on slave ";
    if (_valid_slave) {
        res += std::to_string(_slave_id);
    } else {
        res += "Unknown";
    }
    res += " - ";
    res += Utils::modbus_error_code_to_str(_error_code);
    if (_function_code != MB::Utils::Undefined) {
        res += " ( on function: ";
        res += Utils::modbus_function_to_str(_function_code);
        res += " )";
    }
    return res;
}

std::vector<uint8_t> ModbusException::to_raw() const noexcept
{
    std::vector<uint8_t> result(3);

    result[0] = _slave_id;
    result[1] = static_cast<uint8_t>(_error_code | 0b10000000);
    result[2] = static_cast<uint8_t>(_function_code);

    return result;
}

}  // namespace MB

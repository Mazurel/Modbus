#include "ModbusRequest.h"

#include <algorithm>
#include <sstream>

#include "ModbusUtils.h"

namespace MB {

ModbusRequest::ModbusRequest(uint8_t slave_id, Utils::MBFunctionCode function_code, uint16_t address,
                             uint16_t registers_number, std::vector<ModbusCell> values) noexcept
    : _slave_id(slave_id)
    , _function_code(function_code)
    , _address(address)
    , _registers_number(registers_number)
    , _values(std::move(values))
{
    // Force proper modbuscell type
    switch (function_registers()) {
        case Utils::OutputCoils:
        case Utils::InputContacts:
            std::for_each(_values.begin(), _values.end(), [](ModbusCell &cell) -> void { cell.coil(); });
            break;
        case Utils::HoldingRegisters:
        case Utils::InputRegisters:
            std::for_each(_values.begin(), _values.end(), [](ModbusCell &cell) -> void { cell.reg(); });
            break;
    }
}

ModbusRequest::ModbusRequest(const std::vector<uint8_t> &input_data, bool crc)
{
    try {
        if (input_data.size() < 3) {
            throw ModbusException(Utils::InvalidByteOrder);
        }

        _slave_id = input_data[0];
        _function_code = static_cast<Utils::MBFunctionCode>(input_data[1]);
        _address = Utils::big_endian_conv(&input_data[2]);

        int crc_index = -1;
        int8_t follow;

        switch (_function_code) {
            case Utils::ReadDiscreteOutputCoils:
            case Utils::ReadDiscreteInputContacts:
            case Utils::ReadAnalogOutputHoldingRegisters:
            case Utils::ReadAnalogInputRegisters:
                _registers_number = Utils::big_endian_conv(&input_data[4]);
                _values = {};
                crc_index = 3 * 2;
                break;
            case Utils::WriteSingleDiscreteOutputCoil:
                _registers_number = 1;
                _values = {ModbusCell::init_coil(input_data[4] == 0xFF)};
                crc_index = 3 * 2;
                break;
            case Utils::WriteSingleAnalogOutputRegister:
                _registers_number = 1;
                _values = {Utils::big_endian_conv(&input_data[4])};
                crc_index = 3 * 2;
                break;
            case Utils::WriteMultipleDiscreteOutputCoils:
                _registers_number = Utils::big_endian_conv(&input_data[4]);
                follow = (int8_t)input_data[6];
                _values = std::vector<ModbusCell>(_registers_number);
                for (uint16_t i = 0; i < _registers_number; i++) {
                    _values[i].coil() = input_data[7 + (i / 8)] & (1 << (i % 8));
                }
                crc_index = 6 + follow + 1;
                break;
            case Utils::WriteMultipleAnalogOutputHoldingRegisters:
                _registers_number = Utils::big_endian_conv(&input_data[4]);
                follow = (int8_t)input_data[6];
                _values = std::vector<ModbusCell>(_registers_number);
                for (uint16_t i = 0; i < _registers_number; i++) {
                    _values[i].reg() = Utils::big_endian_conv(&input_data[i * 2 + 7]);
                }
                crc_index = 6 + follow + 1;
                break;
            default:
                throw ModbusException(Utils::InvalidByteOrder);
        }

        _values.resize(_registers_number);

        if (crc) {
            if (crc_index == -1 || static_cast<size_t>(crc_index) + 2 > input_data.size()) {
                throw ModbusException(Utils::InvalidByteOrder);
            }

            auto recv_crc = *reinterpret_cast<const uint16_t *>(&input_data[crc_index]);
            auto my_crc = Utils::calculate_crc(input_data.data(), crc_index);

            if (recv_crc != my_crc) {
                throw ModbusException(Utils::InvalidCRC, _slave_id);
            }
        }
    } catch (const ModbusException &ex) {
        throw ex;
    } catch (const std::exception & /*ex*/) {
        throw ModbusException(Utils::InvalidByteOrder);
    }
}

std::string ModbusRequest::to_string() const noexcept
{
    std::stringstream result;

    result << Utils::modbus_function_to_str(_function_code) << ", from slave " + std::to_string(_slave_id);

    if (function_type() != Utils::WriteSingle) {
        result << ", starting from address " + std::to_string(_address)
               << ", on " + std::to_string(_registers_number) + " registers";
        if (function_type() == Utils::WriteMultiple) {
            result << "\n values = { ";
            for (std::size_t i = 0; i < _values.size(); i++) {
                result << _values[i].to_string() + " , ";
                if (i >= 3) {
                    result << " , ... ";
                    break;
                }
            }
            result << "}";
        }
    } else {
        result << ", starting from address " + std::to_string(_address)
               << "\nvalue = " + (*_values.begin()).to_string();
    }

    return result.str();
}

std::vector<uint8_t> ModbusRequest::to_raw() const noexcept
{
    std::vector<uint8_t> result;
    result.reserve(6);

    result.push_back(_slave_id);
    result.push_back(_function_code);
    Utils::push_uint16(result, _address);

    if (function_type() != Utils::WriteSingle) {
        Utils::push_uint16(result, _registers_number);
    }

    if (_function_code == Utils::WriteMultipleAnalogOutputHoldingRegisters) {
        result.push_back(number_of_registers() * 2);
    } else if (_function_code == Utils::WriteMultipleDiscreteOutputCoils) {
        result.push_back((_registers_number / 8) + (_registers_number % 8 == 0 ? 0 : 1));
    }

    if (function_type() == Utils::WriteMultiple) {
        if (_values[0].is_reg()) {
            for (auto value : _values) {
                Utils::push_uint16(result, value.reg());
            }
        } else {
            size_t end = result.size() - 1;
            for (std::size_t i = 0; i < _values.size(); i++) {
                if (i % 8 == 0) {
                    result.push_back(0x00);
                    end++;
                }
                result[end] |= _values[i].coil() << (i % 8);
            }
        }
    } else if (function_type() == Utils::WriteSingle) {
        if (_values[0].is_reg()) {
            Utils::push_uint16(result, _values[0].reg());
        } else {
            result.push_back(_values[0].coil() ? 0xFF : 0x00);
            result.push_back(0x00);
        }
    }

    return result;
}

}  // namespace MB

#include "ModbusResponse.hpp"

#include <algorithm>
#include <sstream>

#include "ModbusUtils.hpp"

namespace MB {

ModbusResponse::ModbusResponse(uint8_t slave_id, Utils::MBFunctionCode function_code, uint16_t address,
                               uint16_t registers_number, std::vector<ModbusCell> values)
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

ModbusResponse::ModbusResponse(std::vector<uint8_t> input_data, bool crc)
{
    try {
        if (input_data.size() < 3) {
            throw ModbusException(Utils::InvalidByteOrder);
        }

        _slave_id = input_data[0];
        _function_code = static_cast<Utils::MBFunctionCode>(input_data[1]);

        if (function_type() != Utils::Read) {
            _address = Utils::big_endian_conv(&input_data[2]);
        }

        int crc_index = -1;
        uint8_t bytes;

        switch (_function_code) {
            case Utils::ReadDiscreteOutputCoils:
            case Utils::ReadDiscreteInputContacts:
                bytes = input_data[2];
                _registers_number = bytes * 8;
                _values = std::vector<ModbusCell>(_registers_number);
                for (auto i = 0; i < _registers_number; i++) {
                    _values[i].coil() = input_data[3 + (i / 8)] & (1 << (i % 8));
                }
                crc_index = 2 + bytes + 1;
                break;
            case Utils::ReadAnalogOutputHoldingRegisters:
            case Utils::ReadAnalogInputRegisters:
                bytes = input_data[2];
                _registers_number = bytes / 2;
                for (auto i = 0; i < bytes / 2; i++) {
                    _values.emplace_back(Utils::big_endian_conv(&input_data[3 + (i * 2)]));
                }
                crc_index = 2 + bytes + 1;
                break;
            case Utils::WriteSingleDiscreteOutputCoil:
                _registers_number = 1;
                _address = Utils::big_endian_conv(&input_data[2]);
                _values = {ModbusCell::init_coil(input_data[4] == 0xFF)};
                crc_index = 6;
                break;
            case Utils::WriteSingleAnalogOutputRegister:
                _registers_number = 1;
                _address = Utils::big_endian_conv(&input_data[2]);
                _values = {ModbusCell::init_reg(Utils::big_endian_conv(&input_data[4]))};
                crc_index = 6;
                break;
            case Utils::WriteMultipleDiscreteOutputCoils:
            case Utils::WriteMultipleAnalogOutputHoldingRegisters:
                _address = Utils::big_endian_conv(&input_data[2]);
                _registers_number = Utils::big_endian_conv(&input_data[4]);
                crc_index = 6;
                break;
            default:
                throw ModbusException(Utils::InvalidByteOrder);
        }

        _values.resize(_registers_number);

        if (crc) {
            if (crc_index == -1 || static_cast<size_t>(crc_index) + 2 > input_data.size())
                throw ModbusException(Utils::InvalidByteOrder);

            auto recieved_crc = *reinterpret_cast<const uint16_t *>(&input_data[crc_index]);
            auto my_crc = Utils::calculate_crc(input_data.data(), crc_index);

            if (recieved_crc != my_crc) {
                throw ModbusException(Utils::InvalidCRC, _slave_id);
            }
        }
    } catch (const ModbusException &ex) {
        throw ex;
    } catch (const std::exception & /*ex*/) {
        throw ModbusException(Utils::InvalidByteOrder);
    }
}

std::string ModbusResponse::to_string() const
{
    std::stringstream result;

    result << Utils::modbus_function_to_str(_function_code) << ", from slave " + std::to_string(_slave_id);

    if (function_type() != Utils::WriteSingle) {
        result << ", starting from address " + std::to_string(_address)
               << ", on " + std::to_string(_registers_number) + " registers";
        if (function_type() == Utils::WriteMultiple) {
            result << "\n values = { ";
            for (decltype(_values)::size_type i = 0; i < _values.size(); i++) {
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

std::vector<uint8_t> ModbusResponse::to_raw() const
{
    std::vector<uint8_t> result;
    result.reserve(6);

    result.push_back(_slave_id);
    result.push_back(_function_code);

    if (function_type() == Utils::Read) {
        if (_values[0].is_coil()) {
            result.push_back((_registers_number / 8) +
                             (_registers_number % 8 == 0 ? 0 : 1));  // number of bytes to follow
            auto end = result.size() - 1;
            for (std::size_t i = 0; i < _values.size(); i++) {
                if (i % 8 == 0) {
                    result.push_back(0x00);
                    end++;
                }
                result[end] |= _values[i].coil() << (i % 8);
            }
        } else {
            result.push_back(_registers_number * 2);  // number of bytes to follow
            for (auto value : _values) {
                Utils::push_uint16(result, value.reg());
            }
        }
    } else {
        Utils::push_uint16(result, _address);

        if (function_type() == Utils::WriteSingle) {
            if (_values[0].is_coil()) {
                result.push_back(_values[0].coil() ? 0xFF : 0x00);
                result.push_back(0x00);
            } else {
                Utils::push_uint16(result, _values[0].reg());
            }
        } else {
            Utils::push_uint16(result, _registers_number);
        }
    }

    return result;
}

void ModbusResponse::from(const ModbusRequest &req)
{
    if (function_type() == Utils::Read) {
        _address = req.register_address();
        if (_registers_number > req.number_of_registers()) {
            _registers_number = req.number_of_registers();
            _values.resize(req.number_of_registers());
        }
    } else if (function_type() == Utils::WriteMultiple) {
        _values = req.register_values();
        _values.resize(_registers_number);
    }
}

}  // namespace MB

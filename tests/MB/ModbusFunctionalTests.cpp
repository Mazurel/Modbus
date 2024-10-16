// Modbus for c++ <https://github.com/Mazurel/Modbus>
// Copyright (c) 2024 Mateusz Mazur aka Mazurel
// Licensed under: MIT License <http://opensource.org/licenses/MIT>

#include "MB/modbusCell.hpp"
#include "MB/modbusException.hpp"
#include "MB/modbusRequest.hpp"
#include "MB/modbusResponse.hpp"
#include "MB/modbusUtils.hpp"

#include "gtest/gtest.h"
#include <cstddef>
#include <ios>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace MB;

class ModBusFunctional : public ::testing::Test {
  public:
    std::string vectorIntoString(const std::vector<uint8_t> &packet) const;
};

std::string ModBusFunctional::vectorIntoString(const std::vector<uint8_t> &packet) const {
    std::stringstream stream;
    stream << std::hex; // format numbers as hex
    stream << "{";
    for (std::size_t i = 0; i < packet.size(); i++) {
        stream << "0x" << std::setw(2) << std::setfill('0')
               << static_cast<int>(packet[i]);
        if (i + 2 <= packet.size()) {
            stream << ", ";
        }
    }
    stream << "}";
    return stream.str();
}

TEST_F(ModBusFunctional, RequestWithBigAddress) {
    constexpr auto TEST_SLAVE_ID        = 1;
    constexpr auto TEST_FUNCTION_CODE   = MB::utils::ReadDiscreteOutputCoils;
    constexpr auto TEST_ADDRESS         = 301;
    constexpr auto TEST_REGISTER_NUMBER = 10;

    std::vector<uint8_t> packedRequest = ModbusRequest(TEST_SLAVE_ID, TEST_FUNCTION_CODE,
                                                       TEST_ADDRESS, TEST_REGISTER_NUMBER)
                                             .toRaw();

    std::cout << "Packed request: " << this->vectorIntoString(packedRequest) << std::endl;

    // Sanity check
    {
        auto request = ModbusRequest::fromRaw(packedRequest);
        ASSERT_EQ(request.slaveID(), TEST_SLAVE_ID);
        ASSERT_EQ(request.functionCode(), TEST_FUNCTION_CODE);
        ASSERT_EQ(request.registerAddress(), TEST_ADDRESS);
        ASSERT_EQ(request.numberOfRegisters(), TEST_REGISTER_NUMBER);
    }

    ASSERT_EQ(packedRequest[0], TEST_SLAVE_ID);
    ASSERT_EQ(packedRequest[1], TEST_FUNCTION_CODE);
    ASSERT_EQ(packedRequest[2], (TEST_ADDRESS & 0xFF00) >> 8);
    ASSERT_EQ(packedRequest[3], (TEST_ADDRESS & 0x00FF) >> 0);
    ASSERT_EQ(packedRequest[4], (TEST_REGISTER_NUMBER & 0xFF00) >> 8);
    ASSERT_EQ(packedRequest[5], (TEST_REGISTER_NUMBER & 0x00FF) >> 0);
}

TEST_F(ModBusFunctional, InvalidRequest) {
    constexpr auto TEST_SLAVE_ID        = 1;
    constexpr auto TEST_FUNCTION_CODE   = MB::utils::WriteMultipleDiscreteOutputCoils;
    constexpr auto TEST_ADDRESS         = 10;
    constexpr auto TEST_REGISTER_NUMBER = 2;
    constexpr auto TEST_REGISTER_NUMBER_TOO_BIG = 10;
    const auto TEST_VALUES = {ModbusCell::initCoil(false), ModbusCell::initCoil(true)};

    // With some values but too big register number
    const auto correctResponse =
        ModbusRequest(TEST_SLAVE_ID, TEST_FUNCTION_CODE, TEST_ADDRESS,
                      TEST_REGISTER_NUMBER, TEST_VALUES);
    const auto requestWithIncorrectValues =
        ModbusResponse(TEST_SLAVE_ID, TEST_FUNCTION_CODE, TEST_ADDRESS,
                       TEST_REGISTER_NUMBER_TOO_BIG, TEST_VALUES);

    // Sanity check
    {
        ASSERT_NO_THROW(auto _ = correctResponse.toRaw());
        ASSERT_EQ(correctResponse.slaveID(), TEST_SLAVE_ID);
        ASSERT_EQ(correctResponse.functionCode(), TEST_FUNCTION_CODE);
        ASSERT_EQ(correctResponse.registerAddress(), TEST_ADDRESS);
        ASSERT_EQ(correctResponse.numberOfRegisters(), TEST_REGISTER_NUMBER);
    }

    {
        try {
            auto _ = requestWithIncorrectValues.toRaw();
        } catch (const ModbusException &ex) {
            ASSERT_EQ(ex.getErrorCode(), utils::NumberOfValuesInvalid)
                << "Got exception: " << utils::mbErrorCodeToStr(ex.getErrorCode());
        }
    }
}

TEST_F(ModBusFunctional, InvalidResponse) {
    constexpr auto TEST_SLAVE_ID                = 1;
    constexpr auto TEST_FUNCTION_CODE           = MB::utils::ReadAnalogInputRegisters;
    constexpr auto TEST_ADDRESS                 = 10;
    constexpr auto TEST_REGISTER_NUMBER         = 10;
    constexpr auto TEST_REGISTER_NUMBER_TOO_BIG = 300;
    const auto TEST_VALUES                      = {ModbusCell::initCoil(false)};

    // With some values but too big register number
    const auto correctResponse =
        ModbusResponse(TEST_SLAVE_ID, TEST_FUNCTION_CODE, TEST_ADDRESS,
                       TEST_REGISTER_NUMBER, TEST_VALUES);
    const auto responseWithTooBigRegisterNumber =
        ModbusResponse(TEST_SLAVE_ID, TEST_FUNCTION_CODE, TEST_ADDRESS,
                       TEST_REGISTER_NUMBER_TOO_BIG, TEST_VALUES);
    const auto responseWithNoValues = ModbusResponse(TEST_SLAVE_ID, TEST_FUNCTION_CODE,
                                                     TEST_ADDRESS, TEST_REGISTER_NUMBER);

    // Sanity check
    {
        ASSERT_NO_THROW(auto _ = correctResponse.toRaw());
        ASSERT_EQ(correctResponse.slaveID(), TEST_SLAVE_ID);
        ASSERT_EQ(correctResponse.functionCode(), TEST_FUNCTION_CODE);
        ASSERT_EQ(correctResponse.registerAddress(), TEST_ADDRESS);
        ASSERT_EQ(correctResponse.numberOfRegisters(), TEST_REGISTER_NUMBER);
    }

    {
        try {
            auto _ = responseWithTooBigRegisterNumber.toRaw();
        } catch (const ModbusException &ex) {
            ASSERT_EQ(ex.getErrorCode(), utils::NumberOfRegistersInvalid)
                << "Number of bytes to follow: "
                << responseWithTooBigRegisterNumber.numberOfBytesToFollow()
                << "Got exception: " << utils::mbErrorCodeToStr(ex.getErrorCode());
        }
    }

    {
        try {
            auto _ = responseWithNoValues.toRaw();
        } catch (const ModbusException &ex) {
            ASSERT_EQ(ex.getErrorCode(), utils::NumberOfValuesInvalid)
                << "Got exception: " << utils::mbErrorCodeToStr(ex.getErrorCode());
        }
    }
}

TEST_F(ModBusFunctional, RequestToResponse) {
    constexpr auto TEST_SLAVE_ID        = 1;
    constexpr auto TEST_FUNCTION_CODE   = MB::utils::ReadDiscreteOutputCoils;
    constexpr auto TEST_ADDRESS         = 301;
    constexpr auto TEST_REGISTER_NUMBER = 10;

    auto request  = ModbusRequest(TEST_SLAVE_ID, TEST_FUNCTION_CODE, TEST_ADDRESS,
                                  TEST_REGISTER_NUMBER);
    auto response = ModbusResponse::from(request);

    ASSERT_EQ(request.functionCode(), response.functionCode());
    ASSERT_EQ(request.numberOfRegisters(), response.numberOfRegisters());
    ASSERT_EQ(request.registerAddress(), response.registerAddress());
}

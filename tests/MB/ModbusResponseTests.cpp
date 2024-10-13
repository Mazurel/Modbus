// Modbus for c++ <https://github.com/Mazurel/Modbus>
// Copyright (c) 2020 Mateusz Mazur aka Mazurel
// Licensed under: MIT License <http://opensource.org/licenses/MIT>

#include "MB/modbusRequest.hpp"
#include "MB/modbusResponse.hpp"
#include "gtest/gtest.h"

using namespace MB;

class ModBusResponse : public ::testing::Test {
  protected:
    ModBusResponse() {}

    // Testing data from https://www.simplymodbus.ca/
    virtual void SetUp() {
        fn1Data  = {0x11, 0x01, 0x05, 0xCD, 0x6B, 0xB2, 0x0E, 0x1B, 0x45, 0xE6};
        fn2Data  = {0x11, 0x02, 0x03, 0xAC, 0xDB, 0x35, 0x20, 0x18};
        fn3Data  = {0x11, 0x03, 0x06, 0xAE, 0x41, 0x56, 0x52, 0x43, 0x40, 0x49, 0xAD};
        fn4Data  = {0x11, 0x04, 0x02, 0x00, 0x0A, 0xF8, 0xF4};
        fn5Data  = {0x11, 0x05, 0x00, 0xAC, 0xFF, 0x00, 0x4E, 0x8B};
        fn6Data  = {0x11, 0x06, 0x00, 0x01, 0x00, 0x03, 0x9A, 0x9B};
        fn15Data = {0x11, 0x0F, 0x00, 0x13, 0x00, 0x0A, 0x26, 0x99};
        fn16Data = {0x11, 0x10, 0x00, 0x01, 0x00, 0x02, 0x12, 0x98};
    }

    virtual void TearDown() {}

    std::vector<uint8_t> fn1Data;
    std::vector<uint8_t> fn2Data;
    std::vector<uint8_t> fn3Data;
    std::vector<uint8_t> fn4Data;
    std::vector<uint8_t> fn5Data;
    std::vector<uint8_t> fn6Data;
    std::vector<uint8_t> fn15Data;
    std::vector<uint8_t> fn16Data;
};

TEST_F(ModBusResponse, CRC) {
    EXPECT_NO_THROW(ModbusResponse com = ModbusResponse::fromRawCRC(fn1Data));
    EXPECT_NO_THROW(ModbusResponse com = ModbusResponse::fromRawCRC(fn2Data));
    EXPECT_NO_THROW(ModbusResponse com = ModbusResponse::fromRawCRC(fn3Data));
    EXPECT_NO_THROW(ModbusResponse com = ModbusResponse::fromRawCRC(fn4Data));
    EXPECT_NO_THROW(ModbusResponse com = ModbusResponse::fromRawCRC(fn5Data));
    EXPECT_NO_THROW(ModbusResponse com = ModbusResponse::fromRawCRC(fn6Data));
    EXPECT_NO_THROW(ModbusResponse com = ModbusResponse::fromRawCRC(fn15Data));
    EXPECT_NO_THROW(ModbusResponse com = ModbusResponse::fromRawCRC(fn16Data));
}

TEST_F(ModBusResponse, Function1) {
    ModbusResponse com = ModbusResponse::fromRaw(fn1Data);

    EXPECT_EQ(0x11, com.slaveID());
    EXPECT_EQ(0x01, com.functionCode());
    EXPECT_TRUE(com.registerValues()[0].isCoil());
    EXPECT_TRUE(com.registerValues()[0].coil());
}

TEST_F(ModBusResponse, Function2) {
    ModbusResponse com = ModbusResponse::fromRaw(fn2Data);

    EXPECT_EQ(0x11, com.slaveID());
    EXPECT_EQ(0x02, com.functionCode());
    EXPECT_TRUE(com.registerValues()[0].isCoil());
    EXPECT_FALSE(com.registerValues()[0].coil());
    EXPECT_TRUE(com.registerValues()[9].coil());
    EXPECT_FALSE(com.registerValues()[10].coil());
}

TEST_F(ModBusResponse, Function3) {
    ModbusResponse com = ModbusResponse::fromRaw(fn3Data);

    EXPECT_EQ(0x11, com.slaveID());
    EXPECT_EQ(0x03, com.functionCode());
    EXPECT_EQ(0x03, com.numberOfRegisters());
    EXPECT_TRUE(com.registerValues()[0].isReg());
    EXPECT_EQ(0xAE41, com.registerValues()[0].reg());
    EXPECT_EQ(0x4340, com.registerValues()[2].reg());
}

TEST_F(ModBusResponse, Function4) {
    ModbusResponse com = ModbusResponse::fromRaw(fn4Data);

    EXPECT_EQ(0x11, com.slaveID());
    EXPECT_EQ(0x04, com.functionCode());
    EXPECT_EQ(0x01, com.numberOfRegisters());
    EXPECT_TRUE(com.registerValues()[0].isReg());
    EXPECT_EQ(0x000A, com.registerValues()[0].reg());
}

TEST_F(ModBusResponse, Function5) {
    ModbusResponse com = ModbusResponse::fromRaw(fn5Data);

    EXPECT_EQ(0x11, com.slaveID());
    EXPECT_EQ(0x05, com.functionCode());
    EXPECT_EQ(0xAC, com.registerAddress());
    EXPECT_EQ(1, com.numberOfRegisters());
    EXPECT_TRUE(com.registerValues()[0].coil());
}

TEST_F(ModBusResponse, Function6) {
    ModbusResponse com = ModbusResponse::fromRaw(fn6Data);

    EXPECT_EQ(0x11, com.slaveID());
    EXPECT_EQ(0x06, com.functionCode());
    EXPECT_EQ(0x01, com.registerAddress());
    EXPECT_EQ(1, com.numberOfRegisters());
    EXPECT_TRUE(com.registerValues()[0].isReg());
    EXPECT_TRUE(com.registerValues()[0].reg() == 0x03);
}

TEST_F(ModBusResponse, Function15) {
    ModbusResponse com = ModbusResponse::fromRaw(fn15Data);

    EXPECT_EQ(0x11, com.slaveID());
    EXPECT_EQ(0x0F, com.functionCode());
    EXPECT_EQ(0x13, com.registerAddress());
    EXPECT_EQ(0x0A, com.numberOfRegisters());
}

TEST_F(ModBusResponse, Function16) {
    ModbusResponse com = ModbusResponse::fromRaw(fn16Data);

    EXPECT_EQ(0x11, com.slaveID());
    EXPECT_EQ(0x10, com.functionCode());
    EXPECT_EQ(0x01, com.registerAddress());
    EXPECT_EQ(0x02, com.numberOfRegisters());
}

TEST_F(ModBusResponse, RawTest) {
    auto eq = [](const std::vector<uint8_t> &dataA,
                 const std::vector<uint8_t> &dataB) -> bool {
        if (dataA.size() - 2 != dataB.size()) // -2 is for ignoring CRC
            return false;

        for (std::vector<uint8_t>::size_type i = 0; i < dataB.size(); i++) {
            if (dataA[i] != dataB[i])
                return false;
        }
        return true;
    };

    EXPECT_TRUE(eq(fn1Data, ModbusResponse::fromRaw(fn1Data).toRaw()));
    EXPECT_TRUE(eq(fn2Data, ModbusResponse::fromRaw(fn2Data).toRaw()));
    EXPECT_TRUE(eq(fn3Data, ModbusResponse::fromRaw(fn3Data).toRaw()));
    EXPECT_TRUE(eq(fn4Data, ModbusResponse::fromRaw(fn4Data).toRaw()));
    EXPECT_TRUE(eq(fn5Data, ModbusResponse::fromRaw(fn5Data).toRaw()));
    EXPECT_TRUE(eq(fn6Data, ModbusResponse::fromRaw(fn6Data).toRaw()));
    EXPECT_TRUE(eq(fn15Data, ModbusResponse::fromRaw(fn15Data).toRaw()));
    EXPECT_TRUE(eq(fn16Data, ModbusResponse::fromRaw(fn16Data).toRaw()));
}

TEST_F(ModBusResponse, ConstructorsCheck) {
    ModbusResponse com = ModbusResponse::fromRaw(fn1Data);

    ModbusResponse com2(0x11, static_cast<utils::MBFunctionCode>(0x01), 0x13, 0x25);

    EXPECT_TRUE(com.slaveID() == com2.slaveID());
    EXPECT_TRUE(com.functionCode() == com2.functionCode());
}

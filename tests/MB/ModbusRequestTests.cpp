// Modbus for c++ <https://github.com/Mazurel/Modbus>
// Copyright (c) 2020 Mateusz Mazur aka Mazurel
// Licensed under: MIT License <http://opensource.org/licenses/MIT>

#include "MB/modbusRequest.hpp"
#include "gtest/gtest.h"

using namespace MB;

class ModBusRequest : public ::testing::Test {
  protected:
    ModBusRequest() {}

    // Testing data from https://www.simplymodbus.ca/
    virtual void SetUp() {
        fn1Data  = {0x11, 0x01, 0x00, 0x13, 0x00, 0x25, 0x0E, 0x84};
        fn2Data  = {0x11, 0x02, 0x00, 0xC4, 0x00, 0x16, 0xBA, 0xA9};
        fn3Data  = {0x11, 0x03, 0x00, 0x6B, 0x00, 0x03, 0x76, 0x87};
        fn4Data  = {0x11, 0x04, 0x00, 0x08, 0x00, 0x01, 0xB2, 0x98};
        fn5Data  = {0x11, 0x05, 0x00, 0xAC, 0xFF, 0x00, 0x4E, 0x8B};
        fn6Data  = {0x11, 0x06, 0x00, 0x01, 0x00, 0x03, 0x9A, 0x9B};
        fn15Data = {0x11, 0x0F, 0x00, 0x13, 0x00, 0x0A, 0x02, 0xCD, 0x01, 0xBF, 0x0B};
        fn16Data = {0x11, 0x10, 0x00, 0x01, 0x00, 0x02, 0x04,
                    0x00, 0x0A, 0x01, 0x02, 0xC6, 0xF0};
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

TEST_F(ModBusRequest, CRC) {
    EXPECT_NO_THROW(ModbusRequest com = ModbusRequest::fromRawCRC(fn1Data));
    EXPECT_NO_THROW(ModbusRequest com = ModbusRequest::fromRawCRC(fn2Data));
    EXPECT_NO_THROW(ModbusRequest com = ModbusRequest::fromRawCRC(fn3Data));
    EXPECT_NO_THROW(ModbusRequest com = ModbusRequest::fromRawCRC(fn4Data));
    EXPECT_NO_THROW(ModbusRequest com = ModbusRequest::fromRawCRC(fn5Data));
    EXPECT_NO_THROW(ModbusRequest com = ModbusRequest::fromRawCRC(fn6Data));
    EXPECT_NO_THROW(ModbusRequest com = ModbusRequest::fromRawCRC(fn15Data));
    EXPECT_NO_THROW(ModbusRequest com = ModbusRequest::fromRawCRC(fn16Data));
}

TEST_F(ModBusRequest, Function1) {
    ModbusRequest com = ModbusRequest::fromRaw(fn1Data);

    EXPECT_EQ(0x11, com.slaveID());
    EXPECT_EQ(0x01, com.functionCode());
    EXPECT_EQ(0x13, com.registerAddress());
    EXPECT_EQ(0x25, com.numberOfRegisters());
}

TEST_F(ModBusRequest, Function2) {
    ModbusRequest com = ModbusRequest::fromRaw(fn2Data);

    EXPECT_EQ(0x11, com.slaveID());
    EXPECT_EQ(0x02, com.functionCode());
    EXPECT_EQ(0xC4, com.registerAddress());
    EXPECT_EQ(0x16, com.numberOfRegisters());
}

TEST_F(ModBusRequest, Function3) {
    ModbusRequest com = ModbusRequest::fromRaw(fn3Data);

    EXPECT_EQ(0x11, com.slaveID());
    EXPECT_EQ(0x03, com.functionCode());
    EXPECT_EQ(0x6B, com.registerAddress());
    EXPECT_EQ(0x03, com.numberOfRegisters());
}

TEST_F(ModBusRequest, Function4) {
    ModbusRequest com = ModbusRequest::fromRaw(fn4Data);

    EXPECT_EQ(0x11, com.slaveID());
    EXPECT_EQ(0x04, com.functionCode());
    EXPECT_EQ(0x08, com.registerAddress());
    EXPECT_EQ(0x01, com.numberOfRegisters());
}

TEST_F(ModBusRequest, Function5) {
    ModbusRequest com = ModbusRequest::fromRaw(fn5Data);

    EXPECT_EQ(0x11, com.slaveID());
    EXPECT_EQ(0x05, com.functionCode());
    EXPECT_EQ(0xAC, com.registerAddress());
    EXPECT_EQ(1, com.numberOfRegisters());
    EXPECT_TRUE(com.registerValues()[0].coil());
}

TEST_F(ModBusRequest, Function6) {
    ModbusRequest com = ModbusRequest::fromRaw(fn6Data);

    EXPECT_EQ(0x11, com.slaveID());
    EXPECT_EQ(0x06, com.functionCode());
    EXPECT_EQ(0x01, com.registerAddress());
    EXPECT_EQ(1, com.numberOfRegisters());
    EXPECT_TRUE(com.registerValues()[0].isReg());
    EXPECT_TRUE(com.registerValues()[0].reg() == 0x03);
}

TEST_F(ModBusRequest, Function15) {
    ModbusRequest com = ModbusRequest::fromRaw(fn15Data);

    EXPECT_EQ(0x11, com.slaveID());
    EXPECT_EQ(0x0F, com.functionCode());
    EXPECT_EQ(0x13, com.registerAddress());
    EXPECT_EQ(0x0A, com.numberOfRegisters());
    EXPECT_TRUE(com.registerValues()[0].isCoil());
    EXPECT_TRUE(com.registerValues()[0].coil());
    EXPECT_FALSE(com.registerValues()[1].coil());
    EXPECT_TRUE(com.registerValues()[2].coil());
}

TEST_F(ModBusRequest, Function16) {
    ModbusRequest com = ModbusRequest::fromRaw(fn16Data);

    EXPECT_EQ(0x11, com.slaveID());
    EXPECT_EQ(0x10, com.functionCode());
    EXPECT_EQ(0x01, com.registerAddress());
    EXPECT_EQ(0x02, com.numberOfRegisters());
    EXPECT_TRUE(com.registerValues()[0].isReg());
    EXPECT_EQ(0x000A, com.registerValues()[0].reg());
    EXPECT_EQ(0x0102, com.registerValues()[1].reg());
}

TEST_F(ModBusRequest, RawTest) {
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

    EXPECT_TRUE(eq(fn1Data, ModbusRequest::fromRaw(fn1Data).toRaw()));
    EXPECT_TRUE(eq(fn2Data, ModbusRequest::fromRaw(fn2Data).toRaw()));
    EXPECT_TRUE(eq(fn3Data, ModbusRequest::fromRaw(fn3Data).toRaw()));
    EXPECT_TRUE(eq(fn4Data, ModbusRequest::fromRaw(fn4Data).toRaw()));
    EXPECT_TRUE(eq(fn5Data, ModbusRequest::fromRaw(fn5Data).toRaw()));
    EXPECT_TRUE(eq(fn6Data, ModbusRequest::fromRaw(fn6Data).toRaw()));
    EXPECT_TRUE(eq(fn15Data, ModbusRequest::fromRaw(fn15Data).toRaw()));
    EXPECT_TRUE(eq(fn16Data, ModbusRequest::fromRaw(fn16Data).toRaw()));
}

TEST_F(ModBusRequest, ConstructorsCheck) {
    ModbusRequest com = ModbusRequest::fromRaw(fn1Data);

    ModbusRequest com2(0x11, static_cast<utils::MBFunctionCode>(0x01), 0x13, 0x25);

    EXPECT_TRUE(com.slaveID() == com2.slaveID());
    EXPECT_TRUE(com.functionCode() == com2.functionCode());
    EXPECT_TRUE(com.registerAddress() == com2.registerAddress());
    EXPECT_TRUE(com.numberOfRegisters() == com2.numberOfRegisters());
}

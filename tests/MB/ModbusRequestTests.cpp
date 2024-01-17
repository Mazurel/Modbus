#include "MB/ModbusRequest.hpp"
#include "gtest/gtest.h"

using namespace MB;

class ModBusRequest : public ::testing::Test
{
  protected:
    ModBusRequest() {}

    // Testing data from https://www.simplymodbus.ca/
    virtual void SetUp()
    {
        fn1Data = {0x11, 0x01, 0x00, 0x13, 0x00, 0x25, 0x0E, 0x84};
        fn2Data = {0x11, 0x02, 0x00, 0xC4, 0x00, 0x16, 0xBA, 0xA9};
        fn3Data = {0x11, 0x03, 0x00, 0x6B, 0x00, 0x03, 0x76, 0x87};
        fn4Data = {0x11, 0x04, 0x00, 0x08, 0x00, 0x01, 0xB2, 0x98};
        fn5Data = {0x11, 0x05, 0x00, 0xAC, 0xFF, 0x00, 0x4E, 0x8B};
        fn6Data = {0x11, 0x06, 0x00, 0x01, 0x00, 0x03, 0x9A, 0x9B};
        fn15Data = {0x11, 0x0F, 0x00, 0x13, 0x00, 0x0A, 0x02, 0xCD, 0x01, 0xBF, 0x0B};
        fn16Data = {0x11, 0x10, 0x00, 0x01, 0x00, 0x02, 0x04, 0x00, 0x0A, 0x01, 0x02, 0xC6, 0xF0};
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

TEST_F(ModBusRequest, CRC)
{
    EXPECT_NO_THROW(ModbusRequest com = ModbusRequest::from_raw_crc(fn1Data));
    EXPECT_NO_THROW(ModbusRequest com = ModbusRequest::from_raw_crc(fn2Data));
    EXPECT_NO_THROW(ModbusRequest com = ModbusRequest::from_raw_crc(fn3Data));
    EXPECT_NO_THROW(ModbusRequest com = ModbusRequest::from_raw_crc(fn4Data));
    EXPECT_NO_THROW(ModbusRequest com = ModbusRequest::from_raw_crc(fn5Data));
    EXPECT_NO_THROW(ModbusRequest com = ModbusRequest::from_raw_crc(fn6Data));
    EXPECT_NO_THROW(ModbusRequest com = ModbusRequest::from_raw_crc(fn15Data));
    EXPECT_NO_THROW(ModbusRequest com = ModbusRequest::from_raw_crc(fn16Data));
}

TEST_F(ModBusRequest, Function1)
{
    ModbusRequest com = ModbusRequest::from_raw(fn1Data);

    EXPECT_EQ(0x11, com.slave_id());
    EXPECT_EQ(0x01, com.function_code());
    EXPECT_EQ(0x13, com.register_address());
    EXPECT_EQ(0x25, com.number_of_registers());
}

TEST_F(ModBusRequest, Function2)
{
    ModbusRequest com = ModbusRequest::from_raw(fn2Data);

    EXPECT_EQ(0x11, com.slave_id());
    EXPECT_EQ(0x02, com.function_code());
    EXPECT_EQ(0xC4, com.register_address());
    EXPECT_EQ(0x16, com.number_of_registers());
}

TEST_F(ModBusRequest, Function3)
{
    ModbusRequest com = ModbusRequest::from_raw(fn3Data);

    EXPECT_EQ(0x11, com.slave_id());
    EXPECT_EQ(0x03, com.function_code());
    EXPECT_EQ(0x6B, com.register_address());
    EXPECT_EQ(0x03, com.number_of_registers());
}

TEST_F(ModBusRequest, Function4)
{
    ModbusRequest com = ModbusRequest::from_raw(fn4Data);

    EXPECT_EQ(0x11, com.slave_id());
    EXPECT_EQ(0x04, com.function_code());
    EXPECT_EQ(0x08, com.register_address());
    EXPECT_EQ(0x01, com.number_of_registers());
}

TEST_F(ModBusRequest, Function5)
{
    ModbusRequest com = ModbusRequest::from_raw(fn5Data);

    EXPECT_EQ(0x11, com.slave_id());
    EXPECT_EQ(0x05, com.function_code());
    EXPECT_EQ(0xAC, com.register_address());
    EXPECT_EQ(1, com.number_of_registers());
    EXPECT_TRUE(com.register_values()[0].coil());
}

TEST_F(ModBusRequest, Function6)
{
    ModbusRequest com = ModbusRequest::from_raw(fn6Data);

    EXPECT_EQ(0x11, com.slave_id());
    EXPECT_EQ(0x06, com.function_code());
    EXPECT_EQ(0x01, com.register_address());
    EXPECT_EQ(1, com.number_of_registers());
    EXPECT_TRUE(com.register_values()[0].is_reg());
    EXPECT_TRUE(com.register_values()[0].reg() == 0x03);
}

TEST_F(ModBusRequest, Function15)
{
    ModbusRequest com = ModbusRequest::from_raw(fn15Data);

    EXPECT_EQ(0x11, com.slave_id());
    EXPECT_EQ(0x0F, com.function_code());
    EXPECT_EQ(0x13, com.register_address());
    EXPECT_EQ(0x0A, com.number_of_registers());
    EXPECT_TRUE(com.register_values()[0].is_coil());
    EXPECT_TRUE(com.register_values()[0].coil());
    EXPECT_FALSE(com.register_values()[1].coil());
    EXPECT_TRUE(com.register_values()[2].coil());
}

TEST_F(ModBusRequest, Function16)
{
    ModbusRequest com = ModbusRequest::from_raw(fn16Data);

    EXPECT_EQ(0x11, com.slave_id());
    EXPECT_EQ(0x10, com.function_code());
    EXPECT_EQ(0x01, com.register_address());
    EXPECT_EQ(0x02, com.number_of_registers());
    EXPECT_TRUE(com.register_values()[0].is_reg());
    EXPECT_EQ(0x000A, com.register_values()[0].reg());
    EXPECT_EQ(0x0102, com.register_values()[1].reg());
}

TEST_F(ModBusRequest, RawTest)
{
    auto eq = [](const std::vector<uint8_t> &dataA, const std::vector<uint8_t> &dataB) -> bool {
        if (dataA.size() - 2 != dataB.size())  // -2 is for ignoring CRC
            return false;

        for (std::vector<uint8_t>::size_type i = 0; i < dataB.size(); i++) {
            if (dataA[i] != dataB[i])
                return false;
        }
        return true;
    };

    EXPECT_TRUE(eq(fn1Data, ModbusRequest::from_raw(fn1Data).to_raw()));
    EXPECT_TRUE(eq(fn2Data, ModbusRequest::from_raw(fn2Data).to_raw()));
    EXPECT_TRUE(eq(fn3Data, ModbusRequest::from_raw(fn3Data).to_raw()));
    EXPECT_TRUE(eq(fn4Data, ModbusRequest::from_raw(fn4Data).to_raw()));
    EXPECT_TRUE(eq(fn5Data, ModbusRequest::from_raw(fn5Data).to_raw()));
    EXPECT_TRUE(eq(fn6Data, ModbusRequest::from_raw(fn6Data).to_raw()));
    EXPECT_TRUE(eq(fn15Data, ModbusRequest::from_raw(fn15Data).to_raw()));
    EXPECT_TRUE(eq(fn16Data, ModbusRequest::from_raw(fn16Data).to_raw()));
}

TEST_F(ModBusRequest, ConstructorsCheck)
{
    ModbusRequest com = ModbusRequest::from_raw(fn1Data);

    ModbusRequest com2(0x11, static_cast<Utils::MBFunctionCode>(0x01), 0x13, 0x25);

    EXPECT_TRUE(com.slave_id() == com2.slave_id());
    EXPECT_TRUE(com.function_code() == com2.function_code());
    EXPECT_TRUE(com.register_address() == com2.register_address());
    EXPECT_TRUE(com.number_of_registers() == com2.number_of_registers());
}

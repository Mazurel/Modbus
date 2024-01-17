#include "MB/ModbusRequest.hpp"
#include "MB/ModbusResponse.hpp"
#include "gtest/gtest.h"

using namespace MB;

class ModBusResponse : public ::testing::Test
{
  protected:
    ModBusResponse() {}

    // Testing data from https://www.simplymodbus.ca/
    virtual void SetUp()
    {
        fn1Data = {0x11, 0x01, 0x05, 0xCD, 0x6B, 0xB2, 0x0E, 0x1B, 0x45, 0xE6};
        fn2Data = {0x11, 0x02, 0x03, 0xAC, 0xDB, 0x35, 0x20, 0x18};
        fn3Data = {0x11, 0x03, 0x06, 0xAE, 0x41, 0x56, 0x52, 0x43, 0x40, 0x49, 0xAD};
        fn4Data = {0x11, 0x04, 0x02, 0x00, 0x0A, 0xF8, 0xF4};
        fn5Data = {0x11, 0x05, 0x00, 0xAC, 0xFF, 0x00, 0x4E, 0x8B};
        fn6Data = {0x11, 0x06, 0x00, 0x01, 0x00, 0x03, 0x9A, 0x9B};
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

TEST_F(ModBusResponse, CRC)
{
    EXPECT_NO_THROW(ModbusResponse com = ModbusResponse::from_raw_crc(fn1Data));
    EXPECT_NO_THROW(ModbusResponse com = ModbusResponse::from_raw_crc(fn2Data));
    EXPECT_NO_THROW(ModbusResponse com = ModbusResponse::from_raw_crc(fn3Data));
    EXPECT_NO_THROW(ModbusResponse com = ModbusResponse::from_raw_crc(fn4Data));
    EXPECT_NO_THROW(ModbusResponse com = ModbusResponse::from_raw_crc(fn5Data));
    EXPECT_NO_THROW(ModbusResponse com = ModbusResponse::from_raw_crc(fn6Data));
    EXPECT_NO_THROW(ModbusResponse com = ModbusResponse::from_raw_crc(fn15Data));
    EXPECT_NO_THROW(ModbusResponse com = ModbusResponse::from_raw_crc(fn16Data));
}

TEST_F(ModBusResponse, Function1)
{
    ModbusResponse com = ModbusResponse::from_raw(fn1Data);

    EXPECT_EQ(0x11, com.slave_id());
    EXPECT_EQ(0x01, com.function_code());
    EXPECT_TRUE(com.register_values()[0].is_coil());
    EXPECT_TRUE(com.register_values()[0].coil());
}

TEST_F(ModBusResponse, Function2)
{
    ModbusResponse com = ModbusResponse::from_raw(fn2Data);

    EXPECT_EQ(0x11, com.slave_id());
    EXPECT_EQ(0x02, com.function_code());
    EXPECT_TRUE(com.register_values()[0].is_coil());
    EXPECT_FALSE(com.register_values()[0].coil());
    EXPECT_TRUE(com.register_values()[9].coil());
    EXPECT_FALSE(com.register_values()[10].coil());
}

TEST_F(ModBusResponse, Function3)
{
    ModbusResponse com = ModbusResponse::from_raw(fn3Data);

    EXPECT_EQ(0x11, com.slave_id());
    EXPECT_EQ(0x03, com.function_code());
    EXPECT_EQ(0x03, com.number_of_registers());
    EXPECT_TRUE(com.register_values()[0].is_reg());
    EXPECT_EQ(0xAE41, com.register_values()[0].reg());
    EXPECT_EQ(0x4340, com.register_values()[2].reg());
}

TEST_F(ModBusResponse, Function4)
{
    ModbusResponse com = ModbusResponse::from_raw(fn4Data);

    EXPECT_EQ(0x11, com.slave_id());
    EXPECT_EQ(0x04, com.function_code());
    EXPECT_EQ(0x01, com.number_of_registers());
    EXPECT_TRUE(com.register_values()[0].is_reg());
    EXPECT_EQ(0x000A, com.register_values()[0].reg());
}

TEST_F(ModBusResponse, Function5)
{
    ModbusResponse com = ModbusResponse::from_raw(fn5Data);

    EXPECT_EQ(0x11, com.slave_id());
    EXPECT_EQ(0x05, com.function_code());
    EXPECT_EQ(0xAC, com.register_address());
    EXPECT_EQ(1, com.number_of_registers());
    EXPECT_TRUE(com.register_values()[0].coil());
}

TEST_F(ModBusResponse, Function6)
{
    ModbusResponse com = ModbusResponse::from_raw(fn6Data);

    EXPECT_EQ(0x11, com.slave_id());
    EXPECT_EQ(0x06, com.function_code());
    EXPECT_EQ(0x01, com.register_address());
    EXPECT_EQ(1, com.number_of_registers());
    EXPECT_TRUE(com.register_values()[0].is_reg());
    EXPECT_TRUE(com.register_values()[0].reg() == 0x03);
}

TEST_F(ModBusResponse, Function15)
{
    ModbusResponse com = ModbusResponse::from_raw(fn15Data);

    EXPECT_EQ(0x11, com.slave_id());
    EXPECT_EQ(0x0F, com.function_code());
    EXPECT_EQ(0x13, com.register_address());
    EXPECT_EQ(0x0A, com.number_of_registers());
}

TEST_F(ModBusResponse, Function16)
{
    ModbusResponse com = ModbusResponse::from_raw(fn16Data);

    EXPECT_EQ(0x11, com.slave_id());
    EXPECT_EQ(0x10, com.function_code());
    EXPECT_EQ(0x01, com.register_address());
    EXPECT_EQ(0x02, com.number_of_registers());
}

TEST_F(ModBusResponse, RawTest)
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

    EXPECT_TRUE(eq(fn1Data, ModbusResponse::from_raw(fn1Data).to_raw()));
    EXPECT_TRUE(eq(fn2Data, ModbusResponse::from_raw(fn2Data).to_raw()));
    EXPECT_TRUE(eq(fn3Data, ModbusResponse::from_raw(fn3Data).to_raw()));
    EXPECT_TRUE(eq(fn4Data, ModbusResponse::from_raw(fn4Data).to_raw()));
    EXPECT_TRUE(eq(fn5Data, ModbusResponse::from_raw(fn5Data).to_raw()));
    EXPECT_TRUE(eq(fn6Data, ModbusResponse::from_raw(fn6Data).to_raw()));
    EXPECT_TRUE(eq(fn15Data, ModbusResponse::from_raw(fn15Data).to_raw()));
    EXPECT_TRUE(eq(fn16Data, ModbusResponse::from_raw(fn16Data).to_raw()));
}

TEST_F(ModBusResponse, ConstructorsCheck)
{
    ModbusResponse com = ModbusResponse::from_raw(fn1Data);

    ModbusResponse com2(0x11, static_cast<Utils::MBFunctionCode>(0x01), 0x13, 0x25);

    EXPECT_TRUE(com.slave_id() == com2.slave_id());
    EXPECT_TRUE(com.function_code() == com2.function_code());
}

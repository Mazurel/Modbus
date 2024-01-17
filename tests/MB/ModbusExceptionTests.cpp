#include "MB/ModbusException.hpp"
#include "gtest/gtest.h"

TEST(ModbusException, Exists)
{
    EXPECT_TRUE(MB::ModbusException::exist({0x0A, 0x81, 0x02}));
    EXPECT_TRUE(MB::ModbusException::exist({0x01, MB::Utils::WriteMultipleDiscreteOutputCoils | 0b10000000, 0x02}));
}

TEST(ModbusException, Other)
{
    EXPECT_EQ(MB::ModbusException({0x0A, 0x82, 0x02}).function_code(), MB::Utils::ReadDiscreteInputContacts);
}

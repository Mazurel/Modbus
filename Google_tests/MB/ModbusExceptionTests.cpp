//
// Created by mateusz on 31.01.2020.
//

#include "gtest/gtest.h"
#include "modbusException.hpp"
#include "modbusUtils.hpp"

TEST(ModbusException , Exists)
{
    EXPECT_TRUE(MB::ModbusException::exist({ 0x0A, 0x81, 0x02 }));
    EXPECT_TRUE(MB::ModbusException::exist({ 0x01, MB::utils::WriteMultipleDiscreteOutputCoils | 0b10000000 , 0x02 }));
}

TEST(ModbusException, Other)
{
    EXPECT_EQ(MB::ModbusException({0x0A, 0x82, 0x02}).getFunctionCode(), MB::utils::ReadDiscreteInputContacts);
}
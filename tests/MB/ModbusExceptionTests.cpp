// Modbus for c++ <https://github.com/Mazurel/Modbus>
// Copyright (c) 2020 Mateusz Mazur aka Mazurel
// Licensed under: MIT License <http://opensource.org/licenses/MIT>

#include "MB/modbusException.hpp"
#include "gtest/gtest.h"

TEST(ModbusException, Exists) {
  EXPECT_TRUE(MB::ModbusException::exist({0x0A, 0x81, 0x02}));
  EXPECT_TRUE(MB::ModbusException::exist(
      {0x01, MB::utils::WriteMultipleDiscreteOutputCoils | 0b10000000, 0x02}));
}

TEST(ModbusException, Other) {
  EXPECT_EQ(MB::ModbusException({0x0A, 0x82, 0x02}).functionCode(),
            MB::utils::ReadDiscreteInputContacts);
}

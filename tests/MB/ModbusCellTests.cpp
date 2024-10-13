// Modbus for c++ <https://github.com/Mazurel/Modbus>
// Copyright (c) 2020 Mateusz Mazur aka Mazurel
// Licensed under: MIT License <http://opensource.org/licenses/MIT>

#include "MB/modbusCell.hpp"
#include "MB/modbusException.hpp"
#include "MB/modbusUtils.hpp"
#include "gtest/gtest.h"

TEST(ModbusCell, Manipulation) {
    auto cell1 = MB::ModbusCell::initReg(12);
    EXPECT_EQ(cell1.reg(), 12);
    EXPECT_TRUE(cell1.coil());
    cell1.coil() = false;
    EXPECT_FALSE(cell1.coil());
    EXPECT_EQ(cell1.reg(), 0);
    cell1.reg() = 3;
    EXPECT_EQ(cell1.reg(), 3);
}

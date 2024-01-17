#include "MB/ModbusCell.hpp"
#include "MB/ModbusException.hpp"
#include "MB/ModbusUtils.hpp"
#include "gtest/gtest.h"

TEST(ModbusCell, Manipulation)
{
    auto cell1 = MB::ModbusCell::init_reg(12);
    EXPECT_EQ(cell1.reg(), 12);
    EXPECT_TRUE(cell1.coil());
    cell1.coil() = false;
    EXPECT_FALSE(cell1.coil());
    EXPECT_EQ(cell1.reg(), 0);
    cell1.reg() = 3;
    EXPECT_EQ(cell1.reg(), 3);
}

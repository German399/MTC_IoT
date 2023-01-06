#include "ds1721_driver.h"

/**
    @file ds1721driver.c
    @author RedBees, LLC
    @copyright This file is distributed under the terms of the MIT license.
*/

void DS1721_StartConversion(uint8_t i2c_addr)
{
    i2c_write_with_pointer(I2C1,i2c_addr,0x51,0,NULL);
}

int8_t DS1721_ReadTemperature(uint8_t i2c_addr)
{
    uint8_t result;

    if (i2c_read_with_pointer(I2C1,i2c_addr,0xAA,1,&result)!=I2C_ERROR_NONE)
    {
        return 127;
    }
    else
    {
        return result;
    }

    return result;
}

uint8_t DS1721_ReadConfigurationRegister(uint8_t i2c_addr)
{
    uint8_t result;

    if (i2c_read_with_pointer(I2C1,i2c_addr,0xAC,1,&result)!=I2C_ERROR_NONE)
    {
        return 0x00;
    }
    else
    {
        return result;
    }
}

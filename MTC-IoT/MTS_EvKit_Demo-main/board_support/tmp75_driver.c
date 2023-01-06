#include "tmp75_driver.h"

 /**
    @file tmp75_driver.c
    @author RedBees, LLC
    @copyright This file is distributed under the terms of the MIT license.
 */

int8_t TMP75_ReadTemperatureCentigrade(uint8_t device_addr)
{
    uint8_t tmp[2];
    int8_t T;

    if (i2c_read_with_pointer(I2C1,device_addr,0,2,tmp)!=I2C_ERROR_NONE)
    {
        return -1;
    }
    else
    {
        T=(((int16_t)(tmp[0])<<8) | tmp[1])>>8;

        return T;
    }
}

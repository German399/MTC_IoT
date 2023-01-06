#ifndef DS1721_DRIVER_H_INCLUDED
#define DS1721_DRIVER_H_INCLUDED

#include "i2c_api.h"

/**
    @ingroup board_support
    @defgroup device_drivers Драйвера используемых устройств
    @{
    @defgroup tmp75_driver Драйвер термодатчика DS1721
    @{
*/

/**
    @file ds1721_driver.h
    @author RedBees, LLC
    @copyright This file is distributed under the terms of the MIT license.
*/

void DS1721_StartConversion(uint8_t i2c_addr);

int8_t DS1721_ReadTemperature(uint8_t i2c_addr);

uint8_t DS1721_ReadConfigurationRegister(uint8_t i2c_addr);


/**
    @}
    @}
*/

#endif /* DS1721_DRIVER_H_INCLUDED */

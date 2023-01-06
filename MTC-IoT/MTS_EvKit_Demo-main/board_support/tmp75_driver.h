#ifndef TMP75_DRIVER_H_INCLUDED
#define TMP75_DRIVER_H_INCLUDED

#include "i2c_api.h"

/**
    @ingroup board_support
    @defgroup device_drivers Драйвера используемых устройств
    @{
    @defgroup tmp75_driver Драйвер термодатчика TMP75
    @{
*/

/**
    @file tmp75_driver.h
    @author RedBees, LLC
    @copyright This file is distributed under the terms of the MIT license.
*/

/**
    @brief Читает значение регистра температуры из датчика TMP75

    @param [in] device_addr адрес датчика; установленный на плате датчик доступен по адресу LOCAL_TMP75_I2C_ADDR
    @return значение температуры в градусах Цельсия; в случае ошибки доступа возвращаемое значение равняется 0xFF
*/
int8_t TMP75_ReadTemperatureCentigrade(uint8_t device_addr);

/**
    @}
    @}
*/

#endif /* TMP75_DRIVER_H_INCLUDED */

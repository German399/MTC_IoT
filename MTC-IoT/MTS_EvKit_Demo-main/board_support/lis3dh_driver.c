#include "lis3dh_driver.h"

 /**
    @file lis3dh_driver.c
    @author RedBees, LLC
    @copyright This file is distributed under the terms of the MIT license.
 */

void __lis3dh_write_regsiter(uint8_t i2c_addr,uint8_t reg_addr,uint8_t reg_value)
{
    i2c_write_with_pointer(I2C1,i2c_addr,reg_addr,1,&reg_value);
}

uint8_t __lis3dh_read_register(uint8_t i2c_addr,uint8_t reg_addr)
{
    uint8_t reg_value;

    i2c_read_with_pointer(I2C1,i2c_addr,reg_addr,1,&reg_value);

    return reg_value;
}

uint8_t LIS3DH_ReadSignature(uint8_t i2c_addr)
{
    return __lis3dh_read_register(i2c_addr,LIS3DH_REG_WHOAMI);
}

void LIS3DH_EnableXYZ(uint8_t i2c_addr,uint8_t data_rate,uint8_t fullscale,uint8_t hi_res)
{
    __lis3dh_write_regsiter(i2c_addr,LIS3SH_REG_CTRL_REG1,((data_rate & 0x0F) << 4) | 0x07);
    __lis3dh_write_regsiter(i2c_addr,LIS3DH_REG_CTRL_REG4,((fullscale & 0x03) << 4) | ((hi_res & 0x01) << 3));
}

void LIS3DH_EnableDRDYonINT2(uint8_t i2c_addr)
{
    //Map XYZ data ready on internal interrupt source 1
    __lis3dh_write_regsiter(i2c_addr,LIS3DH_REG_CTRL_REG3,(1<<4));
    //Map internal interrupt source 1 to physical pin INT2
    __lis3dh_write_regsiter(i2c_addr,LIS3DH_REG_CTRL_REG6,(1<<6));
}

inline int16_t extend_sign_10bit(uint16_t value)
{
    if (value & (1<<9))
    {
        return value | 0xFC00;
    }
    else
    {
        return value;
    }
}

/*
Standard format is 10 bit left aligned:

              data_H                     data_L
    | D9 D8 D7 D6 D5 D4 D3 D2 | D1 D0  0  0  0  0  0  0 |

making normal 10-bit value of it.
*/
inline uint16_t collect_int_10bit(uint8_t data_L,uint8_t data_H)
{
    return ((((uint16_t)data_L) >> 6) | (((uint16_t)data_H) << 2));
}

void LIS3DH_ReadXYZ(uint8_t i2c_addr,int16_t *X,int16_t *Y,int16_t *Z)
{
    uint8_t data_XYZ[6];

    i2c_read_with_pointer(I2C1,i2c_addr,LIS3DH_XYZ_DATA_ADDR | LIS3DH_SEQ_READ_BIT,6,data_XYZ);

#ifndef INVERT_XY_AXES
    (*X) = extend_sign_10bit(collect_int_10bit(data_XYZ[0],data_XYZ[1]));
    (*Y) = extend_sign_10bit(collect_int_10bit(data_XYZ[2],data_XYZ[3]));
#else
    (*X) = -extend_sign_10bit(collect_int_10bit(data_XYZ[0],data_XYZ[1]));
    (*Y) = -extend_sign_10bit(collect_int_10bit(data_XYZ[2],data_XYZ[3]));
#endif
    (*Z) = extend_sign_10bit(collect_int_10bit(data_XYZ[4],data_XYZ[5]));
}

uint8_t LIS3DH_XYZ_DataReady(uint8_t i2c_addr)
{
    uint8_t reg_value;

    reg_value = __lis3dh_read_register(i2c_addr,LIS3DH_REG_STATUS_REG);

    if (reg_value & (1<<3))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

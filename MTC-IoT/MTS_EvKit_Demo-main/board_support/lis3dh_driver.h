#ifndef LIS3DH_DRIVER_H_INCLUDED
#define LIS3DH_DRIVER_H_INCLUDED

#include "i2c_api.h"

#include "stdint.h"

/**
    @ingroup device_drivers
    @{
    @defgroup lis3sh_driver ������� ������������� LIS3DH
    @{
*/

 /**
    @file lis3dh_driver.h
    @author RedBees, LLC
    @copyright This file is distributed under the terms of the MIT license.
 */

#define INVERT_XY_AXES

#define LIS3DH_READ_BIT     (1<<7)
#define LIS3DH_ADDR_INC_BIT (1<<6)

/**
    @brief �������� ���������� ������ � ���������
    @{
*/
#define LIS3DH_PWDN             0x00
#define LIS3DH_1Hz              0x01
#define LIS3DH_10Hz             0x02
#define LIS3DH_25Hz             0x03
#define LIS3DH_50Hz             0x04
#define LIS3DH_100Hz            0x05
#define LIS3DH_200Hz            0x06
#define LIS3DH_400Hz            0x07
/**
    @}
*/

/**
    @brief ������� ���������, ��������������� ����������� �������� �� ����
    @{
*/
#define LIS3DH_FULLSCALE_2g     0x00
#define LIS3DH_FULLSCALE_4g     0x01
#define LIS3DH_FULLSCALE_8g     0x02
#define LIS3DH_FULLSCALE_16g    0x03
/**
    @}
*/

/**
    @brief ������ ���������� ��������� �������������
    @{
*/
#define LIS3DH_REG_WHOAMI       0x0F
#define LIS3SH_REG_CTRL_REG1    0x20
#define LIS3DH_REG_CTRL_REG3    0x22
#define LIS3DH_REG_CTRL_REG4    0x23
#define LIS3DH_REG_CTRL_REG6    0x25
#define LIS3DH_REG_STATUS_REG   0x27
#define LIS3DH_XYZ_DATA_ADDR    0x28
/**
    @}
*/

#define LIS3DH_SEQ_READ_BIT      (1<<7)

/**
    @brief �������� ������������������ ��� �������������
    @param [in] i2c_addr ����� ���������� �� ���� I2C
    @return ����������������� ���; 0x33 ��� LIS3DH
*/
uint8_t LIS3DH_ReadSignature(uint8_t i2c_addr);
uint8_t LIS3DH_XYZ_DataReady(uint8_t i2c_addr);
/**
    @brief �������� �������� ��������� �� ����
*/
void LIS3DH_ReadXYZ(uint8_t i2c_addr,int16_t *X,int16_t *Y,int16_t *Z);
/**
    @brief ���������������� ��������� ������ �������������
*/
void LIS3DH_EnableXYZ(uint8_t i2c_addr,uint8_t data_rate,uint8_t fullscale,uint8_t hi_res);
void LIS3DH_EnableDRDYonINT2(uint8_t i2c_addr);

/**
    @}
    @}
*/

#endif /* LIS3DH_DRIVER_H_INCLUDED */

#ifndef GNSS_SUPPORT_H_INCLUDED
#define GNSS_SUPPORT_H_INCLUDED

#include "stm32l1xx.h"

/**
    @file gnss_support.h
    @author RedBees, LLC
    @copyright This file is distributed under the terms of the MIT license.
*/

/**
    @ingroup device_drivers
    @defgroup gnss_support ��� ��������� ������ ������ � GNSS-������
    @{
*/

#define GNSS_DATA_BUFFER_SIZE       1024

#ifndef NULL
#define NULL                        ((void *)0)
#endif

/**
    @brief �������� ������ � ��������������, ���������� �� GNSS-���������

    @param [out] str_out ������, � ������� ����� ����������� ������ � ��������������
    @param [in] max_length ����� ������������ ������; ����� ��������� �������� NULL, ���� ������ �� ���������
    @return 1 - ������ ������� �����������; 0 - ����� ������ ��� �� ��������

    @warning ����� ������ ������������ ���������� � �����, ������� �������������� ����� ������ ������ ���������� ������ ��������������.
    ����� �������, ����� GNSS_ReadLocationData() ���������� ������ ������, ������� ���� �������� ����� ����������� ������, � �� ����� ������ ���������� ������.
    ����� �������� ����� ������ ������, ������� ������� ������������� �������� ������� ������ ������� GNSS_ReadLocationData(NULL,0) � ����� ����� ���������,
    ����� ����������� ����� ���������� �������.
*/
uint8_t GNSS_ReadLocationData(uint8_t *str_out,uint16_t max_length);

/**
    @}
*/

#endif /* GNSS_SUPPORT_H_INCLUDED */

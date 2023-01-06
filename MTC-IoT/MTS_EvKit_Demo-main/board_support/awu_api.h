#ifndef AWU_API_H_INCLUDED
#define AWU_API_H_INCLUDED

/**
    @file awu_api.h
    @author RedBees, LLC
    @copyright This file is distributed under the terms of the MIT license.
*/

/**
    @ingroup interface_drivers
    @defgroup awu_api ��� ��������� ����� �������������� ����������� � ������ ����������� �����������������
    @{
*/

#include "stm32l1xx.h"

#define AWU_MAX_WAIT        2000000UL

/**
    @brief ������� ��������� ��������� ����� �������������� ����������� (AWU) ��� ������ � ������� ����������� ����������������� Stop.

    @warning ������� �� �������� ������������ ������������ ���������; ��� ��������� ������� ��������. ��� ��������� ������� ����������� �����������������
    ��������� ���������� ������ RTC, PWR � EXTI.

    @param [in] wakeup_timeout ������������ ����� ���������� � ������ Stop. ������������ �������� - 16000.
    @param [in] timeout_is_in_seconds ��������� �������. ���� ���� �������� ���������� � 1, ����� ������� � ��������, ����� � �������������.
*/
void AWU_Init(uint16_t wakeup_timeout_ms,uint8_t timeout_is_in_seconds);
/**
    @brief ������� ������������� ���������� ��������� � ��������� ���������� � ����� Stop. ����� �� ������ Stop �������������� �� ������� AWU.
    ����� ����� ���������� ��������� ������������ ������� � ������, ��������� �� ���� ��������.

    @note ���� ���� AWU �� ��� �������������� �������� ������� awu_init(), ����� awu_go_stop() ������������.
*/
void AWU_GoStop(void);

/**
    @}
*/

#endif /* AWU_API_H_INCLUDED */

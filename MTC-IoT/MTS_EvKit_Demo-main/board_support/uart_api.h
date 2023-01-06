#ifndef UART_API_H_INCLUDED
#define UART_API_H_INCLUDED

/**
    @ingroup board_support
    @defgroup interface_drivers ��� ��������� ������� �����������
    @brief �������� ��� ��������� �����������. ���������� ��������� � ���� �� ������ ��������� ��������� ��������� �����������, ���������� � ��������� ����,
    �������� ��� ��������� � ������� �� ��������� �� ������� ������������� ��������� ���������. ����� ����, ����������� ���������� API � ������������ ������, ���
    ������������ ����������� �� ��������� ����.
    @{
    @defgroup uart_api ��� ��������� ������ ����� USART
    @{
*/

 /**
    @file uart_api.h
    @author RedBees, LLC
    @copyright This file is distributed under the terms of the MIT license.
 */

#include "stm32l1xx.h"
#include "board_pins.h"
#include "port_macros.h"

/**
    @brief ��������� ������ USART ��� ������ � ��������� ���������.

    ��������� ����������� ��� ������ � ������ 8N1.

    @warning ������� �� �������� ������������ ������ USART � �� ����������� ����� �����-������. ���������������� ������ ��� ������ USART.

    @param [in] uart_inst ��������� �� ������ USART
    @param [in] baudrate �������� �������� ������, ���/�
    @param [in] sys_clk ������� ����, �� ������� ����������� ������������ USART
    @param [in] enable_rxne_int ��������� ��������� ���������� �� ������ �����
    @warning ������� �� �������� ��������� ���������� ������ NVIC, � ������ ��������� ��������� ������� �� ���������� ������ USART.
*/
void init_uart(USART_TypeDef* uart_inst,const uint32_t baudrate,const uint32_t sys_clk,uint8_t enable_rxne_int);

/**
    @brief �������� ���� ����� ��������� USART.
    @param [in] uart_inst ��������� �� ������ USART.
    @param [in] b ����, ������� ���������� ���������.
*/
void UART_SendByte(USART_TypeDef* uart_inst,uint8_t b);

/**
    @brief ������� ������ ����� ����� ��������� USART.
    @param [in] uart_inst ��������� �� ������ USART.
    @return �������� ����.
*/
uint8_t UART_WaitRxByte(USART_TypeDef* uart_inst);

///������������ ����� ������, ����� ������� � UART �������������� �������� UART_SendString()
#define UART_MAX_STRING_LENGTH          255
/**
    ����� � UART ������, ��������������� ��������� NULL (���������� C)
    @param [in] uart_inst ��������� �� ������ USART
    @param [in] str ������, ������� ��������� �������. ����� ������ ���������� ��������� UART_MAX_STRING_LENGTH �� ����������� ������������������.
*/
void UART_SendString(USART_TypeDef* uart_inst,const uint8_t *str);

/**
    @}
    @}
*/

#endif /* UART_API_H_INCLUDED */

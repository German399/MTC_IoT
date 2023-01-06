#ifndef BOARD_PINS_H_INCLUDED
#define BOARD_PINS_H_INCLUDED

/**
    @ingroup board_support
    @{
    @defgroup board_pins ������������� ������� ����������� �� �������� �� �����
    @{
    ��������� �������� � ���� <���������� �����>_<���� �����>. ����������� Arduino-����������� ������� ����� � �����������
    STM32L152RE ���������� �� ������ ����� Nucleo, ���������� �� ���� �����������.

    � I2C1 ���������� ������������ LIS3DH � ������ ����������� TMP75, ����������� �� �����; ����� ��� ���� �������� ��
    Arduino-����������� ������.
*/

 /**
    @file board_pins.h
    @author RedBees, LLC
    @copyright This file is distributed under the terms of the MIT license.
 */

#define USER_LED_PA         5
//13 for engineering sample board
#define USER_SW_PC          6

///USART1 ������������ ��� ���������� ������ printf()
#define USART1_TX_PA        9
#define USART1_RX_PA        10
///USART2 ������������ ��� ������ ������ �� ������ GNSS
#define USART2_TX_PA        2
#define USART2_RX_PA        3
///����� �������������� �������, ��������������� USART1/2/3, �������� ������������
#define USART123_AF_NO        7

///USART3 ��������� � ������ NB-IoT
#define USART3_TX_PC        10
#define USART3_RX_PC        11
#define USART3_AF_NO        7

#define I2C1_SCL_PB         8
#define I2C1_SDA_PB         9
///����� �������������� �������, ��������������� I2C1, �������� ������������
#define I2C1_AF_NO          4

///����� �����, ���������� ������� ������ NB-IoT
#define NB_PWR_ON_PC        8
///����� �����, ��������������� ������ NB-IoT
#define NB_RESET_PC         9
#define NB_RTS_PB           14
#define NB_VINT_PA          15

/**
    @ingroup board_pins
    @{
    @defgroup ardu_pins ����������� ������������ ������� ����������� ����������� ������� ������� Arduino UNO v3
	@brief � ���� �������������� ������ ���������, ���������� ��������������� �� �����, �� ��� ������ Arduino �������� ��� �������������. ������ ��������� ����.
    @{
*/

#define ARDU_A0_PA      0
#define ARDU_A1_PA      1
#define ARDU_A2_PA      4
#define ARDU_A3_PB      0
#define ARDU_A4_PC      1
#define ARDU_A5_PC      0

/** @warning � ������� ������������ ������������ ��� ������������ � ������� GNSS. */
#define ARDU_D0_PA      3
/** @warning � ������� ������������ ������������ ��� ������������ � ������� GNSS. */
#define ARDU_D1_PA      2
/** @warning ������������ ��� ������������ � �������� ���������������� ��������. */
#define ARDU_D2_PA      10

#define ARDU_D3_PB      3
#define ARDU_D4_PB      5
#define ARDU_D5_PB      4
#define ARDU_D6_PB      10
#define ARDU_D7_PA      8

/** @warning ������������ ��� ������������ � �������� ���������������� ��������. */
#define ARDU_D8_PA      9

#define ARDU_D9_PC      7
#define ARDU_D10_PB     6
#define ARDU_D11_PA     7
#define ARDU_D12_PA     6

/** @warning ��������� � ����������, �������������� �� �����.  */
#define ARDU_D13_PA     5

/** @note ����� SDA ����� I2C1. �� ����� � I2C1 ���������� ������ ����������� � ������������. */
#define ARDU_D14_PB     9
/** @note ����� SCL ����� I2C1. �� ����� � I2C1 ���������� ������ ����������� � ������������. */
#define ARDU_D15_PB     8

/**
    @}
    @}
    @}
    @}
*/

#endif /* BOARD_PINS_H_INCLUDED */

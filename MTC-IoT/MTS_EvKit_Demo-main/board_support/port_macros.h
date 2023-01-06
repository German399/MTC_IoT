#ifndef PORT_MACROS_H_INCLUDED
#define PORT_MACROS_H_INCLUDED

/**
    @ingroup board_support
    @defgroup port_macros ������� ��� ����������������� ��������� � ��������� ��������� ������ STM32
    @{
*/

 /**
    @file port_macros.h
    @author RedBees, LLC
    @copyright This file is distributed under the terms of the MIT license.
 */

///����� ��������� �����
#define MODE_GPIN       0x00
///����� ��������� ������
#define MODE_GPOUT      0x01
///����� �������������� �������
#define MODE_AF         0x02
///����� ����������� �����/������
#define MODE_ANALOG     0x03

///@brief ��������� ������� ����� ��� ������ � ������� MODER
///@param [in] pin_no ����� ������
///@param [in] pin_mode ����� ������, ������������ ���� �� ��������, ������������ �����
#define PIN_MODE(pin_no,pin_mode)       ((pin_mode)<<((pin_no)*2))

///����������� ����� ���� � ������� �����
#define PIN_MASK(pin_no)                (1<<(pin_no))

///@brief ��������� ������� ����� ��� ������ � ������� AFRL
///@param [in] pin_no ����� ������
///@param [in] af_no ����� ��������������� ��� �������������� �������, �������� ������������
#define PIN_AFRL(pin_no,af_no)          ((af_no)<<((pin_no)*4))
///@brief ��������� ������� ����� ��� ������ � ������� AFRH
///@param [in] pin_no ����� ������
///@param [in] af_no ����� ��������������� ��� �������������� �������, �������� ������������
#define PIN_AFRH(pin_no,af_no)          ((af_no)<<(((pin_no) - 8)*4))

/**
    @}
*/

#endif /* PORT_MACROS_H_INCLUDED */

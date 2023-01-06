#ifndef PORT_MACROS_H_INCLUDED
#define PORT_MACROS_H_INCLUDED

/**
    @ingroup board_support
    @defgroup port_macros Макросы для человекочитаемого обращения к регистрам настройки портов STM32
    @{
*/

 /**
    @file port_macros.h
    @author RedBees, LLC
    @copyright This file is distributed under the terms of the MIT license.
 */

///Режим цифрового входа
#define MODE_GPIN       0x00
///Режим цифрового выхода
#define MODE_GPOUT      0x01
///Режим альтернативной функции
#define MODE_AF         0x02
///Режим аналогового входа/выхода
#define MODE_ANALOG     0x03

///@brief Формирует битовую маску для записи в регистр MODER
///@param [in] pin_no Номер вывода
///@param [in] pin_mode Режим вывода, определенный одим из макросов, определяющих режим
#define PIN_MODE(pin_no,pin_mode)       ((pin_mode)<<((pin_no)*2))

///Преобразует номер бита в битовую маску
#define PIN_MASK(pin_no)                (1<<(pin_no))

///@brief Формирует битовую маску для записи в регистр AFRL
///@param [in] pin_no Номер вывода
///@param [in] af_no Номер соответствующей ему альтернативной функции, согласно документации
#define PIN_AFRL(pin_no,af_no)          ((af_no)<<((pin_no)*4))
///@brief Формирует битовую маску для записи в регистр AFRH
///@param [in] pin_no Номер вывода
///@param [in] af_no Номер соответствующей ему альтернативной функции, согласно документации
#define PIN_AFRH(pin_no,af_no)          ((af_no)<<(((pin_no) - 8)*4))

/**
    @}
*/

#endif /* PORT_MACROS_H_INCLUDED */

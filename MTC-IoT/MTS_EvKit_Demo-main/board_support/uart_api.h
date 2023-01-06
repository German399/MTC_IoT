#ifndef UART_API_H_INCLUDED
#define UART_API_H_INCLUDED

/**
    @ingroup board_support
    @defgroup interface_drivers Код поддержки базовых интерфейсов
    @brief Драйвера для периферии контроллера. Реализация драйверов с нуля на уровне регистров позволяет выполнить оптимизации, приводящие к ускорению кода,
    снижению его сложности и размера по сравнению со случаем использования сторонних библиотек. Кроме того, достигается унификация API с вышележащими слоями, что
    положительно сказывается на структуре кода.
    @{
    @defgroup uart_api Код поддержки обмена через USART
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
    @brief Настройка модуля USART для работы с указанной скоростью.

    Настройка выполняется для работы в режиме 8N1.

    @warning Функция не включает тактирование модуля USART и не настраивает порты ввода-вывода. Инициализируется только сам модуль USART.

    @param [in] uart_inst Указатель на модуль USART
    @param [in] baudrate желаемая скорость обмена, бит/с
    @param [in] sys_clk частота шины, от которой тактируется используемый USART
    @param [in] enable_rxne_int разрешить генерацию прерывания по приему байта
    @warning Функция не включает обработку прерывания блоком NVIC, а только разрешает генерацию запроса на прерывание блоком USART.
*/
void init_uart(USART_TypeDef* uart_inst,const uint32_t baudrate,const uint32_t sys_clk,uint8_t enable_rxne_int);

/**
    @brief Посылает байт через указанный USART.
    @param [in] uart_inst Указатель на модуль USART.
    @param [in] b Байт, который необходимо отправить.
*/
void UART_SendByte(USART_TypeDef* uart_inst,uint8_t b);

/**
    @brief Ожидает приема байта через указанный USART.
    @param [in] uart_inst Указатель на модуль USART.
    @return Принятый байт.
*/
uint8_t UART_WaitRxByte(USART_TypeDef* uart_inst);

///Максимальная длина строки, вывод которой в UART поддерживается функцией UART_SendString()
#define UART_MAX_STRING_LENGTH          255
/**
    Вывод в UART строки, заканчивающейся значением NULL (соглашение C)
    @param [in] uart_inst Указатель на модуль USART
    @param [in] str Строка, которую требуется вывести. Длина строки ограничена значением UART_MAX_STRING_LENGTH из соображений отказоустойчивости.
*/
void UART_SendString(USART_TypeDef* uart_inst,const uint8_t *str);

/**
    @}
    @}
*/

#endif /* UART_API_H_INCLUDED */

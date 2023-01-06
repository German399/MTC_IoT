#ifndef BOARD_PINS_H_INCLUDED
#define BOARD_PINS_H_INCLUDED

/**
    @ingroup board_support
    @{
    @defgroup board_pins Сопоставление выводов контроллера их функциям на плате
    @{
    Константы записаны в виде <назначение ножки>_<порт ножки>. Подключение Arduino-совместимых выходов платы к контроллеру
    STM32L152RE совместимо со схемой платы Nucleo, основанной на этом контроллере.

    К I2C1 подключены акселерометр LIS3DH и датчик температуры TMP75, находящиеся на плате; также эта шина выведена на
    Arduino-совместимый разъем.
*/

 /**
    @file board_pins.h
    @author RedBees, LLC
    @copyright This file is distributed under the terms of the MIT license.
 */

#define USER_LED_PA         5
//13 for engineering sample board
#define USER_SW_PC          6

///USART1 используется для реализации вывода printf()
#define USART1_TX_PA        9
#define USART1_RX_PA        10
///USART2 используется для приема данных от модуля GNSS
#define USART2_TX_PA        2
#define USART2_RX_PA        3
///Номер альтернативной функции, соответствующей USART1/2/3, согласно документации
#define USART123_AF_NO        7

///USART3 подключен к модулю NB-IoT
#define USART3_TX_PC        10
#define USART3_RX_PC        11
#define USART3_AF_NO        7

#define I2C1_SCL_PB         8
#define I2C1_SDA_PB         9
///Номер альтернативной функции, соответствующей I2C1, согласно документации
#define I2C1_AF_NO          4

///Вывод порта, включающий питание модуля NB-IoT
#define NB_PWR_ON_PC        8
///Вывод порта, перезагружающий модуль NB-IoT
#define NB_RESET_PC         9
#define NB_RTS_PB           14
#define NB_VINT_PA          15

/**
    @ingroup board_pins
    @{
    @defgroup ardu_pins Определение соответствия выводов контроллера стандартным выводам разъема Arduino UNO v3
	@brief В силу специфического набора периферии, изначально присутствующего на плате, не все выводы Arduino доступны для использования. Детали приведены ниже.
    @{
*/

#define ARDU_A0_PA      0
#define ARDU_A1_PA      1
#define ARDU_A2_PA      4
#define ARDU_A3_PB      0
#define ARDU_A4_PC      1
#define ARDU_A5_PC      0

/** @warning В штатной конфигурации используется для коммуникации с модулем GNSS. */
#define ARDU_D0_PA      3
/** @warning В штатной конфигурации используется для коммуникации с модулем GNSS. */
#define ARDU_D1_PA      2
/** @warning Используется для коммуникации с консолью демонстрационной прошивки. */
#define ARDU_D2_PA      10

#define ARDU_D3_PB      3
#define ARDU_D4_PB      5
#define ARDU_D5_PB      4
#define ARDU_D6_PB      10
#define ARDU_D7_PA      8

/** @warning Используется для коммуникации с консолью демонстрационной прошивки. */
#define ARDU_D8_PA      9

#define ARDU_D9_PC      7
#define ARDU_D10_PB     6
#define ARDU_D11_PA     7
#define ARDU_D12_PA     6

/** @warning Подключен к светодиоду, расположенному на плате.  */
#define ARDU_D13_PA     5

/** @note Линия SDA блока I2C1. На плате к I2C1 подключены датчик температуры и акселерометр. */
#define ARDU_D14_PB     9
/** @note Линия SCL блока I2C1. На плате к I2C1 подключены датчик температуры и акселерометр. */
#define ARDU_D15_PB     8

/**
    @}
    @}
    @}
    @}
*/

#endif /* BOARD_PINS_H_INCLUDED */

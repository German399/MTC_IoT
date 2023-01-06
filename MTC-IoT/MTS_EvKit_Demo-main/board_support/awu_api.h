#ifndef AWU_API_H_INCLUDED
#define AWU_API_H_INCLUDED

/**
    @file awu_api.h
    @author RedBees, LLC
    @copyright This file is distributed under the terms of the MIT license.
*/

/**
    @ingroup interface_drivers
    @defgroup awu_api Код поддержки блока периодического пробуждения и режима пониженного энергопотребления
    @{
*/

#include "stm32l1xx.h"

#define AWU_MAX_WAIT        2000000UL

/**
    @brief Функция выполняет настройку блока периодического пробуждения (AWU) для работы с режимом пониженного энергопотребления Stop.

    @warning Функция не включает тактирование используемой периферии; это требуется сделать отдельно. Для поддержки режимов пониженного энергопотребления
    требуется активность блоков RTC, PWR и EXTI.

    @param [in] wakeup_timeout Максимальное время нахождения в режиме Stop. Максимальное значение - 16000.
    @param [in] timeout_is_in_seconds Множитель времени. Если этот параметр установлен в 1, время указано в секундах, иначе в миллисекундах.
*/
void AWU_Init(uint16_t wakeup_timeout_ms,uint8_t timeout_is_in_seconds);
/**
    @brief Функция останавливает выполнение программы и переводит контроллер в режим Stop. Выход из режима Stop осуществляется по сигналу AWU.
    После этого выполнение программы продолжается начиная с команд, следующих за этой функцией.

    @note Если блок AWU не был предварительно настроен вызовом awu_init(), вызов awu_go_stop() игнорируется.
*/
void AWU_GoStop(void);

/**
    @}
*/

#endif /* AWU_API_H_INCLUDED */

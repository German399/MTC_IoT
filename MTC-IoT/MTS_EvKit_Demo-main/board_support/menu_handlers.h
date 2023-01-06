#ifndef MENU_HANDLERS_H_INCLUDED
#define MENU_HANDLERS_H_INCLUDED

/**
    @ingroup app_code
    @{
    @defgroup service_menu Модуль, реализующий функции сервисного меню
    @{

    @file menu_handlers.h
    @author RedBees, LLC
    @copyright This file is distributed under the terms of the MIT license.
*/

#include "board_support_api.h"

#ifndef NULL
#define NULL                        ((void *)0)
#endif

/**
    Тип функции-обработчика опции сервисного меню. Такая функция принимает в качестве параметра структуру системных настроек и не имеет возвращаемого значения.
*/
typedef void (*menu_handler_t)(device_setup_data_t *settings);

/**
    Структура, описывающая элемент, реализующий опцию сервисного меню. Содержит указатель на функцию, выполняющую необходимое действие,
    а также строковое описание, которое будет выведено пользователю при формировании меню.
*/
typedef struct {
    ///Указатель на функцию, ассоциированную с пунктом меню
    menu_handler_t menu_item_handler;
    ///Словесное описание для вывода в меню
    const uint8_t *item_string_description;
} menu_item_descriptor_t;

/**
    @brief Вспомогательная функция. Выполняет AT-команду

    @param [in] atcmd - строка, содержащая AT-команду. CR-LF добавляется автоматически.
    @return 1 - команда выполнена успешно; 0 - команду не удалось выполнить, либо выполнение команды завершилась с ошибкой
*/
uint8_t execute_AT_command(const uint8_t *atcmd);

/**
    @brief Вспомогательная функция. Выполняет AT-команду без вывода результата в консоль

    @param [in] atcmd - строка, содержащая AT-команду. CR-LF должен быть указан явно.
    @param [out] response_out - буфер, в который будет помещен ответ модуля. Если анализ ответа не требуется, допускается передача NULL в качестве этого параметра.
    @return 1 - команда выполнена успешно; 0 - команду не удалось выполнить, либо выполнение команды завершилась с ошибкой
*/
uint8_t execute_AT_command_silent(const uint8_t *cmd,uint8_t *response_out);
/**
    @brief Вспомогательная функция. Проверяет принадлежность символа к буквам либо цифрам.

    @param [in] ch - символ.
    @return 1 - переданный символ - буква из диапазона [A...Z | a...z] либо цифра.
*/
uint8_t is_alphanumeric(uint8_t ch);

/**
    @brief Массив, сопоставляющий указатели на функции, реализующие опции меню настроек, и их описания

    Функция, реализующая меню, использует информацию из этого массива для автоматического формирования списка
    доступных опций и вызова функций, выбранных пользователем по номеру описания.

    @warning Массив должен заканчиваться элементом {NULL,"\0"}. Такой элемент служит признаком конца массива.
*/
extern const menu_item_descriptor_t menu_items[];

/**
    @brief Функция, выполняющая заводскую настройку платы и радиомодуля.
*/
void perform_initial_setup(device_setup_data_t *settings);
/**
    @}
    @}
*/

uint8_t transmit_telemetry(uint8_t *target_IP,uint8_t *target_url,uint16_t target_port,uint8_t *ICCID_string,uint8_t use_NIDD,uint8_t private_gnss);
uint8_t SARA_init(uint8_t use_NIDD,uint8_t *NIDD_APN);
uint8_t NMEA_to_LatLon(uint8_t *NMEA_string_in,double *dd_lat_out,double *dd_lon_out);

#endif /* MENU_HANDLERS_H_INCLUDED */

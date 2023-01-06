#ifndef COAP_ESSENTIALS_H_INCLUDED
#define COAP_ESSENTIALS_H_INCLUDED

/**
    @file coap_essentials.h
    @author RedBees, LLC
    @copyright This file is distributed under the terms of the MIT license.
*/

/**
    @ingroup fw_code
    @{
    @defgroup coap_code Минимальная реализация основных функций поддержки протокола CoAP
    @brief Несмотря на то, что CoAP позиционируется как "облегченный" протокол для приложений с ограниченными ресурсами, он достаточно сложен, и его
    полноценная реализация была бы избыточна для задач простой телеметрии.
    Данный модуль предоставляет базовые функции для формирования и разбора сообщений CoAP.
    @{
*/

#include "stdint.h"
#include "string.h"

#ifndef NULL
#define NULL    ((void *)0)
#endif

/*
    CoAP-specific constants
*/
/**
    @ingroup coap_code
    @{
    @defgroup support_defs2 Определения констант для индентификаторов класса, версии и типа сообщений
    @{
*/
#define COAP_MSG_VERSION            0x01
#define COAP_TYPE_CONFIRMABLE       0x00
#define COAP_TYPE_NON_CONFIRMABLE   0x01
#define COAP_TYPE_ACK               0x02
#define COAP_TYPE_RESET             0x03
#define COAP_CLASS_REQUEST          0
#define COAP_CLASS_RESP_SUCCESS     2
#define COAP_CLASS_CLIENT_ERROR     4
#define COAP_CLASS_SERVER_ERROR     5
/**
    @}
    @}
*/
/**
    @ingroup coap_code
    @{
    @defgroup coap_methods Определения методов для запросов CoAP
    @{
*/
#define COAP_METHOD_GET             1
#define COAP_METHOD_POST            2
#define COAP_METHOD_PUT             3
#define COAP_METHOD_DELETE          4
/**
    @}
    @}
*/
/**
    @ingroup coap_code
    @{
    @defgroup support_defs Определения, относящеся к полям заголовков и опций CoAP
    @{
*/
#define COAP_OPTION_URI_PATH            11
#define COAP_OPTION_CONTENT_FORMAT      12
//The header is followed by one extra byte of option delta
#define COAP_OPTION_DELTA_EXTRA1B       13
//The header is followed by two extra bytes of option delta
#define COAP_OPTION_DELTA_EXTRA2B       14
//This value can only be encountered in a data marker, 0xFF
#define COAP_OPTION_DELTA_DATA_MKR      15
//The data are preceded by one extra byte of data length
#define COAP_OPTION_LENGTH_EXTRA1B      13
//The data are preceded by two extra bytes of data length
#define COAP_OPTION_LENGTH_EXTRA2B      14
//This value can only be encountered in a data marker, 0xFF
#define COAP_OPTION_LENGTH_DATA_MKR     15
//Converts a normal GET request into an OBSERVE request
#define COAP_OPTION_OBSERVE             6

#define COAP_MSG_HDR_LENGTH             4
/**
    @}
    @}
*/

//The parser will give up searching for options after this limit
///Максимальное количество опций, которое будет анализироваться при разборе сообщения
#define MAX_PARSED_OPTIONS          10
//Maximum number of URI entries allowed for CoAP_assemble_request()
///Максимально допустимое количество сегментов в URI
#define MAX_URI_ENTRIES             10
//Maximum URI string length
///Предельная длина URI
#define MAX_URI_LENGTH              128

typedef struct {

    /** Версия сообщения. Должна совпадать с COAP_MSG_VERSION */
    uint8_t version;
    /** Тип сообщения. */
    uint8_t type;
    /** Длина идентификатора запроса */
    uint8_t token_length;
    /** Класс кода запроса либо ответа */
    uint8_t code_class;
    /** Уточнение кода класса запроса либо ответа */
    uint8_t code_detail;
    /** Идентификатор сообщения */
    uint16_t message_ID;

} CoAP_header_info_t;

typedef struct {

    //The code of a current option
    uint8_t option_id;
    //The length of actual data of the option
    uint8_t data_length;
    //Pointer to a position in a buffer where this option's data start
    uint8_t *data_ptr;

} CoAP_option_info_t;

typedef enum {
    /** Неверное значение версии протокола в заголовке; вероятно, сообщение повреждено */
    ERR_WRONG_PROTOCOL_VER,
    /** Неверное значение типа сообщения в заголовке; вероятно, сообщение повреждено */
    ERR_UNKNOWN_MSG_TYPE,
    /** Недопустимая длина идентификатора запроса */
    ERR_WRONG_TKL,
    /** Ошибка в формате опциии */
    ERR_WRONG_OPTION,
    /** Количество опций в заголовке превышает предел */
    ERR_TOO_MUCH_OPTIONS, /* There's more options in a packet that is allowed by MAX_PARSED_OPTIONS */
    /** Ошибок не произошло */
    ERR_NONE /* No error, everything is OK */
} CoAP_parsing_error_t;

/**
    @brief Выполняет разбор заголовка CoAP-сообщения с извлечением информации в структуру типа CoAP_header_info_t

    @param [in] hdr Массив, в котором содержится CoAP-сообщение
    @param [out] out Указатель на структуру, в которую будет скопирована информация из заголовка
*/
void CoAP_parse_message_header(uint8_t *hdr,CoAP_header_info_t *out);

/**
    @brief Выполняет поиск данных в пакете CoAP

    Заголовок CoAP имеет динамический размер за счет полей опций и динамической длины идентификатора запроса (token). CoAP_parse_message() пропускает
    все служебные поля и возвращает указатель на позицию в пакете, с которой начинаются полезные данные (если они есть).

    Служебная информация CoAP не содержит поля с информацией о длине полезных данных; согласно стандарту, она должна вычисляться как разница между
    длиной заголовка и длиной пакета UDP.

    @param [in] buf Массив, содержащий сообщение CoAP
    @param [in] buf_length Длина входного массива
    @param [out] data_start **Указатель на переменную-указатель**, в которую будет помещена позиция, с которой начинаются данные.

    @warning data_start является указателем на указатель. Это необходимо потому, что функция модифицирует это значение для передачи его в вызывающий код.
    Типичный метод использования выглядит следующим образом:
    @warning @code {.c}
    uint8_t *payload_start;

    ...

    CoAP_parse_message(buffer,buffer_length,&payload_start);
    @endcode

    @return Результат разбора, определенный в CoAP_parsing_error_t
*/
CoAP_parsing_error_t CoAP_parse_message(uint8_t *buf,uint16_t buf_length,uint8_t **data_start);

/**
    @brief Формирует CoAP-сообщение с запросом к серверу

    @param [in] request_method Метод запроса; см. @ref coap_methods
    @param [in] is_observe_request Если этот параметр имеет ненулевое значение, в сообщение будет добавлен заголовок OBSERVE
    @note В текущей ревизии стандарта CoAP OBSERVE имеет смысл только в сочетании с методом GET. Если request_method не равен COAP_METHOD_GET, значение is_observe_request игнорируется.
    @param [in] msg_buffer Буфер, в который будет записано сформированное сообщение
    @param [in] buffer_length Доступная длина буфера
    @param [in] url_string Строка URL, по которому должен быть выполнен запрос
    @warning - Строка URL должна начинаться с '/', например /test/telemetry
    @warning - Строка URL не должна включать IP
    @param [in] data Полезная нагрузка запроса
    @param [in] data_length Длина полезной нагрузки

    @return Фактическая длина сформированного сообщения (<= buffer_length). В случае ошибки - ноль.
*/
uint16_t CoAP_assemble_request_extended(uint8_t request_method,uint8_t is_observe_request,uint8_t *msg_buffer,uint16_t buffer_length,uint8_t *url_string,uint8_t *data,uint16_t data_length);

/**
    @brief Препроцессорная замена для обеспечения совместимости с кодом, использующим старую версию функции, а также для случаев, когда параметр OBSERVE не требуется
    и может быть установлен по-умолчанию.
*/
#define CoAP_assemble_request(request_method,buffer,buffer_length,url_string,data,data_length) CoAP_assemble_request_extended(request_method,0,buffer,buffer_length,url_string,data,data_length)

/**
    @}
    @}
*/

#endif // COAP_ESSENTIALS_H_INCLUDED

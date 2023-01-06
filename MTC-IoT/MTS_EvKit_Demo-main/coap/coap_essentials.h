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
    @defgroup coap_code ����������� ���������� �������� ������� ��������� ��������� CoAP
    @brief �������� �� ��, ��� CoAP ��������������� ��� "�����������" �������� ��� ���������� � ������������� ���������, �� ���������� ������, � ���
    ����������� ���������� ���� �� ��������� ��� ����� ������� ����������.
    ������ ������ ������������� ������� ������� ��� ������������ � ������� ��������� CoAP.
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
    @defgroup support_defs2 ����������� �������� ��� ���������������� ������, ������ � ���� ���������
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
    @defgroup coap_methods ����������� ������� ��� �������� CoAP
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
    @defgroup support_defs �����������, ���������� � ����� ���������� � ����� CoAP
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
///������������ ���������� �����, ������� ����� ��������������� ��� ������� ���������
#define MAX_PARSED_OPTIONS          10
//Maximum number of URI entries allowed for CoAP_assemble_request()
///����������� ���������� ���������� ��������� � URI
#define MAX_URI_ENTRIES             10
//Maximum URI string length
///���������� ����� URI
#define MAX_URI_LENGTH              128

typedef struct {

    /** ������ ���������. ������ ��������� � COAP_MSG_VERSION */
    uint8_t version;
    /** ��� ���������. */
    uint8_t type;
    /** ����� �������������� ������� */
    uint8_t token_length;
    /** ����� ���� ������� ���� ������ */
    uint8_t code_class;
    /** ��������� ���� ������ ������� ���� ������ */
    uint8_t code_detail;
    /** ������������� ��������� */
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
    /** �������� �������� ������ ��������� � ���������; ��������, ��������� ���������� */
    ERR_WRONG_PROTOCOL_VER,
    /** �������� �������� ���� ��������� � ���������; ��������, ��������� ���������� */
    ERR_UNKNOWN_MSG_TYPE,
    /** ������������ ����� �������������� ������� */
    ERR_WRONG_TKL,
    /** ������ � ������� ������ */
    ERR_WRONG_OPTION,
    /** ���������� ����� � ��������� ��������� ������ */
    ERR_TOO_MUCH_OPTIONS, /* There's more options in a packet that is allowed by MAX_PARSED_OPTIONS */
    /** ������ �� ��������� */
    ERR_NONE /* No error, everything is OK */
} CoAP_parsing_error_t;

/**
    @brief ��������� ������ ��������� CoAP-��������� � ����������� ���������� � ��������� ���� CoAP_header_info_t

    @param [in] hdr ������, � ������� ���������� CoAP-���������
    @param [out] out ��������� �� ���������, � ������� ����� ����������� ���������� �� ���������
*/
void CoAP_parse_message_header(uint8_t *hdr,CoAP_header_info_t *out);

/**
    @brief ��������� ����� ������ � ������ CoAP

    ��������� CoAP ����� ������������ ������ �� ���� ����� ����� � ������������ ����� �������������� ������� (token). CoAP_parse_message() ����������
    ��� ��������� ���� � ���������� ��������� �� ������� � ������, � ������� ���������� �������� ������ (���� ��� ����).

    ��������� ���������� CoAP �� �������� ���� � ����������� � ����� �������� ������; �������� ���������, ��� ������ ����������� ��� ������� �����
    ������ ��������� � ������ ������ UDP.

    @param [in] buf ������, ���������� ��������� CoAP
    @param [in] buf_length ����� �������� �������
    @param [out] data_start **��������� �� ����������-���������**, � ������� ����� �������� �������, � ������� ���������� ������.

    @warning data_start �������� ���������� �� ���������. ��� ���������� ������, ��� ������� ������������ ��� �������� ��� �������� ��� � ���������� ���.
    �������� ����� ������������� �������� ��������� �������:
    @warning @code {.c}
    uint8_t *payload_start;

    ...

    CoAP_parse_message(buffer,buffer_length,&payload_start);
    @endcode

    @return ��������� �������, ������������ � CoAP_parsing_error_t
*/
CoAP_parsing_error_t CoAP_parse_message(uint8_t *buf,uint16_t buf_length,uint8_t **data_start);

/**
    @brief ��������� CoAP-��������� � �������� � �������

    @param [in] request_method ����� �������; ��. @ref coap_methods
    @param [in] is_observe_request ���� ���� �������� ����� ��������� ��������, � ��������� ����� �������� ��������� OBSERVE
    @note � ������� ������� ��������� CoAP OBSERVE ����� ����� ������ � ��������� � ������� GET. ���� request_method �� ����� COAP_METHOD_GET, �������� is_observe_request ������������.
    @param [in] msg_buffer �����, � ������� ����� �������� �������������� ���������
    @param [in] buffer_length ��������� ����� ������
    @param [in] url_string ������ URL, �� �������� ������ ���� �������� ������
    @warning - ������ URL ������ ���������� � '/', �������� /test/telemetry
    @warning - ������ URL �� ������ �������� IP
    @param [in] data �������� �������� �������
    @param [in] data_length ����� �������� ��������

    @return ����������� ����� ��������������� ��������� (<= buffer_length). � ������ ������ - ����.
*/
uint16_t CoAP_assemble_request_extended(uint8_t request_method,uint8_t is_observe_request,uint8_t *msg_buffer,uint16_t buffer_length,uint8_t *url_string,uint8_t *data,uint16_t data_length);

/**
    @brief ��������������� ������ ��� ����������� ������������� � �����, ������������ ������ ������ �������, � ����� ��� �������, ����� �������� OBSERVE �� ���������
    � ����� ���� ���������� ��-���������.
*/
#define CoAP_assemble_request(request_method,buffer,buffer_length,url_string,data,data_length) CoAP_assemble_request_extended(request_method,0,buffer,buffer_length,url_string,data,data_length)

/**
    @}
    @}
*/

#endif // COAP_ESSENTIALS_H_INCLUDED

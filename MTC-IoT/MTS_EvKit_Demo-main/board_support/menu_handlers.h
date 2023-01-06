#ifndef MENU_HANDLERS_H_INCLUDED
#define MENU_HANDLERS_H_INCLUDED

/**
    @ingroup app_code
    @{
    @defgroup service_menu ������, ����������� ������� ���������� ����
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
    ��� �������-����������� ����� ���������� ����. ����� ������� ��������� � �������� ��������� ��������� ��������� �������� � �� ����� ������������� ��������.
*/
typedef void (*menu_handler_t)(device_setup_data_t *settings);

/**
    ���������, ����������� �������, ����������� ����� ���������� ����. �������� ��������� �� �������, ����������� ����������� ��������,
    � ����� ��������� ��������, ������� ����� �������� ������������ ��� ������������ ����.
*/
typedef struct {
    ///��������� �� �������, ��������������� � ������� ����
    menu_handler_t menu_item_handler;
    ///��������� �������� ��� ������ � ����
    const uint8_t *item_string_description;
} menu_item_descriptor_t;

/**
    @brief ��������������� �������. ��������� AT-�������

    @param [in] atcmd - ������, ���������� AT-�������. CR-LF ����������� �������������.
    @return 1 - ������� ��������� �������; 0 - ������� �� ������� ���������, ���� ���������� ������� ����������� � �������
*/
uint8_t execute_AT_command(const uint8_t *atcmd);

/**
    @brief ��������������� �������. ��������� AT-������� ��� ������ ���������� � �������

    @param [in] atcmd - ������, ���������� AT-�������. CR-LF ������ ���� ������ ����.
    @param [out] response_out - �����, � ������� ����� ������� ����� ������. ���� ������ ������ �� ���������, ����������� �������� NULL � �������� ����� ���������.
    @return 1 - ������� ��������� �������; 0 - ������� �� ������� ���������, ���� ���������� ������� ����������� � �������
*/
uint8_t execute_AT_command_silent(const uint8_t *cmd,uint8_t *response_out);
/**
    @brief ��������������� �������. ��������� �������������� ������� � ������ ���� ������.

    @param [in] ch - ������.
    @return 1 - ���������� ������ - ����� �� ��������� [A...Z | a...z] ���� �����.
*/
uint8_t is_alphanumeric(uint8_t ch);

/**
    @brief ������, �������������� ��������� �� �������, ����������� ����� ���� ��������, � �� ��������

    �������, ����������� ����, ���������� ���������� �� ����� ������� ��� ��������������� ������������ ������
    ��������� ����� � ������ �������, ��������� ������������� �� ������ ��������.

    @warning ������ ������ ������������� ��������� {NULL,"\0"}. ����� ������� ������ ��������� ����� �������.
*/
extern const menu_item_descriptor_t menu_items[];

/**
    @brief �������, ����������� ��������� ��������� ����� � �����������.
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

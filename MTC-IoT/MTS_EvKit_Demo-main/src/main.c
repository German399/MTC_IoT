/**
    @mainpage ������ 2.6 beta, 31.07.2021

    @tableofcontents

    @section p0 ��������

    �������� ��������������� �������� ����������� ��������� ��� NB-IoT �������� ����������� ��������� ���������� ��������� � ������
    ����������, ���������� ������ ����� ���� LTE Cat. NB1 (NB-IoT), � ����� ������������ ������� ������ � ������������ ���������� ������������.

    �������� ��������� @link p3 ��������� ����@endlink, �������������� ������ � �������� ��������� � �����������, � ����� ��������������� ������
    ������ � �����������. ����� ����� �������� ����� ������������� �������� ����������.

    �������������� � ������������� �������������� � ��������� ������ ����� ���������������� ����, ������ � �������� �������������� � �������
    ��������������� USB-UART, ������������� � USB-������� XS8.

    @note ��������� �����������:
    @note �� ������ 2.1: 38400 ���/�, 8 ��� ������, �������� �������� �� ������������.
    @note ������� � ������ 2.1 beta: 115200 ���/�, 8 ��� ������, �������� �������� �� ������������.

    @warning ��� �������� ������������� ������� �������� ������� local echo � ������������ ���������-��������� ���������.

    � ������������� �� ��� ����� ������������ � ������� "������".

    @section p1 ��������� � �����������

    �������� ��� ����� ��������� ��������� � ����������� ������ ������ ���������� �� ������������, ��������� ������������� �� ����� ���������, ���������
    ��������� CoAP, � ����� ���������� ���� ����������.

    �������� ��� �� ����� ������� ������������ � ������������� ��������� ISO C99.

    @note ������������ �������� ����������� �� ����������� ���������� ����� �� � ������������� �������� #warning � #error, ���������� ������������, ������������
    ������������ GCC.

    @note ���������� �������� ������������� � IDE EmBitz ������ 1.11, ���������� arm-none-eabi-gcc 5.4.1 20160609.
    @warning ��� ���������� ������� ���������� ������ __NO_SYSTEM_INIT � ��������� ������� ��� ���������� ������ ������� ������������� ������� ������������.
    @warning � ���������� ������� ������� ������� ����������� ����� ��������� ����� � ��������� ������� � printf().
    @warning ������������� ���������� ������ ����� �� ����� 1 ��.
	@warning � ���� ����, ��� �� ����� ���������� ���������� ������������ ����� ������������ ���������, �� ��� ������ Arduino-������������ ������� �������� ��� �������������.
	������ ��������� � @link ardu_pins �������� Arduino-����������� ������� ����� @endlink.

    @section p2 ������ ����������

    ���������� ���������� �� ������ � �������, ����������� � JSON. ������ ���������� ����� ���

    {'interface':'telemetry', 'ICCID':'[%s]', 'Tamb_degC':[%d], 'aX':[%d], 'aY':[%d], 'aZ':[%d], 'RSSI_dBm':[%d], 'latitude':[%.6f], 'longitude':[%.6f], 'GNSS_data_valid':[%d]}

    ��� �������� ������ ������������� ����������, ����������������� ����� SIM-����, �������������� �� �����, �������� �����������, ���������� ��������,
    ������������� �� �����, �������� ��������� �� ��� �������������, ������� �������� ������� ������� ����, ���������� �����������, � �����
    ���������� GNSS � �������� �� ������������.

    � ������ �������� ������ ����� IP ������ ��������������� � ��������� CoAP, ������� ����� ���������� �� ������ �� ��������� UDP.

    � ������ �������� ������ ��� ������������� ���������� IP, ������ ���������� � �������� ���� � ������� non-IP �������� ����.

    ����� ������ �������� ������ �������������� � ��������� ����.

    @section p3 ��������� ����

    �������� ��������� ���� ��������, ��������������� ��� ��������� ������� ����������, �����������, � ����� ������� � ��������� ������ ������������.

    @note ������� � ������ 2.6, �������� ��� ���� ����: ������ 1 (������������) � ������ 2, �������������� ����� ������� ���� ��������� "�����"/"����"
    �� ���������� � ������������ ����������� ������������������ ANSI ��� ������ ������ � �������������� �����. ��� ���� ��-��������� ����� ���� ����������
    � ����������.

    @note ������������� ���� ������ 2
    @note ����������� ����� ���� ����� ������� ��������� "�����"/"����" �� ����������. ����������� ������� ������������ ������������ ������.
    @note ������� ������� 'r' ���������� ������ ����������� ����. ��� ����� ���� �������, ���� ����������� ���� �� �����-�� ������� ���� ��������.
    @note ����� ���� ����� ������ �������, ����� ��� ���������� �����, ������������ ����� �� ��� ��������. ���������� ������ ������� ���������� �����;
    �� ����, ��������, ��� ������ ���� ����� 3 ������� ������� ������ ������� '0' � ����� '3'.
    @note ����������� ������� ������� Escape ��������� ���� ������ 2 � �������� � ������ � ������������ ����.

    @note ���� �������� ������������� � ������ ������������� ������� ���������� (��. �������� ����), �� ��� ����� � ��������� ���� ������� ������
    ������ SB1 �, ��������� ��, ������������� ���������� ������� SB2, ����� ���� ���������� SB1 �� ��������� "(i) The device will enter service menu". �����
    � ������� ������� �� ���������� ���� (��. ����) ����� ������� �������� ����� �������� �� ���������.

    @warning ��������� XP10 � XP11 �� ����� ������ ���� ����������� � ��������� �� ���������.

    ���� ��� ������ ����� � ������� EEPROM ����������� �� ���������� ���������� ��������, ���� ��� ���������� ��� ��������� ������ @link ALWAYS_ENTER_SETUP @endlink,
    ���� � ��������� ���� � ����� ������ �������������� �������������.

    @warning ������� �� ����������� ��������� ��������; ��� ����������� ����������� ��������������� ������� ��������� ������� ������� � EEPROM.

    @warning ������� ��� ���������� ���� ����� ������������� ���������� � ����������� �� ������ ��������. ���������� �������� ������� ��������� ����.

    @note ����������� ���� ������ ����� ������������� ������ �������� 2.6 beta �� 31.07.2021
    @note � ������ ����������� ���� ������������ � ���������� ���������-��������� �������� ��������� ������� �������� ���������� ����������.

    1  - ���������� URL, �� �������� ����� ������������ ������ ���������� \n

    @warning URL �� ������ �������� IP-����� � ����

    @warning URL ������ ���������� � ������� '/', �������� /api/v1/<TOKEN>/telemetry

    2  - ���������� IP-����� ������� \n
    3  - ���������� ����� UDP-�����, �� ������� ������ ����� ������� ��������� CoAP \n
    4  - ���������� APN ��� ������� � �������������� NIDD; ��� �� ����� ��������� �������� ���� ��������� ������������� NIDD \n

    5  - ������������ ����������� �������� ������ ���������� � ���������� ����������� (����) \n
    6  - ���� NIDD: �������� �������� ������ ����� ����� NIDD � ������� ��������� ��������� ������� \n
    7  - ���� ������� OBSERVE ��������� CoAP \n
    8  - ����� ������ �������� AT-������ ������ \n

    @warning � ������ ������� ������ AT-��������� ���������� ���������� URC-���������, ��������� � ������� ���������� ������� ����� ������ ������������ � ��
    @warning ����������� ��������� ������ ������ ������� � ������������.

    @note ����� ������� ������ AT-��������� ������������ ��� ����������� ����������� �������, �� ������������ �����������:
    @note exit - ����� �� ������ ������ �������� AT-������
    @note pwrp - ������������ ������� �� ������ PWR_ON ������; ��� ������� ������ ��� ���������� ������ ������������� PSM (��������� ��������� ������).

    9  - ����� ������ �������� ������ ����������� \n

    @note � ������ ������ �������� ������ ����������� �������� ���������� URC-���������.

    @warning ����� �� ������ ������ �������� ������ �������� ������ ����� ���������� ������������ �����.

    @note � ������ ������ �������� ������ ������ UART, ������������ ��� ������ � ������������� � � ������������, ����������� �������� ����������� ����� DMA.
    ����� ������� ��������� ����� ����������� �� ��� ������������ ����������; � ���� ������ ���������������, ������������� �� �����, ���������� ������������
    ������� �������; ������������ �������������� ������� - �������� �������� �� ����� ������ �����. \n
    � ������ �������, ��������� ��������������� �� ������������ �������� ������� ������������ ����������, ����������� ����� �� ������ ������ �������� ������ ����������.

    10  - ������� �������������: ����� ICCID, IMEI, IMSI, ��������� ������ ���������������� � �.�.

    11 - ����������� ���������� � ����: RSSI, SNR, Cell ID, EARFCN \n

    12 - ��������� ��������� �������� ��� ������ ������������� ������� ���������� \n
    13 - ��������� ������ �������� ������ �� �������� �������������� \n

    @note ���� ��� ��������� ������� (�������� �� ���������), �������� ������ � GNSS-������ (��� �� �����������) �� ����� ������������ � ������ ����������.
    ������ ����� � ����� ����� �������� ������������ ������. ��������� �� ������ �� GNSS-����������, ��������� � �������.

    14 - ��������� ������ ���������������� �������� \n

    @note �������� ����� ����������� � ������ ���������� ���� ���� ������������ �������������� ������� ����������. � ������ �������������� ������� ����������
    ���������� �������� �� ������ ������ � ����������, ������������ � �. 11. ���� � ���� �������������� ��������, ��������� ����.

    15 - ������ ��������� ��������, ������������� �� �����, � ��������� GNSS (���� ��������������� ������ ���������� � ��������� � ��������� ����������) \n

    16 - ������������ ����������� \n

    17 - ����������� ������������ ����������� \n

    18 - ����������� ��������, ����������� � EEPROM \n

    19 - ��������� ������ ����, ������������ ��-��������� \n

    20 - ����� ������� �� ������������� ���� ������ 2 \n

    21 - ������ ���� ������ 2 (�������� ������ �� ������������� ����) \n

    22 - ��������� ���� � ���������; ��� ������������� �� ������������ (���������� �� ���� ������ 2). \n

    @section p4 ����������� ������ ������� ����������������

    ������ ������������ ���� LTE Cat NB1 �������� ��������� ������ PSM, �� ���� ���������������� ����������� ��������� ��������� �����������,
    �� ��� ���� �� �������� ����������� � ����; ����� �������, ��� ��������� ��� ��������� ���������� ����� �� �����������.

    � ������ �������, ��������������� STM32L152RE ������������ ����� STOP, ����� ����������� ������� ����������������� �� �������������� ��������.

    @note ��� ������������� ������������ ���������������� ������� ��������������� ������� ���������� ������ @link DO_NOT_GO_STOP @endlink.

    ��������� ����������� ��������� ���������� ��������� ������� ��������� �������; ������ ����������� ������ ����������� ����������� ��������� �����������
    �� ���������������� ������������� ����������������� �������.

    ������ ��������� � ����� PSM ������ ��������������, �� ��������� ����� ������ � ������� ���������, ���� �� ��������� �������, �������������
    ���������� ���� T3324.

    �� ���� ������� � ������, ���� �������� �������� ���������� �� ������ ���������� ����� ����� ������, ������������� ������ PSM �� ����� ������,
    ��������� ������ �� ������ ������� � ����.

    � ������ ���������� ���� ������� ���������������� ����������.
*/

/**
    @file main.c
    @author RedBees, LLC
    @copyright This file is distributed under the terms of the MIT license.

    @defgroup fw_code ��� ���������������� ��������
    @{
    @defgroup app_code ��� ������ ����������������� ����������
    @{
*/

#include "stm32l1xx.h"
#include "board_support_api.h"
#include "menu_engine.h"

/**
    ���������� �����: ��������� ������� ����������� � ����� STOP ����� �������� �������� ����������
    (����������� ����� �� ��������������� ��� ������ � �������������� PSM; ��� ��������� ���� ����� ������� ���������, ��� ������������� PSM ���������).

    ���������, ���� ����������, ����� ������������ ��������� ������������ � ����������� (��� �������� � ����� STOP ����������� ��������).
*/
#define DO_NOT_GO_STOP              0
/**
    ���������� �����: ������ ������� � ��������� ����
*/
#define ALWAYS_ENTER_SETUP          0

/**
    ������������ ������ ������ ������������� �����������. ����� ������ ������������ ����� ������� � ����������� �����.
*/
#define USE_OLD_MODULE_INIT         0

#ifndef __NO_SYSTEM_INIT
#error This firmware must be compiled with __NO_SYSTEM_INIT defined at project level.
#endif

/**
    @brief ������������� �����������

    ������� �������� �������� � ���������������� �����������. ���� � ������� ���� ������ � ������� �������� ��������� ������ �� �������, �� �� ���� ��� ��� �� ���������� ���������
    � ������� ������ ����������� (��������, ���� ���������� ��� ������������ �������� �� �����������), ���� �� ��� ��������������� ������� �������
    (��������� ������� ����������� ��� ������), ���� ����������. ������� ���������, �������� �� ����������� �� �������. ���� ��������, ������ ������ ��������.

    ����� ������������ �������� �������� ������� �����������. ���� ������� �� ������������� ��������� � ����������, ���������� ������������� ������ � �����������
    ��� �������������.

    @param [in] use_NIDD 1 - ������������ NIDD ��� �������� ������; 0 - ������������ IP ��� �������� ������.
    @param [in] NIDD_APN - ������, �������� APN ��� ������ NIDD. � ������ ������������� IP ����� ��������� �������� NULL.
    @return 1 - ������ ��������������� � ����� � ������; 0 - ��������� ������ ��� �������������.
*/
#if USE_OLD_MODULE_INIT == 1
#warning Firmware compiled with the old module initialization method used
uint8_t SARA_init(uint8_t use_NIDD,uint8_t *NIDD_APN)
{
    uint8_t k;
    uint16_t resp_len;
    uint32_t boot_start,boot_end;
    uint8_t module_OK;
    uint8_t buffer[500];
    uint8_t error;
    uint8_t needs_reconfig;

    module_OK=0;

    printf("Starting SARA-R410M...\r\n");
    SARA_R410_PWR_ON_pulse();
    printf("Waiting module to boot...\r\n");

    boot_start = get_uptime_ms();
    /*
        Module should be configured to transmit the greeting text "BOOT OK" as soon as it has booted.
        This is done with AT+CSGT=1,"BOOT OK" command.
    */
    resp_len = AT_ReadReponseBuffer(NULL,0,NULL,NULL,NULL,5*CYCLES_PER_1SEC);
    boot_end = get_uptime_ms();

    if (resp_len==0)
    {
        /* The loop above has finished by timeout, so the module is either already running, or is faulty by some reason. */
        printf("No startup message from the module, checking if it is already running.\r\n");

        k=0;
        while ((resp_len==0) && (k<3))
        {
            printf("Attempt %d...\r\n",k+1);
            AT_SendCommand("AT\r\n");
            resp_len = AT_ReadReponseBuffer(NULL,0,NULL,NULL,NULL,CYCLES_PER_1SEC/2);
            k++;
        }

        if (resp_len==0)
        {
            printf("Module failed to start. Radio communication unavailable.\r\n");
        }
        else
        {
            printf("Module was already running, OK.\r\n");

            module_OK=1;
        }
    }
    else
    {
        printf("Module has booted in %d ms.\r\n",boot_end - boot_start);
        module_OK=1;
    }

    /* Configuring the module for IP or NIDD */
    if (module_OK)
    {
        needs_reconfig=1;

        AT_SendCommand("AT+CGDCONT?\r\n");
        AT_ReadReponseBuffer(buffer,500,NULL,NULL,&error,3*CYCLES_PER_1SEC);

        if (error)
        {
            printf("Unable to analyze and set profile, the RF module does not respond.\r\n");

            return 0;
        }

        if (use_NIDD==1)
        {
            if ((NIDD_APN==NULL) || (NIDD_APN[0]=='\0'))
            {
                printf("NIDD APN value is wrong; the function will not be configured.\r\n");
                needs_reconfig=0;
            }
            else
            {
                if (strstr(buffer,"+CGDCONT: 1,\"NONIP\"")==NULL)
                {
                    printf("Current context is set for IP, but settings call for NIDD, fixing...\r\n");
                    snprintf(buffer,500,"AT+CGDCONT=1,\"NONIP\",\"%s\"\r\n",NIDD_APN);
                }
                else
                {
                    printf("NIDD mode used.\r\n");
                    needs_reconfig=0;
                }
            }
        }
        else
        {
            if (strstr(buffer,"+CGDCONT: 1,\"IPV4V6\"")==NULL)
            {
                printf("Current context is set for NIDD method, but settings call for IP, fixing...\r\n");
                snprintf(buffer,500,"AT+CGDCONT=1,\"IPV4V6\"\r\n");
            }
            else
            {
                printf("IP mode used.\r\n");
                needs_reconfig=0;
            }
        }

        if (needs_reconfig)
        {
            AT_SendCommand(buffer);
            AT_ReadReponseBuffer(buffer,500,NULL,NULL,&error,3*CYCLES_PER_1SEC);

            if (error!=0)
            {
                printf("Unable to configure module.\r\n");

                return 0;
            }

            printf("Applying settings (needs module reboot)...\r\n");
            AT_SendCommand("AT+CFUN=15\r\n");
            AT_ReadReponseBuffer(NULL,0,NULL,NULL,&error,CYCLES_PER_1SEC);

            if (error)
            {
                printf("Unable to reboot module.\r\n");

                return 0;
            }

            /* The module should be configured to output "BOOT OK" after boot, see appropriate AT command. */
            AT_ReadReponseBuffer(NULL,0,NULL,NULL,&error,5*CYCLES_PER_1SEC);
        }
    }

    return 1;
}
#else
uint8_t SARA_init(uint8_t use_NIDD,uint8_t *NIDD_APN)
{
    uint8_t k;
    uint16_t resp_len;
    uint32_t boot_start,boot_end;
    uint32_t time;
    uint8_t module_OK;
    uint8_t buffer[500];
    uint8_t error;
    uint8_t needs_reconfig;

    module_OK = 0;
    resp_len = 0;

    if (GPIOA->IDR & PIN_MASK(NB_VINT_PA))
    {
        printf("V_INT is detected to be inactive, RF module is not switched on.\r\nStarting SARA-R410M...\r\n");
        __AT_FSM_rearm();
        SARA_R410_PWR_ON_pulse();
        printf("Waiting module to boot...\r\n");

        boot_start = get_uptime_ms();
        time = boot_start;

        while ((GPIOA->IDR & PIN_MASK(NB_VINT_PA)) && ((time - boot_start)<5000))
        {
            delay_ms(100);
            time = get_uptime_ms();
        }

        if ((time - boot_start)>7000)
        {
            printf("Unable to switch the RF module. This might be a hardware problem.\r\n");

            return 0;
        }
        else
        {
            printf("V_INT detected after %d ms.\r\n",time - boot_start);
        }

        /*
            Module should be configured to transmit the greeting text "BOOT OK" as soon as it has booted.
            This is done with AT+CSGT=1,"BOOT OK" command.
        */
        resp_len = AT_ReadReponseBuffer(NULL,0,NULL,NULL,NULL,5*CYCLES_PER_1SEC);
        boot_end = get_uptime_ms();

        if (resp_len == 0)
        {
            printf("No startup message from the module.\r\n");
        }

        printf("The module has booted in %d second(s).\r\n",boot_end - boot_start);
    }
    else
    {
        printf("V_INT is present, the RF module is already powered ON.\r\n");
    }

    printf("Ensuring that the module responds to AT commands...\r\n");

    k=0;
    resp_len = 0;
    while ((resp_len==0) && (k<3))
    {
        printf("Attempt %d...\r\n",k+1);
        AT_SendCommand("AT\r\n");
        resp_len = AT_ReadReponseBuffer(NULL,0,NULL,NULL,NULL,CYCLES_PER_1SEC);
        k++;
    }

    if (resp_len==0)
    {
        printf("The module does not respond.\r\n");
    }
    else
    {
        printf("Module OK.\r\n");

        module_OK=1;
    }

    /* Configuring the module for IP or NIDD */
    if (module_OK)
    {
        needs_reconfig=1;

        printf("Reading module configuration...\r\n");

        k=0;
        do
        {
            k++;
            printf("Attempt %d...\r\n",k);
            AT_SendCommand("AT+CGDCONT?\r\n");
            AT_ReadReponseBuffer(buffer,500,NULL,NULL,&error,5*CYCLES_PER_1SEC);

            if (error != 0)
            {
                delay_ms(3000);
            }
        }
        while ((error != 0) && (k<3));

        if (error)
        {
            printf("Unable to analyze and set profile, try this later.\r\n");

            return 0;
        }

        if (use_NIDD==1)
        {
            if ((NIDD_APN==NULL) || (NIDD_APN[0]=='\0'))
            {
                printf("NIDD APN value is wrong; the function will not be configured.\r\n");
                needs_reconfig=0;
            }
            else
            {
                if (strstr(buffer,"+CGDCONT: 1,\"NONIP\"")==NULL)
                {
                    printf("Current context is set for IP, but settings call for NIDD, fixing...\r\n");
                    snprintf(buffer,500,"AT+CGDCONT=1,\"NONIP\",\"%s\"\r\n",NIDD_APN);
                }
                else
                {
                    printf("NIDD mode used.\r\n");
                    needs_reconfig=0;
                }
            }
        }
        else
        {
            if (strstr(buffer,"+CGDCONT: 1,\"IPV4V6\"")==NULL)
            {
                printf("Current context is set for NIDD method, but settings call for IP, fixing...\r\n");
                snprintf(buffer,500,"AT+CGDCONT=1,\"IPV4V6\"\r\n");
            }
            else
            {
                printf("IP mode used.\r\n");
                needs_reconfig=0;
            }
        }

        if (needs_reconfig)
        {
            AT_SendCommand(buffer);
            AT_ReadReponseBuffer(buffer,500,NULL,NULL,&error,3*CYCLES_PER_1SEC);

            if (error!=0)
            {
                printf("Unable to configure module.\r\n");

                return 0;
            }

            printf("Applying settings (needs module reboot)...\r\n");
            AT_SendCommand("AT+CFUN=15\r\n");
            AT_ReadReponseBuffer(NULL,0,NULL,NULL,&error,CYCLES_PER_1SEC);

            if (error)
            {
                printf("Unable to reboot module.\r\n");

                return 0;
            }

            /* The module should be configured to output "BOOT OK" after boot, see appropriate AT command. */
            AT_ReadReponseBuffer(NULL,0,NULL,NULL,&error,5*CYCLES_PER_1SEC);
        }
    }

    return 1;
}
#endif

uint8_t SARA_PowerOff(void)
{
    uint32_t timeout_start;
    uint32_t time;

    AT_SendCommand("AT+CPWROFF\r\n");

    timeout_start = get_uptime_ms();
    time = timeout_start;

    while ((!(GPIOA->IDR & PIN_MASK(NB_VINT_PA))) && ((time - timeout_start)<40000))
    {
        time = get_uptime_ms();
        delay_ms(1000);
    }

    if (GPIOA->IDR & PIN_MASK(NB_VINT_PA))
    {
        /* Module has been switched off successfully. */
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
    @brief ����������� ������ � ������� NMEA (� ������� � �������� � �������� � �������) � ������� � ������� ������.

    @param [in] NMEA_string_in ������ � ������� NMEA (��������� $GNGLL)
    @param [out] dd_lat_out ��������� �� ���������� ���� double, � ������� ����� �������� ������
    @param [out] dd_lon_out ��������� �� ���������� ���� double, � ������� ����� �������� �������

    @return 1 - �������������� ��������� �������; 0 - �������������� ��������� �� �������.
*/
uint8_t NMEA_to_LatLon(uint8_t *NMEA_string_in,double *dd_lat_out,double *dd_lon_out)
{
    uint8_t *NMEA_pointers[10];
    uint8_t ptr_n;
    uint8_t tmp_str[5];
    double lat_deg,lat_min;
    double lon_deg,lon_min;

    NMEA_pointers[0] = strtok(NMEA_string_in,",");

    if (NMEA_pointers[0]==NULL)
    {
        //Some weird string
        return 0;
    }

    if (strcmp(NMEA_pointers[0],"$GNGLL")!=0)
    {
        //Wrong NMEA message
        return 0;
    }

    ptr_n=0;
    while ((NMEA_pointers[ptr_n]!=NULL) && (ptr_n<10))
    {
        ptr_n++;
        NMEA_pointers[ptr_n] = strtok(NULL,",");

        if ((NMEA_pointers[ptr_n]==NULL) && (ptr_n<7))
        {
            //String lacks fields
            return 0;
        }
    }

    if (ptr_n>9)
    {
        //Incorrect string
        return 0;
    }

    if (NMEA_pointers[6][0]!='A')
    {
        //Position data are not fixed
        return 0;
    }

    if (strlen(NMEA_pointers[1])<4)
    {
        return 0;
    }

    if (strlen(NMEA_pointers[3])<4)
    {
        return 0;
    }

    //Thank God, the NMEA format is fixed and leading zeros are always inserted, so it is safe to parse it the simple way.

    //Extracting latitude degrees
    tmp_str[0] = NMEA_pointers[1][0];
    tmp_str[1] = NMEA_pointers[1][1];
    tmp_str[2] = 0;
    lat_deg = atof(tmp_str);
    //Extracting latitude minutes
    lat_min = atof(&(NMEA_pointers[1][2]));

    //Extracting longitude degrees
    tmp_str[0] = NMEA_pointers[3][0];
    tmp_str[1] = NMEA_pointers[3][1];
    tmp_str[2] = NMEA_pointers[3][2];
    tmp_str[3] = 0;
    lon_deg = atof(tmp_str);
    //Extracting longitude minutes
    lon_min = atof(&(NMEA_pointers[3][3]));

    if (dd_lat_out!=NULL)
    {
        if (NMEA_pointers[2][0]=='N')
        {
            (*dd_lat_out) = lat_deg + (lat_min/60.0);
        }
        else
        {
            (*dd_lat_out) = -(lat_deg + (lat_min/60.0));
        }
    }

    if (dd_lon_out!=NULL)
    {
        if (NMEA_pointers[4][0]=='E')
        {
            (*dd_lon_out) = lon_deg + (lon_min/60.0);
        }
        else
        {
            (*dd_lon_out) = -(lon_deg + (lon_min/60.0));
        }
    }

    return 1;
}

#define MAX_JSON_LENGTH     200
#define MAX_COAP_MSG_LEN    (MAX_JSON_LENGTH + MAX_URI_LENGTH + 50)
/**
    @brief �������� ����������

    ���������� ������������ �� ������ � ���� ������ ���������� �������:
    {"interface":"telemetry", "ICCID":"%s", "Tamb_degC":%d, "aX":%d, "aY":%d, "aZ":%d, "RSSI_dBm":%d}

    ���������� ICCID �������������� SIM-����, �����������, ���������� ���������� �������������, �������� ��������� �� ��� �������������, � �����
    ������� �������� ������� ����, �������������� �����������.

    � ������ ������������� ��������� IP, ������ ��������������� � ��������� CoAP � ������������� ������� CoAP POST, ����� ���� ����������
    � ������ UDP �� ��������� ������ � ����.

    � ������ ������������� ������ NIDD ������ ���������� ��� ���� ����� non-IP ��������� ����.

    @param [in] target_IP - ������, ���������� IP-����� �������, �� ������� ����� �������� ���������� (����� ����� ������ ��� ������ IP)
    @param [in] target_url - ������, ���������� URL, �� ������� ����� �������� ���������� (����� ����� ������ ��� ������ IP)
    @param [in] target_port - ����� ����� UDP, �� ������� ����� ������� ����� � �����������
    @param [in] ICCID_string - ������-�������������� SIM-���� ��� ��������� � ����������
    @param [in] use_NIDD - 1 - ������������ ����� NIDD ��� ��������� ������; 0 - ������������ ����� IP ��� ��������� ������
    @return 1 - ���������� ������� ��������; 0 - ��������� ������
*/
uint8_t transmit_telemetry(uint8_t *target_IP,uint8_t *target_url,uint16_t target_port,uint8_t *ICCID_string,uint8_t use_NIDD,uint8_t private_gnss)
{
    int16_t Tamb;
    int16_t accel_X,accel_Y,accel_Z;
    int16_t RSSI;
    static uint8_t json_string[MAX_JSON_LENGTH]; /* Making long strings static to offload the stack */
    volatile uint16_t CoAP_msg_len;
    static uint8_t CoAP_msg_buffer[MAX_COAP_MSG_LEN];
    uint8_t socket_ID;
    uint16_t rx_data_length;
    uint8_t rx_socket_ID;
    volatile uint32_t wait_counter;
    CoAP_header_info_t hdr_info;
    uint32_t resp_wait_start,resp_wait_end;
    volatile uint16_t k;
    uint8_t gnss_result;
    uint8_t gnss_data_conversion_result;
    static uint8_t gnss_string[80] = {"No GNSS data"};
    double latitude;
    double longitude;
    uint8_t GNSS_data_valid;
    static uint8_t NIDD_response_data[520];
    uint16_t NIDD_response_length;

    //Palace square in St. Petersburg by default.
    latitude = 59.9397655;
    longitude = 30.3132998;

    if (use_NIDD!=0)
    {
        printf("Using NIDD method.\r\n");
        execute_AT_command("AT+CRTDCP=1\r\n");
    }
    else
    {
        printf("Using IP method.\r\n");

        if ((target_IP[0]==0) || (target_url[0]==0))
        {
            printf("Unable to transmit telemetry.\r\nPlease check that IP and URL are properly set and try again.\r\n");

            return 0;
        }

        printf("Sending data to %s:%d%s\r\n",target_IP,target_port,target_url);
    }

    GNSS_data_valid=1;
    gnss_result = GNSS_ReadLocationData(gnss_string,80);
    gnss_data_conversion_result=0;

    if (gnss_result==0)
    {
        printf("Unable to read GNSS data, the shield is not fitted or does not function properly.\r\n");
        GNSS_data_valid=0;
    }
    else
    {
        gnss_data_conversion_result = NMEA_to_LatLon(gnss_string,&latitude,&longitude);

        if (gnss_data_conversion_result==0)
        {
            printf("Unable to convert GNSS data to decimal degrees format. GNSS shield is not fitted, GNSS fix is not completed or some other error occurred.\r\n");
            GNSS_data_valid=0;
        }

        if (private_gnss)
        {
            printf("GNSS logging is set to private mode; actual data will not be transmitted to the server.\r\n");
            printf("Actual GNSS location: latitude = %.6f, longitude = %.6f valid = %d\r\n",latitude,longitude,GNSS_data_valid);
            latitude = 59.931839;
            longitude = 30.243405;
            printf("Location data will be replaced to: latitude = %.6f, longitude = %.6f\r\n",latitude,longitude);
        }
    }

#if USE_DS1721 == 1
    Tamb = DS1721_ReadTemperature(LOCAL_TSENSOR_I2C_ADDR);
#else
    Tamb = TMP75_ReadTemperatureCentigrade(LOCAL_TSENSOR_I2C_ADDR);
#endif
    LIS3DH_ReadXYZ(LOCAL_ACCEL_I2C_ADDR,&accel_X,&accel_Y,&accel_Z);
    RSSI = AT_GetRSSI(CYCLES_PER_1SEC);

    snprintf(json_string,MAX_JSON_LENGTH,
    "{\"interface\":\"telemetry\", \"ICCID\":\"%s\", \"Tamb_degC\":%d, \"aX\":%d, \"aY\":%d, \"aZ\":%d, \"RSSI_dBm\":%d, \"latitude\":%.6f, \"longitude\":%.6f, \"GNSS_data_valid\":%d}",
             ICCID_string,
             Tamb,
             accel_X,
             accel_Y,
             accel_Z,
             RSSI,
             latitude,
             longitude,
             GNSS_data_valid);

    printf("Telemetry:\r\n%s\r\n(%d bytes)\r\n",json_string,strlen(json_string));

    if (use_NIDD!=0)
    {
        if (AT_SendStringUsingNIDD(json_string,CYCLES_PER_1SEC)!=0)
        {
            printf("Message successfully sent.\r\n");

            wait_counter = 0;
            NIDD_response_length = 0;
            while ((NIDD_response_length == 0) && (wait_counter<CYCLES_PER_1SEC))
            {
                NIDD_response_length = AT_CheckNIDDReceived(NIDD_response_data);
                wait_counter++;
            }

            if (NIDD_response_length != 0)
            {
                printf("NIDD response received: %s\r\n",NIDD_response_data);
            }
            else
            {
                printf("No NIDD response.\r\n");
            }
        }
        else
        {
            printf("Failed to send message.\r\n");
        }
    }
    else
    {
        CoAP_msg_len = CoAP_assemble_request(COAP_METHOD_POST,CoAP_msg_buffer,MAX_COAP_MSG_LEN,target_url,json_string,strlen(json_string));

        printf("Raw CoAP data (%d bytes):\r\n",CoAP_msg_len);
        for (k=0; k<CoAP_msg_len; k++)
        {
            if (CoAP_msg_buffer[k]<0x10)
            {
                printf("0%X",CoAP_msg_buffer[k]);
            }
            else
            {
                printf("%X",CoAP_msg_buffer[k]);
            }
        }
        printf("\r\n");

        socket_ID = AT_CreateUDPSocket(CYCLES_PER_1SEC);

        if (socket_ID==INVALID_SOCKET_ID)
        {
            printf("Unable to create UDP socket.\r\n");

            return 0;
        }
        else
        {
            printf("Socket ID = %d\r\n",socket_ID);
        }

        if (AT_SendUDPData(socket_ID,target_IP,target_port,CoAP_msg_buffer,CoAP_msg_len,3*CYCLES_PER_1SEC)!=AT_NO_ERROR)
        {
            printf("Failed to send data over IP.\r\n");
        }
        else
        {
            printf("Data sent.\r\n");
        }

        resp_wait_start = get_uptime_ms();
        resp_wait_end = resp_wait_start;
        wait_counter=0;
        rx_socket_ID = INVALID_SOCKET_ID;
        rx_data_length=0;

        printf("Waiting for an answer from the server...\r\n");

        while ((rx_socket_ID != socket_ID) && ((resp_wait_end - resp_wait_start) < 5000UL))
        {
            AT_CheckUDPReceived(&rx_socket_ID,&rx_data_length);
            resp_wait_end = get_uptime_ms();
        }

        printf("RX socket ID = %d, data length = %d\r\n",rx_socket_ID,rx_data_length);

        if ((rx_socket_ID!=socket_ID) || (rx_data_length==0))
        {
            printf("No answer from server.\r\n");
        }
        else
        {
            if (rx_data_length<MAX_COAP_MSG_LEN)
            {
                AT_ReadUDPData(rx_socket_ID,rx_data_length,NULL,NULL,CoAP_msg_buffer,3*CYCLES_PER_1SEC);

                CoAP_parse_message_header(CoAP_msg_buffer,&hdr_info);

                printf("Server response code: %d.%02d, response time = %d ms.\r\n",hdr_info.code_class,hdr_info.code_detail,resp_wait_end - resp_wait_start);

                printf("Server response dump:\r\n");
                for (k=0; k<rx_data_length; k++)
                {
                    if (CoAP_msg_buffer[k]<0x10)
                    {
                        printf("0%X",CoAP_msg_buffer[k]);
                    }
                    else
                    {
                        printf("%X",CoAP_msg_buffer[k]);
                    }
                }
                printf("\r\n");
            }
            else
            {
                printf("Server response is too large (%d bytes).\r\n",rx_data_length);
            }
        }

        if (!AT_CloseUDPSocket(socket_ID,CYCLES_PER_1SEC))
        {
            printf("Unable to close UDP socket (socket_ID = %d).\r\n",socket_ID);
        }
    }

    return 1;
}



/**
    @brief ����� ����� ����������.
*/
void main(void)
{
    device_setup_data_t eep_settings;
    /*
        The board uses soldered-in SIM-chip, so it's obvious that ICCID can not change while the board is powered.
        It is a good idea to read it just once at startup and then use cached value to avoid pointless requests to the RF module.
    */
    uint8_t ICCID_cache_string[100];

    init_board();

    LIS3DH_EnableXYZ(LOCAL_ACCEL_I2C_ADDR,LIS3DH_25Hz,LIS3DH_FULLSCALE_2g,0);
    printf("Accelerometer initialized.\r\n");

#if USE_DS1721 == 1
    DS1721_StartConversion(LOCAL_TSENSOR_I2C_ADDR);
    printf("Temperature sensor initialized.\r\n");
#else
    printf("Temperature sensor always ready.\r\n");
#endif
    printf("Board initialization complete.\r\n");

    recall_device_settings(&eep_settings);

    if ((eep_settings.reserved[0]!=SETTINGS_SIGNATURE_0) || (eep_settings.reserved[1]!=SETTINGS_SIGNATURE_1) || (eep_settings.reserved[2]!=SETTINGS_SIGNATURE_2))
    {
        printf("The board is all-new or the settings in EEPROM are severely damaged or non-existent.\r\nPerforming factory setup.\r\n");
        perform_initial_setup(&eep_settings);
    }

    if ((eep_settings.telemetry_interval_ms<1000) || (eep_settings.telemetry_interval_ms>16000))
    {
        printf("Saved telemetry interval is incorrect. Messages will be sent with 1000 ms timeout.\r\n");
        eep_settings.telemetry_interval_ms = 1000;
    }

    if ((eep_settings.target_URL[0]!='/') || (!(GPIOC->IDR & PIN_MASK(USER_SW_PC))) || (eep_settings.logger_mode_on_startup!=LOGGER_MODE_VALUE) || ALWAYS_ENTER_SETUP)
    {
        printf("(!) The device will enter service menu.\r\n");

        if (SARA_init(eep_settings.use_NIDD,eep_settings.NIDD_APN_name)==0)
        {
            printf("Unable to initialize the RF module. Please check its configuration manually.\r\n");
        }
        else
        {
            printf("Switching off the use of PSM...\r\n");

            if (!AT_SwitchPSM(0,3*CYCLES_PER_1SEC))
            {
                printf("Could not disable the use of PSM;\r\ncheck that the module has started up and not in PSM mode already, and try to do this manually.\r\n");
            }
        }

        if (eep_settings.bit_options & OPTION_START_WITH_MENU_V2)
        {
            service_menu_v2(menu_items);
        }

        while (1)
        {
            service_menu();
        }
    }
    else
    {
        if (SARA_init(eep_settings.use_NIDD,eep_settings.NIDD_APN_name)==0)
        {
            printf("Unable to initialize the RF module, rebooting in menu mode...\r\n");

            eep_settings.logger_mode_on_startup = 0;
            store_device_settings(&eep_settings);

            NVIC_SystemReset();
        }

        while (AT_IsRegistered(CYCLES_PER_1SEC)==0)
        {
            /*
                Notice: this is SUPPOSED TO HANG if the module is unable to register, because if it is not registered in a network,
                continuing has no sense.
            */
            printf("Waiting for the module to register in a network...\r");
            delay_ms(50);
        }

        printf("\r\n");

        AT_ReadICCID(ICCID_cache_string,CYCLES_PER_1SEC);

        #if DO_NOT_GO_STOP == 0
        AWU_Init(eep_settings.telemetry_interval_ms,0);

        if (eep_settings.telemetry_interval_ms>6000)
        {
            printf("Enabling the use of PSM...");
            if (!AT_SwitchPSM(1,3*CYCLES_PER_1SEC))
            {
                printf("Could not enable the use of PSM; something is wrong, the result will be unpredictable.\r\n");
            }
        }
        else
        {
            printf("Telemetry interval is lower than 6 sec, so the module will not be able to go into PSM; the use of PSM will be disabled.\r\n");
            if (!AT_SwitchPSM(0,3*CYCLES_PER_1SEC)==0)
            {
                printf("Could not disable PSM; something is wrong, the result will be unpredictable.\r\n");
            }
        }
        #else
        printf("(i) Notice: this firmware was compiled with power saving features disabled (debug mode).\r\n");
        #endif

        while (1)
        {
            switch_LED(1);
            #if DO_NOT_GO_STOP == 0
            if (eep_settings.telemetry_interval_ms>6000)
            {
                /*
                    The module is most likely in PSM mode now (see docs for more about it), so we need to wake it up.
                */
                SARA_R410_PWR_ON_pulse();
                /*
                    When greeting message is set to "BOOT OK", this call will return on "OK", as designed.
                    If the message is not set, it will return after five seconds or so, which is sufficient for the module to start up.
                    This is effectively a small hack to determine when the module is ready.
                */
                AT_ReadReponseBuffer(NULL,0,NULL,NULL,NULL,5*CYCLES_PER_1SEC);
            }
            #endif
            transmit_telemetry(eep_settings.target_server_IP,eep_settings.target_URL,eep_settings.target_server_port,ICCID_cache_string,eep_settings.use_NIDD,eep_settings.gnss_privacy_mode);
            switch_LED(0);
            #if DO_NOT_GO_STOP == 0
            AWU_GoStop();
            #else
            delay_ms(eep_settings.telemetry_interval_ms);
            #warning Firmware compiled without power-saving features.
            #endif
        }
    }
}

/**
    @}
    @}
*/

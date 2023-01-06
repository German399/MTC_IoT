#include "menu_handlers.h"

/**
    @file menu_handlers.c
    @author RedBees, LLC
    @copyright This file is distributed under the terms of the MIT license.
*/

#define RF_MODULE_BUGFIX            0

#if RF_MODULE_BUGFIX == 1
#warning The firmware compiled with additional code to fix the bug in RF module.
static uint8_t receive_bug_fixed=0; //A flag to determine if the bug with no receive function available until a message is sent was treated
#endif

uint8_t execute_AT_command(const uint8_t *atcmd)
{
    uint8_t data_buffer[255];
    uint16_t resp_length;
    uint8_t error;

    delay_ms(1000);

    snprintf(data_buffer,255,"%s\r\n",atcmd);
    AT_SendCommand(data_buffer);
    resp_length = AT_ReadReponseBuffer(data_buffer,255,NULL,NULL,&error,3*CYCLES_PER_1SEC);

    if (resp_length==0)
    {
        printf("%s - no answer from the module.\r\n",atcmd);
    }
    else
    {
        data_buffer[resp_length] = 0;
        printf("%s\r\n",data_buffer);
    }

    if ((resp_length==0) || (error==1))
    {
        return 0;
    }

    return 1;
}

uint8_t is_alphanumeric(uint8_t ch)
{
    if (((ch>='A') && (ch<='Z')) || ((ch>='a') && (ch<='z')) || ((ch>='0') && (ch<='9')))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
    @ingroup service_menu
    @{
    @defgroup handlers_implementation Реализации функций меню настроек
    @brief Функции не имеют детальных комментариев, поскольку их назначение описано в документации на сервисное меню;
    кроме этого, их описание приводится в строковых элементах массива @link menu_items @endlink
    @{
*/

#define USER_INPUT_LENGTH       80
static uint8_t user_input[USER_INPUT_LENGTH];

void set_target_URL(device_setup_data_t *settings)
{
    printf("Current URL saved in EEPROM: %s\r\n",settings->target_URL);
    printf("New URL (127 characters max!):\r\n");
    scanf("%s",user_input);
    apply_backspace(user_input,USER_INPUT_LENGTH);
    /* Remember, strlen() DOES NOT count NULL character! */
    if (strlen(user_input)>127)
    {
        printf("String too long.\r\n");
    }
    else
    {
        strcpy(settings->target_URL,user_input);
        store_device_settings(settings);
    }
}

void set_server_IP(device_setup_data_t *settings)
{
    uint32_t version;
    uint16_t error_state;

    printf("Current server IP saved in EEPROM: %s\r\n",settings->target_server_IP);

    printf("Enter IP version you want to use, 4 for IPv4 or 6 for IPv6: ");
    scanf("%d",&version);
    printf("\r\n");

    if ((version != 4) && (version != 6))
    {
        printf("Unknown protocol version.\r\n");

        return;
    }

    printf("New server IP (%d characters max!):\r\n",sizeof(settings->target_server_IP));
    scanf("%s",user_input);
    apply_backspace(user_input,USER_INPUT_LENGTH);
    if (strlen(user_input)>sizeof(settings->target_server_IP))
    {
        printf("String too long.\r\n");
    }
    else
    {
        strcpy(settings->target_server_IP,user_input);
        store_device_settings(settings);
    }

    printf("Applying RF module settings for a chosen mode...\r\n");

    error_state = 0;

    if (version == 4)
    {
        error_state += !execute_AT_command("AT+UPSD=0,0,0");
    }
    else
    {
        //Any option except no. 6 was excluded earlier
        error_state += !execute_AT_command("AT+UPSD=0,0,1");
    }

    if (error_state != 0)
    {
        printf("Failed to apply settings for a chosen IP version; the module may not work properly.\r\n");
    }
}

void set_server_port(device_setup_data_t *settings)
{
    printf("Current port saved in EEPROM: %d\r\n",settings->target_server_port);
    printf("New port (0 - 65535):\r\n");
    scanf("%d",&(settings->target_server_port));
    store_device_settings(settings);
}

void set_NIDD_APN(device_setup_data_t *settings)
{
    uint32_t cmp1,cmp2;

    printf("Current APN: %s\r\n",settings->NIDD_APN_name);
    printf("Enter new APN value (63 characters max!),\r\nor enter \"nidd=1\" or \"nidd=0\" to turn NIDD ON/OFF:\r\n");
    scanf("%s",user_input);
    printf("\r\n");
    apply_backspace(user_input,USER_INPUT_LENGTH);
    if (strlen(user_input)>63)
    {
        printf("String too long.\r\n");
    }
    else
    {
        cmp1 = strcmp(user_input,"nidd=1");
        cmp2 = strcmp(user_input,"nidd=0");

        if (cmp1==0)
        {
            if (settings->NIDD_APN_name[0]=='\0')
            {
                printf("NIDD APN not set, so NIDD cannot be switched on!\r\n");
            }
            else
            {
                printf("Saving NIDD mode ON...\r\n");
                settings->use_NIDD=1;
                SARA_init(settings->use_NIDD,settings->NIDD_APN_name);
            }
        }

        if (cmp2==0)
        {
            settings->use_NIDD=0;
            printf("Saving NIDD mode OFF...\r\n");
            SARA_init(settings->use_NIDD,settings->NIDD_APN_name);
        }

        if ((cmp1!=0) && (cmp2!=0))
        {
            strcpy(settings->NIDD_APN_name,user_input);
            printf("Saving new APN...\r\n");
        }

        store_device_settings(settings);
    }
}

void test_transmit_telemetry(device_setup_data_t *settings)
{
    uint8_t temp_ICCID[50];

    AT_ReadICCID(temp_ICCID,CYCLES_PER_1SEC);

    if (settings->gnss_privacy_mode==0)
    {
        printf("Your actual location could be sent to a server. Do you really want to continue (y/n)?\r\n\
You can turn on GNSS privacy mode to avoid unplanned location disclosure.\r\n");
        scanf("%s",user_input);
        apply_backspace(user_input,USER_INPUT_LENGTH);

        if ((user_input[0]=='y') || (user_input[0]=='Y'))
        {
            transmit_telemetry(settings->target_server_IP,
                               settings->target_URL,
                               settings->target_server_port,
                               temp_ICCID,
                               settings->use_NIDD,
                               settings->gnss_privacy_mode);

#if RF_MODULE_BUGFIX == 1
            receive_bug_fixed = 1; //A transmit action fixes the receive bug in NIDD mode
#endif
        }
    }
    else
    {
        transmit_telemetry(settings->target_server_IP,
                           settings->target_URL,
                           settings->target_server_port,
                           temp_ICCID,
                           settings->use_NIDD,
                           settings->gnss_privacy_mode);
#if RF_MODULE_BUGFIX == 1
        receive_bug_fixed = 1;
#endif
    }
}

void true_direct_mode(device_setup_data_t *settings)
{
    printf("Entering true direct mode.\r\n\
From now on everything you type into this terminal will be transferred to the RF module as is \r\n(and similarly in reverse direction).\r\n\
NOTICE: No special commands supported here, for nothing is between you and the RF module.\r\n\
YOU NEED TO REBOOT THE BOARD PHYSICALLY TO EXIT THIS MODE.\r\n---\r\n");

    //Enable DMA1 to transfer data between USART2 and USART3, from console to RF module and vice versa
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    //In this direct mode interrupt is not needed, for all the data transfers are done using DMA
    NVIC_DisableIRQ(USART3_IRQn);

    //Move data from USART3 (RF module data) to USART1 (console) on USART3 RX event
    USART3->CR3 |= USART_CR3_DMAR;
    DMA1_Channel3->CPAR = (uint32_t)(&(USART3->DR));
    DMA1_Channel3->CMAR = (uint32_t)(&(USART1->DR));
    DMA1_Channel3->CNDTR = 1;
    DMA1_Channel3->CCR = DMA_CCR3_MSIZE_0 | DMA_CCR3_PSIZE_0 | DMA_CCR3_CIRC | DMA_CCR3_EN;

    //Move data from USART1 (console) to USART3 (RF module) on USART1 RX event
    USART1->CR3 |= USART_CR3_DMAR;
    DMA1_Channel5->CPAR = (uint32_t)(&(USART1->DR));
    DMA1_Channel5->CMAR = (uint32_t)(&(USART3->DR));
    DMA1_Channel5->CNDTR = 1;
    DMA1_Channel5->CCR = DMA_CCR5_MSIZE_0 | DMA_CCR5_PSIZE_0 | DMA_CCR5_CIRC | DMA_CCR5_EN;

    while (1)
    {
    }
}

void direct_AT_mode(device_setup_data_t *settings)
{
    printf("Entering direct AT-command mode.\r\n\
Everything you type in will be passed to RF module directly, except special commands:\r\n\
\t\"exit\" - exit direct AT command mode\r\n\
\t\"pwrp\" - generate low pulse on PWR_ON pin of a module and wait for startup, useful when using PSM (to wake up the module).\r\n");

    while (1)
    {
        printf("> ");
        scanf("%s",user_input);
        printf("\r\n");
        apply_backspace(user_input,USER_INPUT_LENGTH);

        if (strcmp(user_input,"exit")==0)
        {
            break;
        }
        else
        {
            if (strcmp(user_input,"pwrp")==0)
            {
                printf("Generating pulse on PWR_ON...\r\n");
                SARA_R410_PWR_ON_pulse();
                printf("Waiting module to start...\r\n");
                /*
                    When greeting message is set to "BOOT OK", this call will return on "OK", as designed.
                    If the message is not set, it will return after five seconds or so, which is sufficient for the module to start up.
                    This is effectively a small hack to determine when the module is ready.
                */
                AT_ReadReponseBuffer(NULL,0,NULL,NULL,NULL,5*CYCLES_PER_1SEC);
                printf("Module ready.\r\n");
            }
            else
            {
                execute_AT_command(user_input);
            }
        }
    }
}

void print_identification_data(device_setup_data_t *settings)
{
    uint32_t *MCU_unique_ID_addr;
    uint8_t str[50];

    MCU_unique_ID_addr = MCU_UNIQUE_IDENTIFIER_ADDRESS;

    printf("Board identification data\r\n");
    AT_ReadICCID(str,CYCLES_PER_1SEC);
    printf("\r\nSIM card ICCID: %s\r\n",str);
    execute_AT_command("AT+CCID");
    AT_ReadIMEI(str,CYCLES_PER_1SEC);
    printf("\r\nRF module IMEI: %s\r\n",str);
    execute_AT_command("AT+CGSN");
    printf("\r\nIMSI:\r\n");
    execute_AT_command("AT+CIMI");
    printf("\r\nRF module model:\r\n");
    execute_AT_command("AT+GMM");
    printf("\r\nRF module firmware version:\r\n");
    execute_AT_command("AT+GMR");

    printf("\r\nMCU firmware version:\r\n%s\r\n",FIRMWARE_VERSION);
    printf("\r\nMCU serial number:\r\n0x%X%X%X\r\n",MCU_unique_ID_addr[2],MCU_unique_ID_addr[1],MCU_unique_ID_addr[0]);
}

//As AT_ReadResponseBuffer() supports having NULL for a buffer pointer, this function supports this too
uint8_t execute_AT_command_silent(const uint8_t *cmd,uint8_t *response_out)
{
    uint8_t error;

    AT_SendCommand(cmd);
    AT_ReadReponseBuffer(response_out,512,NULL,NULL,&error,3*CYCLES_PER_1SEC);

    return error;
}

void print_network_info(device_setup_data_t *settings)
{
    static uint8_t resp_buffer[512];
    uint8_t error;
    int32_t RSSI;
    int32_t RSRP;
    int32_t RSRQ;
    int32_t CellID;
    int32_t EARFCN;
    uint16_t k;
    uint16_t marker_pos;

    if (execute_AT_command_silent("AT+CSQ\r\n",resp_buffer)==0)
    {
        printf("\x1B[0m");

        sscanf(resp_buffer,"%*s\r\r\n+CSQ: %d",&RSSI);
        printf("RSSI\t= %d dBm (valid range is -111 to -51 dBm)\r\n",(RSSI*2) - 113);

        marker_pos = 0.42*((RSSI*2.0) - 113.0) + 50;

        /*      __________________________________________________     */
        printf("\r\n");
        printf("  -111                     -51                    0\r\n");
        //printf("   |                         |                    |\r\n");

        for (k=0; k<50; k++)
        {
            if (k==0)
            {
                printf("\x1B[47m");
            }

            if (k==3)
            {
                printf("\x1B[41m");
            }

            if (k==8)
            {
                printf("\x1B[43m");
            }

            if (k==20)
            {
                printf("\x1B[42m");
            }

            if (k==28)
            {
                printf("\x1B[47m");
            }

            if (k == marker_pos)
            {
                printf("\x1B[30m*\x1B[37m");
            }
            else
            {
                printf(" ");
            }
        }
        printf("\x1B[0m dBm\r\n\r\n");
    }
    else
    {
        printf("Unable to execute AT+CSQ\r\n");
    }

    if (execute_AT_command_silent("AT+CESQ\r\n",resp_buffer)==0)
    {
        sscanf(resp_buffer,"%*s\r\r\n+CESQ: %*d,%*d,%*d,%*d,%d,%d",&RSRQ,&RSRP);
        printf("SNR\t= %d dB\r\nRSRP\t= %d dBm (valid range is -141 to -44 dBm)\r\n",RSRQ,RSRP - 141);
    }
    else
    {
        printf("Unable to execute AT+CESQ\r\n");
    }

    AT_SendCommand("AT+CEREG?\r\n");
    AT_ReadReponseBuffer(resp_buffer,512,NULL,NULL,&error,3*CYCLES_PER_1SEC);

    if (execute_AT_command_silent("AT+CEREG=2\r\n",NULL)!=0)
    {
        printf("Unable to set up CEREG request mode.\r\n");
    }
    else
    {
        if (execute_AT_command_silent("AT+CEREG?\r\n",resp_buffer)==0)
        {
            sscanf(resp_buffer,"%*s\r\r\n+CEREG: %*d,%*d,\"%*lx\",\"%lx",&CellID);
            printf("Cell ID\t= %d\r\n",CellID);
        }
        else
        {
            printf("Unable to request Cell ID.\r\n");
        }
    }

    if (execute_AT_command_silent("AT+UCGED=5\r\n",NULL)!=0)
    {
        printf("Unable to set UCGED request mode.\r\n");
    }
    else
    {
        if (execute_AT_command_silent("AT+UCGED?\r\n",resp_buffer)==0)
        {
            sscanf(resp_buffer,"%*s\r\r\n+RSRP: %*d,%d",&EARFCN);
            printf("EARFCN\t= %d\r\n",EARFCN);
        }
        else
        {
            printf("Unable to request EARFCN.\r\n");
        }
    }
}

void wait_NIDD_URC(device_setup_data_t *settings)
{
    uint32_t timeout_ms;
    uint32_t current_time;
    uint32_t start_time;
    uint16_t NIDD_resp_len;
    static uint8_t NIDD_data[512]; //Making large variable static to offload the stack

    if (settings->use_NIDD == 0)
    {
        printf("NIDD method is not enabled. Please first enable NIDD.\r\n");

        return;
    }

#if RF_MODULE_BUGFIX == 1
    if (receive_bug_fixed == 0)
    {
        printf("This is the first time in this session that you use the NIDD receive function;\r\n\
due to bug in the RF module firmware, in NIDD mode it is necessary to first send something to uplink before it will be possible to receive data.\r\n\
For this reason, a telemetry packet will be sent now.\r\n");

        test_transmit_telemetry(settings);

        receive_bug_fixed = 1;
    }
#endif

    printf("Please enter timeout (seconds): ");
    scanf("%d",&timeout_ms);
    printf("\r\n");

    timeout_ms *= 1000; //Convert to milliseconds

    NIDD_resp_len = 0;
    start_time = get_uptime_ms();
    current_time = start_time;

    execute_AT_command("AT+CRTDCP=1\r\n");

    printf("Waiting for message via NIDD...\r\n");

    printf("Timeout: %d sec.     \r",(timeout_ms - (current_time - start_time))/1000);

    while ((NIDD_resp_len == 0) && ((current_time - start_time) < timeout_ms))
    {
        NIDD_resp_len = AT_CheckNIDDReceived(NIDD_data);
        current_time = get_uptime_ms();

        if ((current_time % 1000) == 0)
        {
            printf("Timeout: %d sec.     \r",(timeout_ms - (current_time - start_time))/1000);
        }
    }

    if (NIDD_resp_len == 0)
    {
        printf("No NIDD data received; stopping due to timeout.\r\n");
    }
    else
    {
        printf("NIDD data (%d bytes):\r\n%s\r\n",NIDD_resp_len,NIDD_data);
    }
}

void set_telemetry_interval(device_setup_data_t *settings)
{
    printf("Enter telemetry transmission interval in milliseconds (min=1000, max=16000):\r\n");
    scanf("%i",&(settings->telemetry_interval_ms));

    if (settings->telemetry_interval_ms<1000)
    {
        settings->telemetry_interval_ms = 1000;
        printf("Interval too short, corrected to minimum of 1000 ms.\r\n");
    }

    if (settings->telemetry_interval_ms>16000)
    {
        settings->telemetry_interval_ms = 16000;
        printf("Interval too long, corrected to maximum of 16000 ms.\r\n");
    }

    store_device_settings(settings);
}

void set_GNSS_privacy(device_setup_data_t *settings)
{
    printf("GNSS privacy mode: 1 - enabled (default), 0 - disabled (use with care):\r\n");
    scanf("%s",user_input);
    apply_backspace(user_input,USER_INPUT_LENGTH);

    if (user_input[0]=='0')
    {
        settings->gnss_privacy_mode = 0;
    }
    else
    {
        settings->gnss_privacy_mode = 1;
    }

    store_device_settings(settings);
}

void set_startup_mode(device_setup_data_t *settings)
{
    printf("Firmware startup mode: 0 - this menu (default), 1 - logger mode:\r\n");
    scanf("%s",user_input);
    printf("\r\n");
    apply_backspace(user_input,USER_INPUT_LENGTH);

    if (user_input[0]=='1')
    {
        settings->logger_mode_on_startup = LOGGER_MODE_VALUE;
    }
    else
    {
        settings->logger_mode_on_startup = 0;
    }

    store_device_settings(settings);

    printf("This setting will take effect after MCU restart.\r\n");
}

void test_sensors(device_setup_data_t *settings)
{
    uint8_t error_state;
    int8_t Tamb;
    int16_t accel_X,accel_Y,accel_Z;
    uint8_t accel_signature;
    uint8_t temp_GNSS_string[80];
    double latitude;
    double longitude;

#if USE_DS1721 == 1
    Tamb = DS1721_ReadTemperature(LOCAL_TSENSOR_I2C_ADDR);
#else
    Tamb = TMP75_ReadTemperatureCentigrade(LOCAL_TSENSOR_I2C_ADDR);
#endif
    LIS3DH_ReadXYZ(LOCAL_ACCEL_I2C_ADDR,&accel_X,&accel_Y,&accel_Z);
    accel_signature = LIS3DH_ReadSignature(LOCAL_ACCEL_I2C_ADDR);

    printf("On-board sensors data:\r\n\ttemperature:\t%d\r\n\r\n\taccelerometer:\tX = %d\tY = %d\tZ = %d\r\n",Tamb,accel_X,accel_Y,accel_Z);

    if (accel_signature==0x33)
    {
        printf("\tAccelerometer signature value is correct.\r\n\r\n");
    }
    else
    {
        printf("\tAccelerometer signature value is wrong.\r\n\r\n");
    }

    printf("Testing GNSS option...\r\n");
    //Resetting the FSM to get the freshest data
    GNSS_ReadLocationData(NULL,0);
    //Should be enough for the GNSS module to generate some data, if the shield is present
    delay_ms(1200);
    error_state = GNSS_ReadLocationData(temp_GNSS_string,80);

    if (error_state==0)
    {
        printf("GNSS shield is not fitted or does not function properly.\r\n");
    }
    else
    {
        printf("GNSS string:\r\n%s\r\n",temp_GNSS_string);

        if (!NMEA_to_LatLon(temp_GNSS_string,&latitude,&longitude))
        {
            printf("Unable to convert NMEA to decimal degrees data;\r\nthis is probably because GNSS fix is not achieved yet.\r\n");
        }
        else
        {
            printf("Parsed data:\r\n\tlatitude = %.6f\r\n\tlongitude = %.6f\r\n",latitude,longitude);
        }
    }
}

void perform_system_reset(device_setup_data_t *settings)
{
    NVIC_SystemReset();
}

void perform_RF_module_reset(device_setup_data_t *settings)
{
    execute_AT_command("AT+CFUN=15");
}

void perform_initial_setup(device_setup_data_t *settings)
{
    uint16_t error_state;
    int8_t Tamb;
    uint16_t cmd_resp_length;
    uint8_t k;
    uint8_t res;

    printf("Applying default values to settings (%d bytes)...\r\n",sizeof(device_setup_data_t));
    snprintf(settings->target_server_IP,24,"%s",MTS_COAP_SERVER_IP);
    settings->target_URL[0] = '\0';
    settings->NIDD_APN_name[0] = '\0';
    settings->target_server_port = MTS_COAP_SERVER_PORT;
    settings->use_NIDD = 0;
    settings->gnss_privacy_mode=1;
    settings->logger_mode_on_startup=0;
    settings->telemetry_interval_ms = 1000;
    settings->bit_options = 0;
    settings->reserved[0] = SETTINGS_SIGNATURE_0;
    settings->reserved[1] = SETTINGS_SIGNATURE_1;
    settings->reserved[2] = SETTINGS_SIGNATURE_2;
    printf("Saving data...\r\n");
    store_device_settings(settings);
    printf("EEPROM data structure initialized.\r\n");

    printf("Checking the RF module activity...\r\n");

    k=0;
    do
    {
        printf("Attempt %d...\r\n",k+1);
        AT_SendCommand("AT\r\n");
        cmd_resp_length = AT_ReadReponseBuffer(NULL,0,NULL,NULL,NULL,CYCLES_PER_1SEC);
        if (cmd_resp_length==0)
        {
            printf("No answer.\r\n");
        }
        k++;
    }
    while ((k<3) && (cmd_resp_length==0));

    if (cmd_resp_length==0)
    {
        printf("RF module does not seem to be active, running startup algorithm...");
        SARA_init(0,settings->NIDD_APN_name);
    }

    printf("Configuring the module...\r\n");

    error_state = 0;

    error_state += !execute_AT_command("AT+UMNOPROF=100");
    printf("Rebooting module...\r\n");
    error_state += !execute_AT_command("AT+CFUN=15");
    delay_ms(5000);
    printf("error_state = %d\r\n",error_state);

    /*
        This command is executed, but not included in error checking
    */
    k=0;
    res=0;

    while ((k<5) && (res == 0))
    {
        res = execute_AT_command("AT+UFOTACONF=2,-1");

        if (res == 0)
        {
            delay_ms(2000);
        }

        k++;
    }

    error_state += !execute_AT_command("AT+CSGT=1,\"BOOT OK\"");
    error_state += !execute_AT_command("AT+CFUN=0");
    error_state += !execute_AT_command("AT+CPSMS=0");
    printf("Rebooting module...\r\n");
    error_state += !execute_AT_command("AT+CFUN=15");
    delay_ms(5000);
    printf("error_state = %d\r\n",error_state);

    error_state += !execute_AT_command("AT+CFUN=0");
    error_state += !execute_AT_command("AT+CEDRXS=0,5,\"0010\"");
    error_state += !execute_AT_command("AT+CEDRXS=0,4,\"0010\"");
    printf("Rebooting module...\r\n");
    error_state += !execute_AT_command("AT+CFUN=15");
    delay_ms(5000);
    printf("error_state = %d\r\n",error_state);

    error_state += !execute_AT_command("AT+CGDCONT=1,\"IPV4V6\",\"iot\"");
    printf("Rebooting module...\r\n");
    error_state += !execute_AT_command("AT+CFUN=15");
    delay_ms(5000);
    printf("error_state = %d\r\n",error_state);

    error_state += !execute_AT_command("AT+URAT=8");
    printf("Rebooting module...\r\n");
    error_state += !execute_AT_command("AT+CFUN=15");
    delay_ms(5000);
    printf("error_state = %d\r\n",error_state);

    error_state += !execute_AT_command("AT+UBANDMASK=0,0");
    error_state += !execute_AT_command("AT+UBANDMASK=1,524420");
    printf("Rebooting module...\r\n");
    error_state += !execute_AT_command("AT+CFUN=15");
    delay_ms(5000);
    printf("error_state = %d\r\n",error_state);

    error_state += !execute_AT_command("AT+UGPIOC=16,2");
    printf("error_state = %d\r\n",error_state);

    if (LIS3DH_ReadSignature(LOCAL_ACCEL_I2C_ADDR)!=0x33)
    {
        printf("Accelerometer signature is wrong.\r\n");
        error_state++;
    }
    else
    {
        printf("Accelerometer OK.\r\n");
    }

#if USE_DS1721 == 1
    printf("Firmware compiled with DS1721 support instead of TMP75.\r\n");
    DS1721_StartConversion(LOCAL_TSENSOR_I2C_ADDR);
    printf("DS1721 initialized.\r\n");
    Tamb = DS1721_ReadTemperature(LOCAL_TSENSOR_I2C_ADDR);
#else
    Tamb = TMP75_ReadTemperatureCentigrade(LOCAL_TSENSOR_I2C_ADDR);
#endif

    if ((Tamb<15) || (Tamb>35))
    {
        printf("The temperature is out of the sane range (%d deg. C); the temperature sensor is possibly not functional.\r\n",Tamb);
        error_state++;
    }
    else
    {
        printf("Temperature sensor OK (%d deg. C measured).\r\n",Tamb);
    }

    print_identification_data(settings);

    printf("\r\n--------------------------------------------------------------------------------\r\n");

    if (error_state!=0)
    {
        printf("CONFIG_OK=0\r\n");
    }
    else
    {
        printf("CONFIG_OK=1\r\n");
    }

    printf("\r\n--------------------------------------------------------------------------------\r\n");
}

/**
    @}
    @}
*/

#define MAX_JSON_LENGTH     200
#define MAX_COAP_MSG_LEN    (MAX_JSON_LENGTH + MAX_URI_LENGTH + 50)
uint8_t wait_coap_server_response(uint32_t wait_time)
{
    uint32_t start_time,t;
    uint8_t rx_socket_ID;
    uint16_t rx_data_length;
    CoAP_header_info_t hdr_info;
    uint8_t *coap_data_start_ptr;
    uint8_t CoAP_msg[MAX_COAP_MSG_LEN];

    start_time = get_uptime_ms();
    t=start_time;
    rx_socket_ID = INVALID_SOCKET_ID;

    while ((rx_socket_ID == INVALID_SOCKET_ID) && ((t - start_time)<wait_time))
    {
        AT_CheckUDPReceived(&rx_socket_ID,&rx_data_length);
        t = get_uptime_ms();
        if ((t % 1000) == 0)
        {
            printf("%d     \r",(wait_time/1000) - ((t - start_time)/1000));
        }
    }
    printf("\r\n");

    if (rx_socket_ID == INVALID_SOCKET_ID)
    {
        printf("No response from the server, reception terminated by timeout.\r\n");

        return 0;
    }

    AT_ReadUDPData(rx_socket_ID,rx_data_length,NULL,NULL,CoAP_msg,1*CYCLES_PER_1SEC);

    CoAP_parse_message_header(CoAP_msg,&hdr_info);

    printf("Server responded %d.%02d\r\n",hdr_info.code_class,hdr_info.code_detail);
    CoAP_msg[rx_data_length] = 0;
    CoAP_parse_message(CoAP_msg,rx_data_length,&coap_data_start_ptr);

    if (coap_data_start_ptr != NULL)
    {
        printf("Data: %s\r\n",coap_data_start_ptr);
    }
    else
    {
        printf("No data in response.\r\n");
    }

    return 1;
}

/*
    As of 05.09.2020 MTS server has a bug. According to the standard, a server should respond to the GET request with
    the OBSERVE option set the same way as to an ordinary GET request, and it also must include into the response the
    OBSERVE option with the subscription ID. Currently server does not do this. So it is required to modify behavior of
    the firmware to account for this. When the workaround is used, the firmware will not wait for the initial response
    from a server.
*/
#define USE_SERVER_BUG_WORKAROUND           1

void test_CoAP_observe_function(device_setup_data_t *settings)
{
    static uint8_t res_url[80];
    static uint8_t CoAP_msg[MAX_COAP_MSG_LEN];
    uint16_t CoAP_msg_len;
    uint8_t socket_ID;
    uint32_t wait_time;
    uint16_t k;

    if (settings->use_NIDD != 0)
    {
        printf("The device is in NIDD mode. CoAP observe function cannot be used, because NIDD does not use CoAP.\r\n");

        return;
    }

    printf("Please enter the URL of a resource to be observed:\r\n");
    scanf("%s",res_url);
    printf("\r\n");
    apply_backspace(res_url,80);

    printf("Please enter the maximum time to wait for server responses (seconds):\r\n");
    scanf("%i",&wait_time);
    printf("\r\n");

    wait_time *= 1000; //Convert to milliseconds

    CoAP_msg_len = CoAP_assemble_request_extended(COAP_METHOD_GET,1,CoAP_msg,MAX_COAP_MSG_LEN,res_url,NULL,0);

    if (CoAP_msg_len==0)
    {
        printf("Unable to assemble CoAP request; please check the URL; in particular, it must start with '/'.\r\n");
        return;
    }

    printf("CoAP OBSERVE message:\r\n");
    for (k=0; k<CoAP_msg_len; k++)
    {
        printf("%02X ",CoAP_msg[k]);
    }
    printf("\r\n");

    socket_ID = AT_CreateUDPSocket(2*CYCLES_PER_1SEC);

    if (socket_ID == INVALID_SOCKET_ID)
    {
        printf("Unable to create a UDP socket.\r\n");
        return;
    }

    if (AT_SendUDPData(socket_ID,settings->target_server_IP,settings->target_server_port,CoAP_msg,CoAP_msg_len,3*CYCLES_PER_1SEC) != AT_NO_ERROR)
    {
        printf("Faled to send CoAP message to server.\r\n");
        return;
    }

    printf("CoAP message with an OBSERVE option was sent.\r\n");

#if USE_SERVER_BUG_WORKAROUND != 1
    printf("Waiting for the initial server response...\r\n");

    if (wait_coap_server_response(wait_time) == 0)
    {
        AT_CloseUDPSocket(socket_ID,CYCLES_PER_1SEC);
        return;
    }
#else
#warning The firmware is compiled with the server bug workaround included. Notice that this version will not work correctly with a standard-compliant server.
#endif

    printf("Waiting for the resource change notification...\r\n");

    wait_coap_server_response(wait_time);

    AT_CloseUDPSocket(socket_ID,CYCLES_PER_1SEC);
}

void service_menu_v2(const menu_item_descriptor_t *menu_data);
void test_function(device_setup_data_t *settings)
{
    service_menu_v2(menu_items);
}

void show_EEPROM_settings(device_setup_data_t *settings)
{
    printf("Current settings found in EEPROM:\r\n\r\n");

    if (settings->target_server_IP[0]=='\0')
    {
        printf("Warning: target server IP not set.\r\n");
    }
    else
    {
        printf("Target IP:\t\t%s\r\n",settings->target_server_IP);
    }

    printf("Target port:\t\t%d\r\n",settings->target_server_port);

    if ((!is_alphanumeric(settings->target_URL[1])) || (settings->target_URL[0]=='\0'))
    {
        printf("Warning: target URL not set.\r\n");
    }
    else
    {
        printf("Target URL:\t\t%s\r\n",settings->target_URL);
    }

    if ((!is_alphanumeric(settings->NIDD_APN_name[1])) || (settings->NIDD_APN_name[0]=='\0'))
    {
        printf("Warning: APN name not set.\r\n");
    }
    else
    {
        printf("NIDD APN:\t\t%s\r\n",settings->NIDD_APN_name);
    }

    printf("Use NIDD for telemetry:\t%d\r\n",settings->use_NIDD);

    printf("Board mode on startup:\t");

    if (settings->logger_mode_on_startup==LOGGER_MODE_VALUE)
    {
        printf("logger\r\n");
    }
    else
    {
        printf("service menu\r\n");
    }

    printf("Telemetry interval\r\n(in logger mode):\t%d ms\r\n",settings->telemetry_interval_ms);
    printf("GNSS privacy mode:\t%d\r\n",settings->gnss_privacy_mode);
}

void set_menu_version(device_setup_data_t *settings)
{
    uint32_t desired_version;

    printf("Enter the menu version to be used on device startup\r\n");
    printf("(1 - version 1, \"classic\", 2 - version 2):\r\n");

    scanf("%d",&desired_version);

    printf("\r\n");

    switch (desired_version)
    {
        case 1:
            settings->bit_options &= ~OPTION_START_WITH_MENU_V2;
        break;
        case 2:
            settings->bit_options |= OPTION_START_WITH_MENU_V2;
        break;
        default:
            printf("Warning: menu version %d is not supported. The startup mode will be left unchanged.\r\n");
        break;
    }

    store_device_settings(settings);

    printf("Setting saved.\r\n");
}

void show_menu_v2_help(device_setup_data_t *settings)
{
    printf("Menu version 2 usage and hotkeys\r\n\r\n");
    printf("Press arrows Up/Down to move selection, Enter to run the selected function.\r\n\
The items which are unavailable are drawn with a dim font.\r\n\r\n\
Press 'r' to force menu redraw (may be needed if menu gets broken for some reason).\r\n\r\n\
You can type in a two-digit number of the menu item (which is displayed on the left)\r\n\
to quickly select the desired item without the need to press up/down keys many times.\r\n\
Notice that you must enter two digits, e.g. for the item no. 3 you must enter '0' and then '3'.\r\n\r\n\
Press Escape three times to exit menu version 2 and return to classic menu.\r\n");
}

const menu_item_descriptor_t menu_items[] = {
    {set_target_URL,"set the URL of the resource JSON data will be transmitted to"},
    {set_server_IP,"set the IP address"},
    {set_server_port,"set the port"},
    {set_NIDD_APN,"set an APN for NIDD access or turn NIDD mode ON or OFF"},
    {test_transmit_telemetry,"force send telemetry packet"},
    {wait_NIDD_URC,"wait for incoming NIDD data during specified timeout (and then exit)"},
    {test_CoAP_observe_function,"test CoAP OBSERVE function"},
    {direct_AT_mode,"enter direct AT-command mode"},
    {true_direct_mode,"enter true direct mode to access the RF module"},
    {print_identification_data,"show board identification data (serial numbers, firmware versions, etc.)"},
    {print_network_info,"show network information"},
    {set_telemetry_interval,"set telemetry transmission interval"},
    {set_GNSS_privacy,"set GNSS privacy mode (hide actual location data when transmitting on server)"},
    {set_startup_mode,"set firmware startup mode (setup or logger)"},
    {test_sensors,"read on-board sensors and try to acquire GNSS data"},
    {perform_system_reset,"reboot MCU"},
    {perform_RF_module_reset,"reboot RF module"},
    {show_EEPROM_settings,"show EEPROM settings"},
    {set_menu_version,"set menu version to use after startup"},
    {show_menu_v2_help,"show usage and hotkeys for menu version 2"},
    {test_function,"launch menu version 2"},
    {perform_initial_setup,"factory setup & test (do not use this unless you really know what you want)"},
    {NULL,"\0"}
};

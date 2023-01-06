#include "at_cmd_support.h"

 /**
    \file at_cmd_support.c
    \author RedBees, LLC
    \copyright This file is distributed under the terms of the MIT license.
 */

static volatile uint8_t response_buffer[AT_RESP_BUFFER_LENGTH];
static volatile uint16_t buffer_index=0;
static volatile uint8_t resp_buffer_overflow=0;
static volatile uint8_t resp_buffer_overrun=0;
static volatile uint8_t lock_buffer=0;
#define NO_RESP_RECEIVED            0
#define RECEIVED_OK                 1
#define RECEIVED_ERROR              2
static volatile uint8_t resp_received=NO_RESP_RECEIVED;
//This is the sequences which terminate the response. By observing it we can determine when the complete response has arrived.
#define OK_LEN          2
#define ERR_LEN         5
static const uint8_t resp_end_OK[] = "OK";
static const uint8_t resp_end_ERR[] = "ERROR";
static volatile uint8_t OK_index=0;
static volatile uint8_t ERR_index=0;

#if USE_ADVANCED_URC_SEARCH == 1
#define UDP_URC_LIST_INDEX          0
#define NIDD_URC_LIST_INDEX         1
volatile expected_URC_entry_t URC_search_list[NUM_URC_SUPPORTED] = {

{"+UUSORF: ",0,0,0}, /* UDP data received */
{"+CRTDCP:",0,0,0} /* Control plane (NIDD) data received */

};
#else
#define URC_UDPRCV_LEN  9
static const uint8_t resp_urc_udp_rcv[] = "+UUSORF: ";
static volatile uint8_t URC_index=0;
static volatile uint8_t URC_UDP_received=0;
#endif

void USART3_IRQHandler(void)
{
    volatile uint8_t tmp;
    //static uint8_t urc_header_found=0;
    volatile uint32_t SR_content;
#if USE_ADVANCED_URC_SEARCH == 1
    uint8_t k;
#endif

    SR_content = USART3->SR;
    tmp = USART3->DR;
    USART3->SR = 0;

    if (!lock_buffer)
    {
        if (SR_content & USART_SR_ORE)
        {
            //Reset the search because of an overrun error
            buffer_index=0;
            OK_index=0;
            ERR_index=0;
#if USE_ADVANCED_URC_SEARCH == 1
            for (k=0; k<NUM_URC_SUPPORTED; k++)
            {
                URC_search_list[k].match_counter = 0;
            }
#else
            URC_index=0;
#endif
        }

        if (tmp!=0)
        {
            response_buffer[buffer_index] = tmp;

#ifndef NO_URC_SEARCH
 #if USE_ADVANCED_URC_SEARCH == 1
            //Perform symbol-by-symbol comparison for each URC we want to support
            for (k=0; k<NUM_URC_SUPPORTED; k++)
            {
                if (URC_search_list[k].URC_detected == 0)
                {
                    //If the header of this URC was already detected, there is no need to continue its search
                    if (URC_search_list[k].URC_header_detected == 0)
                    {
                        //Compare symbol by symbol
                        if (response_buffer[buffer_index] == URC_search_list[k].URC_string_name[URC_search_list[k].match_counter])
                        {
                            URC_search_list[k].match_counter++;

                            //If the next symbol has a zero value, it means that we are at the end of the name string, i.e. URC header is detected
                            if (URC_search_list[k].URC_string_name[URC_search_list[k].match_counter] == 0)
                            {
                                URC_search_list[k].match_counter = 0;
                                URC_search_list[k].URC_header_detected = 1;
                            }
                        }
                        else
                        {
                            //Reset search at any non-matching character
                            URC_search_list[k].match_counter = 0;
                        }
                    }
                    else
                    {
                        //If URC header was found, then we just wait for CR-LF sequence, which completes the URC message
                        if ((response_buffer[buffer_index] == 0x0A) && (response_buffer[buffer_index - 1] == 0x0D))
                        {
                            lock_buffer = 1;
                            URC_search_list[k].URC_detected = 1;
                            URC_search_list[k].URC_header_detected = 0;
                            URC_search_list[k].match_counter = 0;
                        }
                    }
                }
            }
 #else
 #warning Firmware compiled with legacy URC search engine. NIDD will not work properly.
            if (response_buffer[buffer_index]==resp_urc_udp_rcv[URC_index])
            {
                URC_index++;

                if (URC_index>=URC_UDPRCV_LEN)
                {
                    URC_index=0;
                    urc_header_found=1;
                }
            }
            else
            {
                URC_index=0;
            }

            if ((urc_header_found) && (response_buffer[buffer_index]==0x0A) && (response_buffer[buffer_index-1]==0x0D))
            {
                /* URC ends with CR-LF */
                lock_buffer=1;
                URC_UDP_received=1;
                urc_header_found=0;
            }
 #endif
#else
#warning URC support turned off. UDP and NIDD will not work properly.
#endif

#ifndef DEBUG_NO_RESP_END_SEARCH
            if (response_buffer[buffer_index]==resp_end_OK[OK_index])
            {
                OK_index++;

                if (OK_index>=OK_LEN)
                {
                    lock_buffer=1;
                    OK_index=0;
                    resp_received = RECEIVED_OK;
                }
            }
            else
            {
                OK_index=0;
            }

            if (response_buffer[buffer_index]==resp_end_ERR[ERR_index])
            {
                ERR_index++;

                if (ERR_index>=ERR_LEN)
                {
                    lock_buffer=1;
                    ERR_index=0;
                    resp_received = RECEIVED_ERROR;
                }
            }
            else
            {
                ERR_index=0;
            }
#else
            #warning Response end detection is deactivated; AT-command engine is in debug mode.
            resp_received=1;
#endif

            buffer_index++;
        }
    }
    else
    {
        tmp = USART3->DR; //Just to clear interrupt/error flags

        //Ignore delimiters
        if ((tmp!='\r') && (tmp!='\n') && (tmp!=' '))
        {
            resp_buffer_overrun=1;
        }
    }

    if (buffer_index>=AT_RESP_BUFFER_LENGTH)
    {
        buffer_index=AT_RESP_BUFFER_LENGTH-1;
        resp_buffer_overflow=1;
    }
}

/* Wait some time for a response. If timeout has been reached, but no answer was received, the function returns the current state value, i.e. NO_RESP_RECEIVED */
uint8_t __wait_AT_response(uint32_t timeout)
{
    volatile uint32_t safety_counter;

    safety_counter = 0;
    while ((resp_received==NO_RESP_RECEIVED) && (safety_counter<timeout))
    {
        safety_counter++;
    }

    return resp_received;
}

/* Resets the state of a state machine controlling AT response reception, gets ready for the new data. */
void __AT_FSM_rearm(void)
{
#if USE_ADVANCED_URC_SEARCH == 1
    uint8_t k;
#endif

    resp_received = NO_RESP_RECEIVED;
    lock_buffer = 0;
    resp_buffer_overflow = 0;
    resp_buffer_overrun = 0;
    buffer_index = 0;
#if USE_ADVANCED_URC_SEARCH == 1
    for (k=0; k<NUM_URC_SUPPORTED; k++)
    {
        URC_search_list[k].match_counter = 0;
        URC_search_list[k].URC_detected = 0;
        URC_search_list[k].URC_header_detected = 0;
    }
#else
    URC_UDP_received = 0;
#endif
}

uint16_t AT_ReadReponseBuffer(uint8_t* buffer_out,uint16_t buffer_length,uint8_t* is_overflowed,uint8_t* overrun_detected,uint8_t *error_status,uint32_t max_wait_time)
{
    uint16_t k;
    uint16_t data_length;
    uint8_t wait_result;

#ifndef NO_RESP_END_SEARCH
    wait_result = __wait_AT_response(max_wait_time);

    if (wait_result==NO_RESP_RECEIVED)
    {
        /* Timeout has elapsed, but still no response received */
        if (is_overflowed!=NULL)
        {
            (*is_overflowed) = 0;
        }

        if (overrun_detected!=NULL)
        {
            (*overrun_detected) = 0;
        }

        buffer_out[0]=0;

        __AT_FSM_rearm();

        return 0;
    }
#endif

    lock_buffer=1; //Disable writing to buffer to safely copy its contents
    data_length = buffer_index;

    if (buffer_out!=NULL)
    {
        for (k=0; k<data_length; k++)
        {
            if (k==(buffer_length-1))
            {
                break;
            }

            buffer_out[k] = response_buffer[k];
        }

        buffer_out[k] = 0;
    }

    if (is_overflowed!=NULL)
    {
        (*is_overflowed) = resp_buffer_overflow;
    }

    if (overrun_detected!=NULL)
    {
        (*overrun_detected) = resp_buffer_overrun;
    }

    if (error_status!=NULL)
    {
        if (wait_result==RECEIVED_ERROR)
        {
            (*error_status) = 1;
        }
        else
        {
            (*error_status) = 0;
        }
    }

    __AT_FSM_rearm();

    return data_length;
}

void AT_SendCommand(const uint8_t *cmd)
{
    __AT_FSM_rearm();
    UART_SendString(USART3,cmd);
}

uint8_t AT_CreateUDPSocket(uint32_t max_wait_time)
{
    uint8_t resp;
    uint8_t *data_start;
    uint8_t socket_id;

    AT_SendCommand("AT+USOCR=17\r\n");

    resp = __wait_AT_response(max_wait_time);

    if ((resp==NO_RESP_RECEIVED) || (resp==RECEIVED_ERROR))
    {
        if (resp==RECEIVED_ERROR)
        {
            __AT_FSM_rearm();
        }

        return 255;
    }

    lock_buffer = 1;
    response_buffer[buffer_index]=0;

    data_start = strstr((char *)response_buffer,"+USOCR: ");

    if (data_start==NULL)
    {
        return 255;
    }

    /*
        Move to the position after "+USOCR: ", read the ASCII representation of the socket ID and convert it to integer in-place.
        SARA-R410 only supports <10 sockets, so the value will always consist of one digit - no problem with this.
    */
    socket_id = data_start[8] - '0';

    __AT_FSM_rearm();

    return socket_id;
}

uint8_t AT_CloseUDPSocket(uint8_t socket_id,uint32_t max_wait_time)
{
    uint8_t cmd[20];
    uint8_t result;

    snprintf(cmd,20,"AT+USOCL=%d\r\n",socket_id);

    AT_SendCommand(cmd);
    result = __wait_AT_response(max_wait_time);

    if ((result == NO_RESP_RECEIVED) || (result == RECEIVED_ERROR))
    {
        if (result==RECEIVED_ERROR)
        {
            __AT_FSM_rearm();
        }

        return 0;
    }

    __AT_FSM_rearm();

    return 1;
}

extern volatile uint32_t CYCLES_PER_1SEC;

at_udp_error_t AT_SendUDPData(uint8_t socket_id,uint8_t *target_IP_string,uint16_t target_port,uint8_t *data,uint16_t data_length,uint32_t max_wait_time)
{
    uint8_t cmd_header[30];
    const uint8_t conv_LUT[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    uint32_t k;
    uint8_t resp;

    UART_SendString(USART3,"AT+UDCONF=1,1\r\n");

    resp = __wait_AT_response(max_wait_time);

    if (resp==NO_RESP_RECEIVED)
    {
        return AT_NO_RESPONSE;
    }

    __AT_FSM_rearm();

    if (resp==RECEIVED_ERROR)
    {
        return AT_ERROR;
    }

    sprintf(cmd_header,"AT+USOST=%d,\"%s\",%d,%d,\"",socket_id,target_IP_string,target_port,data_length);
    UART_SendString(USART3,cmd_header);

    for (k=0; k<data_length; k++)
    {
        UART_SendByte(USART3,conv_LUT[data[k] >> 4]);
        UART_SendByte(USART3,conv_LUT[data[k] & 0x0F]);
    }

    UART_SendString(USART3,"\"\r\n");

    resp = __wait_AT_response(max_wait_time);

    if (resp==NO_RESP_RECEIVED)
    {
        return AT_NO_RESPONSE;
    }

    __AT_FSM_rearm();

    if (resp==RECEIVED_ERROR)
    {
        return AT_ERROR;
    }

    return AT_NO_ERROR;
}

uint8_t AT_CheckUDPReceived(uint8_t *socket_id,uint16_t *packet_length)
{
    uint8_t *data_start;
    uint8_t k;

#if USE_ADVANCED_URC_SEARCH == 1
    if (URC_search_list[UDP_URC_LIST_INDEX].URC_detected == 0)
#else
    if (URC_UDP_received==0)
#endif
    {
        if (socket_id!=NULL)
        {
            (*socket_id) = 255;
        }

        if (packet_length!=NULL)
        {
            (*packet_length) = 0;
        }

        return 0;
    }

    response_buffer[buffer_index] = 0;

    data_start = strstr((char *)response_buffer,"+UUSORF: ");

    if (socket_id!=NULL)
    {
        /* The socket ID cannot be greater than 9 (in fact SARA-R410 supports inly 7 sockets), so we can go the simple way. */
        (*socket_id) = data_start[9] - '0';
    }

    k=0;
    if (packet_length!=NULL)
    {
        /* But for the packet length we'll go the long way. */
        (*packet_length) = 0;
        while ((data_start[11+k]>='0') && (data_start[11+k]<='9') && (k<5))
        {
            (*packet_length) *= 10;
            (*packet_length) += data_start[11 + k] - '0';
            k++;
        }
    }

    __AT_FSM_rearm();

    if (k>=5)
    {
        /* Something went wrong. */
        return 0;
    }

    return 1;
}

uint16_t AT_CheckNIDDReceived(uint8_t *data_out)
{
#if USE_ADVANCED_URC_SEARCH == 0
#warning AT_CheckNIDDReceived() is unavailable because advanced URC search is not included. Placeholder code is compiled instead of AT_CheckNIDDReceived().
    return 0;
#else
    uint32_t URC_data_length;
    uint8_t URC_data_buffer[520];
    uint16_t k;

    if (URC_search_list[NIDD_URC_LIST_INDEX].URC_detected == 0)
    {
        return 0;
    }

    response_buffer[buffer_index] = 0;

    sscanf((char *)response_buffer,"\r\n\r\n+CRTDCP:%*d,%d,\"%[^\r\n\"]",&URC_data_length,URC_data_buffer);

    if (data_out != NULL)
    {
        k=0;
        while ((k<URC_data_length) && (k<512))
        {
            data_out[k] = URC_data_buffer[k];
            k++;
        }

        data_out[k] = 0;
    }

    __AT_FSM_rearm();

    return URC_data_length;
#endif
}

inline uint8_t h_digit_to_nibble(uint8_t h_digit)
{
    if ((h_digit>='0') && (h_digit<='9'))
    {
        return h_digit - '0';
    }

    if ((h_digit>='A') && (h_digit<='F'))
    {
        return (h_digit - 'A') + 10;
    }

    if ((h_digit>='a') && (h_digit<='f'))
    {
        return (h_digit - 'a') + 10;
    }

    return 0;
}

at_udp_error_t AT_ReadUDPData(uint8_t socket_id,uint16_t read_length,uint8_t *source_IP,uint16_t *source_port,uint8_t *data_out,uint32_t max_wait_time)
{
    uint8_t cmd[20];
    uint8_t resp;
    uint8_t *response_pointer;
    uint8_t ip_string[20];
    uint16_t port;
    uint16_t data_bytes_count;
    uint8_t data_string[128];
    uint8_t k;
    uint16_t data_ptr;

    sprintf(cmd,"AT+USORF=%d,%d\r\n",socket_id,read_length);
    AT_SendCommand(cmd);

    resp = __wait_AT_response(max_wait_time);

    if (resp == NO_RESP_RECEIVED)
    {
        return AT_NO_RESPONSE;
    }

    if (resp == RECEIVED_ERROR)
    {
        __AT_FSM_rearm();

        return AT_ERROR;
    }

    response_buffer[buffer_index] = 0;

    response_pointer = strstr((char *)response_buffer,"+USORF: ");

    if (sscanf(response_pointer,"+USORF: %*d,\"%[^\"]\",%d,%d,\"%[^\"]",ip_string,&port,&data_bytes_count,data_string) != 4)
    {
        __AT_FSM_rearm();

        return AT_FORMAT_ERROR;
    }

    if (data_out!=NULL)
    {
        data_ptr=0;
        k=0;

        while (k<(data_bytes_count*2))
        {
            data_out[data_ptr] = (h_digit_to_nibble(data_string[k]) << 4) | h_digit_to_nibble(data_string[k+1]);
            data_ptr++;
            k+=2;
        }
    }

    if (source_IP!=NULL)
    {
        strcpy(source_IP,ip_string);
    }

    if (source_port!=NULL)
    {
        (*source_port) = port;
    }

    __AT_FSM_rearm();

    return AT_NO_ERROR;
}

int16_t AT_GetRSSI(uint32_t timeout)
{
    uint8_t *result_pos;
    uint8_t k;
    int16_t rssi_num;
    int16_t rssi_actual;
    uint8_t buf_copy[100];
    uint16_t buf_copy_length;
    uint8_t error;

    AT_SendCommand("AT+CSQ\r\n");

    buf_copy_length = AT_ReadReponseBuffer(buf_copy,100,NULL,NULL,&error,timeout);

    if ((error!=0) || (buf_copy_length==0))
    {
        return 0;
    }

    result_pos = strstr((char *)buf_copy,"+CSQ: ");

    if (result_pos==NULL)
    {
        return -1002;
    }

    /*
        There can be no more than 5 valuable characters, because both resulting values do not exceed 99;
        so the longest string looks like "+CSQ: 99,99".
        By setting k=6 we skip "+CSQ: " part.
    */
    k=6;
    rssi_num = 0;
    while ((result_pos[k]!=',') && (result_pos[k]!=0) && (k<8))
    {
        rssi_num *=10;
        rssi_num += (result_pos[k] - '0');
        k++;
    }

    if ((rssi_num>=2) && (rssi_num<=30))
    {
        rssi_actual = 2*rssi_num - 113;
    }
    else
    {
        if (rssi_num==1)
        {
            rssi_actual = -111;
        }

        if (rssi_num==31)
        {
            rssi_actual = -51;
        }
    }

    return rssi_actual;
}

uint8_t AT_ReadICCID(uint8_t *ICCID,uint32_t timeout)
{
    uint8_t *id_pos;
    uint8_t k;
    uint8_t buf_copy[100];
    uint16_t buf_copy_length;
    uint8_t error;

    AT_SendCommand("AT+CCID\r\n");

    buf_copy_length = AT_ReadReponseBuffer(buf_copy,100,NULL,NULL,&error,timeout);

    if ((error!=0) || (buf_copy_length==0))
    {
        if (ICCID!=NULL)
        {
            ICCID[0]=0;
        }

        return 0;
    }

    id_pos = strstr((char *)buf_copy,"+CCID: ");

    if (id_pos==NULL)
    {
        return 0;
    }

    if (ICCID!=NULL)
    {
        k=0;
        while ((id_pos[7+k]!='\r') && (k<49))
        {
            ICCID[k] = id_pos[7+k];
            k++;
        }
        ICCID[k]=0;
    }

    return 1;
}

uint8_t AT_ReadIMEI(uint8_t *IMEI,uint32_t timeout)
{
    uint8_t *resp_start;
    uint8_t buf_copy[100];
    uint16_t k;
    uint16_t buf_copy_length;
    uint8_t error;

    AT_SendCommand("AT+CGSN\r\n");
    buf_copy_length = AT_ReadReponseBuffer(buf_copy,100,NULL,NULL,&error,timeout);

    if ((error!=0) || (buf_copy_length==0))
    {
        if (IMEI!=NULL)
        {
            IMEI[0]=0;
        }

        return 0;
    }

    resp_start = strstr(buf_copy,"AT+CGSN\r\r\n");

    if (IMEI!=NULL)
    {
        k=0;
        while ((resp_start[10+k]!='\r') && (k<49))
        {
            IMEI[k] = resp_start[10+k];
            k++;
        }
        IMEI[k] = 0;
    }

    return 1;
}

uint8_t AT_SendStringUsingNIDD(uint8_t *str,uint32_t timeout)
{
    uint8_t cmd_buffer[512];
    uint16_t string_length;
    uint8_t error_state;

    //Determining the length of a string safely
    string_length=0;
    while ((str[string_length]!=0) && (string_length<450))
    {
        string_length++;
    }

    if (string_length==450)
    {
        //Possibly unterminated string?
        return 0;
    }

    snprintf(cmd_buffer,512,"AT+CSODCP=1,%d,\"%s\"\r\n",string_length,str);
    AT_SendCommand(cmd_buffer);

    AT_ReadReponseBuffer(NULL,0,NULL,NULL,&error_state,timeout);

    if (error_state!=0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

uint8_t AT_IsRegistered(uint32_t timeout)
{
    uint8_t resp[100];
    uint8_t error;
    uint32_t reg_state;
    uint8_t *resp_ptr;
    uint32_t res;

    AT_SendCommand("AT+CEREG?\r\n");
    AT_ReadReponseBuffer(resp,100,NULL,NULL,&error,timeout);

    resp_ptr = strstr(resp,"+CEREG:");

    if (resp_ptr == NULL)
    {
        error++;
    }
    else
    {
        res = sscanf(resp_ptr,"+CEREG: %*d,%d",&reg_state);

        if ((reg_state != 1) || (res == 0))
        {
            error++;
        }
    }

    if (error!=0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

uint8_t AT_SwitchPSM(uint8_t state,uint32_t timeout)
{
    uint8_t error;

    if (state!=0)
    {
        AT_SendCommand("AT+CPSMS=1\r\n");
    }
    else
    {
        AT_SendCommand("AT+CPSMS=0\r\n");
    }

    AT_ReadReponseBuffer(NULL,0,NULL,NULL,&error,timeout);

    return !(error);
}

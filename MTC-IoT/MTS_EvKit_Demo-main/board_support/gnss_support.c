#include "gnss_support.h"

/**
    @file gnss_support.c
    @author RedBees, LLC
    @copyright This file is distributed under the terms of the MIT license.
*/

static volatile uint8_t gnss_data_buffer[GNSS_DATA_BUFFER_SIZE];
static volatile uint16_t buf_index=0;
static volatile uint8_t GNSS_location_stored=0;
static volatile uint8_t GNSS_location_found=0;
#define LOC_ID_LENGTH           6
const uint8_t location_identifier[] = {"$GNGLL"};
static volatile uint8_t id_index=0;

void USART2_IRQHandler(void)
{
    volatile uint8_t tmp;
    volatile uint32_t SR_content;

    //Clearing every possible interrupt flag. Some are cleared by writing them to zero, and some by specific read sequence.
    SR_content = USART2->SR;
    tmp = USART2->DR;
    USART2->SR = 0;

    if (!GNSS_location_stored)
    {
        if (SR_content & USART_SR_ORE)
        {
            //Reset search because overrun has occurred
            buf_index=0;
            id_index=0;
        }

        gnss_data_buffer[buf_index] = tmp;
        if (buf_index<(GNSS_DATA_BUFFER_SIZE-1)) //Reserving space to terminate a string with a null character
        {
            buf_index++;
        }

        if ((tmp==location_identifier[id_index]) && (GNSS_location_found==0))
        {
            id_index++;

            if (id_index>=LOC_ID_LENGTH)
            {
                id_index=0;
                GNSS_location_found=1;
            }
        }
        else
        {
            id_index=0;

            if (GNSS_location_found==0)
            {
                buf_index=0; //To not save data that we do not want
            }
        }

        if (GNSS_location_found)
        {
            if ((gnss_data_buffer[buf_index-1]=='\n') && (gnss_data_buffer[buf_index-2]=='\r'))
            {
                //End of GNSS string, data ready
                //Terminate the string and do not include CR-LF
                gnss_data_buffer[buf_index-2]=0;
                //Reset the FSM
                GNSS_location_found=0;
                buf_index=0;
                id_index=0;
                //Signal that the location data are ready
                GNSS_location_stored=1;
            }
        }
    }
}

uint8_t GNSS_ReadLocationData(uint8_t *str_out,uint16_t max_length)
{
    uint16_t k;

    if (GNSS_location_stored)
    {
        k=0;
        if (str_out!=NULL)
        {
            while ((gnss_data_buffer[k]!=0) && (k<(max_length-1)))
            {
                str_out[k] = gnss_data_buffer[k];
                k++;
            }

            str_out[k]=0;
        }

        GNSS_location_stored=0;

        return 1;
    }
    else
    {
        return 0;
    }
}

#include "uart_api.h"

 /**
    @file uart_api.c
    @author RedBees, LLC
    @copyright This file is distributed under the terms of the MIT license.
 */

void UART_SendByte(USART_TypeDef* uart_inst,uint8_t b)
{
    uart_inst->DR = b;

    while (!(uart_inst->SR & USART_SR_TC))
    {
    }
}

void UART_SendString(USART_TypeDef* uart_inst,const uint8_t *str)
{
    uint16_t k;

    k=0;
    while ((str[k]!=0) && (k<UART_MAX_STRING_LENGTH))
    {
        UART_SendByte(uart_inst,str[k]);
        k++;
    }
}

uint8_t UART_WaitRxByte(USART_TypeDef* uart_inst)
{
    while (!(uart_inst->SR & USART_SR_RXNE))
    {
    }

    return uart_inst->DR;
}

void init_uart(USART_TypeDef* uart_inst,const uint32_t baudrate,const uint32_t sys_clk,uint8_t enable_rxne_int)
{
    const float temp_float_baudrate = ((float)sys_clk)/(16.0*baudrate);
    const uint32_t temp_baud_whole = (uint32_t)temp_float_baudrate;
    const uint32_t temp_baud_frac = (temp_float_baudrate - ((float)temp_baud_whole))*16.0;

    uart_inst->BRR = (temp_baud_whole<<4) | temp_baud_frac;

    if (enable_rxne_int)
    {
        uart_inst->CR1 = USART_CR1_RE | USART_CR1_TE | USART_CR1_RXNEIE | USART_CR1_UE;
    }
    else
    {
        uart_inst->CR1 = USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;
    }

    uart_inst->SR = 0;
}

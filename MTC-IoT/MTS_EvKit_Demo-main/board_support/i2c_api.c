#include "i2c_api.h"

 /**
    @file i2c_api.c
    @author RedBees, LLC
    @copyright This file is distributed under the terms of the MIT license.
 */

uint8_t __wait_SR1_bit(I2C_TypeDef* i2c_inst,uint16_t mask)
{
    volatile uint32_t safety_counter;

    safety_counter=0;
    while ((!(i2c_inst->SR1 & mask)) && (safety_counter<I2C_SAFETY_TIMEOUT))
    {
        safety_counter++;
    }

    if (safety_counter<I2C_SAFETY_TIMEOUT)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

inline uint8_t __generate_START(I2C_TypeDef* i2c_inst)
{
    i2c_inst->CR1 |= I2C_CR1_START;

    if (!__wait_SR1_bit(i2c_inst,I2C_SR1_SB))
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

inline uint8_t __read_single_byte(I2C_TypeDef* i2c_inst,uint8_t i2c_addr,uint8_t* byte_out)
{
    i2c_inst->DR = (i2c_addr<<1) | 1;

    if (!__wait_SR1_bit(i2c_inst,I2C_SR1_ADDR))
    {
        return 0;
    }

    //Now set NACK
    i2c_inst->CR1 &= ~I2C_CR1_ACK;

    if (!(i2c_inst->SR2 & I2C_SR2_MSL))
    {
        return 0;
    }

    //Now the byte reception is in progress, program STOP
    i2c_inst->CR1 |= I2C_CR1_STOP;

    if (!__wait_SR1_bit(i2c_inst,I2C_SR1_RXNE))
    {
        return 0;
    }

    (*byte_out) = i2c_inst->DR;

    return 1;
}

inline uint8_t __read_two_bytes(I2C_TypeDef* i2c_inst,uint8_t i2c_addr,uint8_t* bytes_out)
{
    i2c_inst->DR = (i2c_addr<<1) | 1;

    if (!__wait_SR1_bit(i2c_inst,I2C_SR1_ADDR))
    {
        return 0;
    }

    i2c_inst->CR1 &= ~I2C_CR1_ACK;
    i2c_inst->CR1 |= I2C_CR1_POS; //Magic bit

    if (!(i2c_inst->SR2 & I2C_SR2_MSL))
    {
        return 0;
    }

    if (!__wait_SR1_bit(i2c_inst,I2C_SR1_BTF))
    {
        return 0;
    }

    i2c_inst->CR1 |= I2C_CR1_STOP;

    bytes_out[0] = i2c_inst->DR;
    bytes_out[1] = i2c_inst->DR;

    i2c_inst->CR1 &= ~I2C_CR1_POS;

    return 1;
}

inline uint8_t __read_multiple_bytes(I2C_TypeDef* i2c_inst,uint8_t i2c_addr,uint8_t* bytes_out,uint16_t read_length)
{
    uint16_t k;

    if (read_length<3)
    {
        return 0; //Wrong usage!
    }

    i2c_inst->DR = (i2c_addr<<1) | 1;

    if (!__wait_SR1_bit(i2c_inst,I2C_SR1_ADDR))
    {
        return 0;
    }

    if (!(i2c_inst->SR2 & I2C_SR2_MSL))
    {
        return 0;
    }

    i2c_inst->CR1 |= I2C_CR1_ACK;

    for (k=0; k<read_length; k++)
    {
        if (k<(read_length-3))
        {
            if (!__wait_SR1_bit(i2c_inst,I2C_SR1_RXNE))
            {
                return 0;
            }

            bytes_out[k] = i2c_inst->DR;
        }
        else
        {
            if (k==(read_length-3))
            {
                if (!__wait_SR1_bit(i2c_inst,I2C_SR1_BTF))
                {
                    return 0;
                }
                i2c_inst->CR1 &= ~I2C_CR1_ACK;
                bytes_out[k] = i2c_inst->DR;
            }

            if (k==(read_length-2))
            {
                if (!__wait_SR1_bit(i2c_inst,I2C_SR1_BTF))
                {
                    return 0;
                }
                i2c_inst->CR1 |= I2C_CR1_STOP;
                bytes_out[k] = i2c_inst->DR;
            }

            if (k==(read_length-1))
            {
                bytes_out[k] = i2c_inst->DR;
            }
        }
    }

    return 1;
}

i2c_error_t i2c_read_with_pointer(I2C_TypeDef* i2c_inst,uint8_t i2c_addr,uint8_t pointer_byte,uint8_t read_length,uint8_t* read_buffer)
{
    volatile uint32_t safety_counter;

    if (read_length==0)
    {
        return I2C_ERROR_READ_FAIL;
    }

    if (read_buffer==NULL)
    {
        return I2C_ERROR_READ_FAIL;
    }

    //Step 1: generate START condition
    if (!__generate_START(i2c_inst))
    {
        return I2C_ERROR_START_FAIL;
    }

    //Step 2: transmit slave address with LSB reset, master transmitter mode
    i2c_inst->DR = i2c_addr<<1;

    if (!__wait_SR1_bit(i2c_inst,I2C_SR1_ADDR))
    {
        return I2C_ERROR_ADDR_FAIL;
    }

    if (!(i2c_inst->SR2 & I2C_SR2_MSL))
    {
        return I2C_ERROR_ADDR_FAIL;
    }

    i2c_inst->DR = pointer_byte; //Send pointer byte, as required by i2c devices protocol

    if (!__wait_SR1_bit(i2c_inst,I2C_SR1_TXE))
    {
        //For some reason, byte could not be transmitted
        return I2C_ERROR_READ_FAIL;
    }

    //Send repeated start
    if (!__generate_START(i2c_inst))
    {
        return I2C_ERROR_START_FAIL;
    }

    if (read_length==1)
    {
        if (!__read_single_byte(i2c_inst,i2c_addr,read_buffer))
        {
            return I2C_ERROR_READ_FAIL;
        }
    }

    if (read_length==2)
    {
        if (!__read_two_bytes(i2c_inst,i2c_addr,read_buffer))
        {
            return I2C_ERROR_READ_FAIL;
        }
    }

    if (read_length>2)
    {
        if (!__read_multiple_bytes(i2c_inst,i2c_addr,read_buffer,read_length))
        {
            return I2C_ERROR_READ_FAIL;
        }
    }

    safety_counter=0;
    while ((i2c_inst->SR2 & I2C_SR2_MSL) && (safety_counter<I2C_SAFETY_TIMEOUT))
    {
        safety_counter++;
    }

    return I2C_ERROR_NONE;
}

i2c_error_t i2c_write_with_pointer(I2C_TypeDef* i2c_inst,uint8_t i2c_addr,uint8_t pointer_byte,uint16_t write_length,uint8_t* write_buffer)
{
    uint16_t k;
    volatile uint32_t safety_counter;

    if (!__generate_START(i2c_inst))
    {
        return I2C_ERROR_START_FAIL;
    }

    i2c_inst->DR = i2c_addr<<1; //Send I2C address with LSB reset, master transmitter mode

    if (!__wait_SR1_bit(i2c_inst,I2C_SR1_ADDR))
    {
        return I2C_ERROR_ADDR_FAIL;
    }

    if (!(i2c_inst->SR2 & I2C_SR2_MSL))
    {
        return I2C_ERROR_ADDR_FAIL;
    }

    i2c_inst->DR = pointer_byte;

    if (!__wait_SR1_bit(i2c_inst,I2C_SR1_TXE))
    {
        return I2C_ERROR_WRITE_FAIL;
    }

    if (write_buffer!=NULL)
    {
        for (k=0; k<write_length; k++)
        {
            i2c_inst->DR = write_buffer[k];

            if (!__wait_SR1_bit(i2c_inst,I2C_SR1_TXE))
            {
                return I2C_ERROR_WRITE_FAIL;
            }
        }
    }

    if (!__wait_SR1_bit(i2c_inst,I2C_SR1_BTF))
    {
        return I2C_ERROR_WRITE_FAIL;
    }

    i2c_inst->CR1 |= I2C_CR1_STOP;

    //When all bus master activity is over, the interface will return to default slave mode
    safety_counter=0;
    while ((i2c_inst->SR2 & I2C_SR2_MSL) && (safety_counter<I2C_SAFETY_TIMEOUT))
    {
        safety_counter++;
    }

    return I2C_ERROR_NONE;
}

void init_i2c(I2C_TypeDef* i2c_inst,uint32_t sys_clk)
{
    i2c_inst->CR2 = sys_clk / 1000000UL;
    i2c_inst->CCR = sys_clk / 200000UL; //T_bus_high = T_bus_low = CCR periods of sys_clk = 0.5*(period of 100 kHz clock)
    i2c_inst->TRISE = (sys_clk / 1000000UL) + 1; //T_rise_max = 1 us for standard mode
    i2c_inst->CR1 = I2C_CR1_PE;
}

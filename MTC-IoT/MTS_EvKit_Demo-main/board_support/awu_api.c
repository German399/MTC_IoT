#include "awu_api.h"

 /**
    @file awu_api.c
    @author RedBees, LLC
    @copyright This file is distributed under the terms of the MIT license.
 */

static uint8_t awu_initialized=0;

void RTC_WKUP_IRQHandler(void)
{
    EXTI->PR = EXTI_PR_PR20;
    //This bit can be accessed without fully disabling the RTC write protection
    PWR->CR |= PWR_CR_DBP;
    RTC->ISR &= ~RTC_ISR_WUTF;
    PWR->CR &= ~PWR_CR_DBP;
}

void __unlock_RTC_registers(void)
{
    //Disable RTC domain write protection
    PWR->CR |= PWR_CR_DBP;
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;
}

void __lock_RTC_registers(void)
{
    //Re-enable RTC domain write protection
    PWR->CR &= ~PWR_CR_DBP;
    RTC->WPR = 0x00;
}

void AWU_Init(uint16_t wakeup_timeout,uint8_t timeout_is_in_seconds)
{
    uint32_t safety_counter;

    NVIC_DisableIRQ(RTC_WKUP_IRQn);

    __unlock_RTC_registers();

    EXTI->PR = EXTI_PR_PR20;
    //This bit can be accessed without disabling write protection
    RTC->ISR &= ~RTC_ISR_WUTF;

    RTC->CR &= ~(RTC_CR_WUTE | RTC_CR_WUTIE);

    safety_counter=0;
    while ((!(RTC->ISR & RTC_ISR_WUTWF)) && (safety_counter<AWU_MAX_WAIT))
    {
        safety_counter++;
    }

    if (timeout_is_in_seconds != 0)
    {
        RTC->WUTR = wakeup_timeout;
    }
    else
    {
        RTC->WUTR = (((uint32_t)wakeup_timeout)*4096UL)/1000UL - 1;
    }

    RTC->CR &= ~RTC_CR_WUCKSEL;

    if (timeout_is_in_seconds != 0)
    {
        //Set ck_spre (1 Hz) clock
        RTC->CR |= RTC_CR_WUCKSEL_2;
    }
    else
    {
        //Set LSE/8 clock frequency for AWU (4096 Hz at standard 32768 Hz crystal), approx. 244.14 us resolution
        RTC->CR |= RTC_CR_WUCKSEL_0;
    }

    //Enable AWU
    RTC->CR |= RTC_CR_WUTE | RTC_CR_WUTIE;

    __lock_RTC_registers();

    //Configure the interrupt for AWU
    EXTI->IMR |= EXTI_IMR_MR20;
    EXTI->RTSR |= EXTI_RTSR_TR20;

    NVIC_EnableIRQ(RTC_WKUP_IRQn);

    awu_initialized=1;
}

void AWU_GoStop(void)
{
    uint32_t safety_counter=0;

    if (!awu_initialized)
    {
        return;
    }

    SCB->SCR |= SCB_SCR_SLEEPDEEP;
    PWR->CR |= PWR_CR_LPSDSR | PWR_CR_CWUF;
    PWR->CR &= ~PWR_CR_PDDS;

    __WFI();

    RCC->CR |= RCC_CR_HSION;

    safety_counter=0;
    while ((!(RCC->CR & RCC_CR_HSIRDY)) && (safety_counter<AWU_MAX_WAIT))
    {
        safety_counter++;
    }

    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_HSI;

    safety_counter=0;
    while (((RCC->CFGR & RCC_CFGR_SW)==RCC_CFGR_SW_HSI) && (safety_counter<AWU_MAX_WAIT))
    {
        safety_counter++;
    }
}

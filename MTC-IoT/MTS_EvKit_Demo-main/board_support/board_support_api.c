#include "board_support_api.h"

 /**
    @file board_support_api.c
    @author RedBees, LLC
    @copyright This file is distributed under the terms of the MIT license.
 */

volatile uint8_t LSE_failed=0;
volatile uint32_t uptime_counter=0;
volatile uint32_t CYCLES_PER_1SEC=0;
volatile uint32_t button_press_counter=0;
volatile uint8_t button_press_detected=0;

/**
    @file board_drivers.c
    @ingroup board_support
    @brief Файл содержит функции, обеспечивающие доступ к используемой периферии контроллера и устройствам, расположенным на плате.
*/

/**
    @brief Переопределение функции работы с выходным потоком для использования printf() c USART2
*/
int _write(int fd, const void *buf, size_t count)
{
    size_t k;

    for (k=0; k<count; k++)
    {
        UART_SendByte(USART1,((uint8_t *)buf)[k]);
    }

    return count;
}

/**
    @brief Переопределение функции работы со входным потоком для использования scanf() с USART2
*/
int _read(int fd, const void *buf, size_t count)
{
    size_t k;

    for (k=0; k<count; k++)
    {
        ((uint8_t *)buf)[k] = UART_WaitRxByte(USART1);
#if ECHO_INPUT_CHARS == 1
        UART_SendByte(USART1,((uint8_t *)buf)[k]); //Echo
        #warning Firmware compiled with console echo option turned ON; this may cause problems with rapid input.
#endif
    }

    return count;
}

void TIM9_IRQHandler(void)
{
    if (TIM9->SR & TIM_SR_CC1IF)
    {
        uptime_counter++;

        TIM9->CCR1 += TMR_CNT_1MSEC;
    }

    if (TIM9->SR & TIM_SR_CC2IF)
    {
        //Button is active low
        if ((!(GPIOC->IDR & PIN_MASK(USER_SW_PC))) && (!button_press_detected))
        {
            button_press_counter++;
        }

        if ((GPIOC->IDR & PIN_MASK(USER_SW_PC)) && (!button_press_detected) && (button_press_counter!=0))
        {
            button_press_detected = 1;
        }

        TIM9->CCR2 += BTN_POLL_PERIOD;
    }

    TIM9->SR = 0;
}

uint32_t get_uptime_ms(void)
{
    return uptime_counter;
}

void delay_ms(uint32_t N)
{
    uint32_t wait_start;

    wait_start = get_uptime_ms();

    while ((get_uptime_ms() - wait_start)<N)
    {
    }
}

uint32_t get_button_press(void)
{
    uint32_t tmp;

    if (button_press_detected)
    {
        tmp = button_press_counter;
        button_press_counter = 0;
        button_press_detected = 0;

        return tmp;
    }
    else
    {
        return 0;
    }
}

void switch_LED(uint8_t state)
{
    if (state!=0)
    {
        GPIOA->BSRRH = PIN_MASK(USER_LED_PA);
    }
    else
    {
        GPIOA->BSRRL = PIN_MASK(USER_LED_PA);
    }
}

void SARA_R410_PWR_ON_pulse(void)
{
    volatile uint32_t k;

    //Notice that this signal gets inverted on board.
    GPIOC->BSRRL = PIN_MASK(NB_PWR_ON_PC);

    for (k=0; k<(CYCLES_PER_1SEC/2); k++)
    {
    }

    GPIOC->BSRRH = PIN_MASK(NB_PWR_ON_PC);
}

void FLASH_WriteEEPROM(volatile uint32_t *eeprom_ptr,uint32_t *data_ptr,uint16_t data_size_words)
{
    volatile uint16_t k;

    FLASH->PEKEYR = 0x89ABCDEF;
    FLASH->PEKEYR = 0x02030405;

    for (k=0; k<data_size_words; k++)
    {
        eeprom_ptr[k] = data_ptr[k];
    }

    FLASH->PECR = FLASH_PECR_PELOCK;
}

void store_device_settings(device_setup_data_t *data)
{
    //The easiest way to write to EEPROM area is by word at once
    FLASH_WriteEEPROM(EEPROM_START_ADDRESS,(uint32_t *)data,sizeof(device_setup_data_t)/4);
}

void recall_device_settings(device_setup_data_t *data)
{
    //Unlike writing, reading EERPOM area can be performed byte by byte, so no problem with memcpy()
    memcpy(data,EEPROM_START_ADDRESS,sizeof(device_setup_data_t));
}

void apply_backspace(uint8_t *str,uint16_t max_length)
{
    uint16_t ptr_r;
    uint16_t ptr_w;

    ptr_r = 0;
    ptr_w = 0;

    while ((str[ptr_r]!=0) && (ptr_r<max_length))
    {
        if (str[ptr_r]==0x7F)
        {
            /* Backspace found */
            if (ptr_w>0)
            {
                /* Each backspace shifts the write pointer one symbol left */
                ptr_w--;
            }
        }
        else
        {
            /* Symbols after the backspace character are written to the position, corrected by backspace sequence,
                effectively replacing the old input. */
            if (ptr_r!=ptr_w)
            {
                str[ptr_w] = str[ptr_r];
            }

            ptr_w++;
        }

        ptr_r++;
    }

    str[ptr_w] = 0;
}

void init_board(void)
{
    volatile uint32_t safety_counter;
    uint32_t calib_start;

    /*
        Clock system init
    */

    //The STM32L152RE uses MSI clock at 2.097 MHz at startup. Switching clock to 16 MHz HSI to use I2C more conveniently.
    RCC->CR |= RCC_CR_HSION;

    safety_counter=0;
    while ((!(RCC->CR & RCC_CR_HSIRDY)) && (safety_counter<OSC_STARTUP_CYCLES_MAX))
    {
        safety_counter++;
    }
	
	/*
        By default voltage regulator is in range 2 (1.5 V core voltage), and in this mode
        1 wait state is needed for 16 MHz
    */
    FLASH->ACR |= FLASH_ACR_ACC64;
    while (!(FLASH->ACR & FLASH_ACR_ACC64))
    {
    }
    FLASH->ACR |= FLASH_ACR_PRFTEN | FLASH_ACR_LATENCY;
    while (!(FLASH->ACR & FLASH_ACR_LATENCY))
    {
    }
	
    //Switch clock to HSI; after this write clock will be switched automatically after some time.
    RCC->CFGR |= RCC_CFGR_SW_HSI;

    /*
        Peripheral clock enable
    */
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOCEN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN | RCC_APB1ENR_USART2EN | RCC_APB1ENR_USART3EN;
    RCC->APB2ENR |= RCC_APB2ENR_TIM9EN | RCC_APB2ENR_TIM10EN | RCC_APB2ENR_USART1EN;

    /*
        USART1 is used for console I/O
        USART2 is used to communicate with the GNSS module
    */
    GPIOA->MODER  |= PIN_MODE(USART2_RX_PA,MODE_AF)   |
                     PIN_MODE(USART2_TX_PA,MODE_AF)   |
                     PIN_MODE(USART1_RX_PA,MODE_AF)   |
                     PIN_MODE(USART1_TX_PA,MODE_AF)   |
                     PIN_MODE(USER_LED_PA,MODE_GPOUT) |
                     PIN_MODE(NB_VINT_PA,MODE_GPIN);
    GPIOA->AFR[0] |= PIN_AFRL(USART2_RX_PA,USART123_AF_NO) |
                     PIN_AFRL(USART2_TX_PA,USART123_AF_NO);
    GPIOA->AFR[1] |= PIN_AFRH(USART1_RX_PA,USART123_AF_NO) |
                     PIN_AFRH(USART1_TX_PA,USART123_AF_NO);

    init_uart(USART1,CONSOLE_BAUDRATE,DEFAULT_CLOCK_FREQ,0);
    init_uart(USART2,GNSS_BAUDRATE,DEFAULT_CLOCK_FREQ,1);
    NVIC_EnableIRQ(USART2_IRQn);

    //To remedy possible lags
    setbuf(stdin,NULL);
    setbuf(stdout,NULL);

    printf("\r\n\r\nBoard started, console initialized.\r\n");

    RCC->APB1ENR |= RCC_APB1ENR_PWREN;

    //Check if LSE is already running; if not, start it in order to be able to use AWU
    if (!(RCC->CSR & RCC_CSR_LSERDY))
    {
        printf("Starting LSE...\r\n");
        //To enable LSE for AWU, we have first to deal with RCC registers protection, for it is controlled from RTC domain.
        PWR->CR |= PWR_CR_DBP;
        RTC->WPR = 0xCA;
        RTC->WPR = 0x53;
        RCC->CSR |= RCC_CSR_LSEON;

        safety_counter=0;
        while ((!(RCC->CSR & RCC_CSR_LSERDY)) && (safety_counter<OSC_STARTUP_CYCLES_MAX))
        {
            safety_counter++;
        }

        if (safety_counter>=OSC_STARTUP_CYCLES_MAX)
        {
            LSE_failed=1;
        }
        else
        {
            LSE_failed=0;
        }

        //Turn RTC clock ON and configure it to be LSE
        RCC->CSR |= RCC_CSR_RTCEN | RCC_CSR_RTCSEL_0;

        RTC->WPR = 0x00; //Reactivate write protection
        PWR->CR &= ~PWR_CR_DBP;
    }
    else
    {
        LSE_failed=0;
        printf("Found LSE already running.\r\n");
    }

    if (LSE_failed)
    {
        printf("(!) Failed to start LSE.\r\n");
    }

#ifdef CONFIGURE_CLOCK_OUT

#if defined(OBSERVE_LSE) && defined(OBSERVE_SYSCLK)
    #error Only one clock source can be monitored at once
#else

#ifdef OBSERVE_LSE
    RCC->CFGR |= RCC_CFGR_MCO_LSE;
    #warning Pin PA8 (Arduino-compatible: D7) is configured as MCO for LSE clock observation
    printf("NOTICE: PA8 (Arduino D7) is configured to output LSE clock.\r\n");
#endif
#ifdef OBSERVE_SYSCLK
    RCC->CFGR |= RCC_CFGR_MCO_SYSCLK;
    #warning Pin PA8 (Arduino-compatible: D7) is configured as MCO for SYSCLK clock observation
    printf("NOTICE: PA8 (Arduino D7) is configured to output SYSCLK.\r\n");
#endif

    GPIOA->MODER |= PIN_MODE(8,MODE_AF);
    GPIOA->AFR[1] |= PIN_AFRH(8,0);
#endif
#endif

    /*
        I2C1 is used for accelerometer and temperature sensor
    */
    GPIOB->BSRRH = PIN_MASK(NB_RTS_PB); //Set RTS low to enable USART for module revisions 00 and 01
    GPIOB->MODER  |= PIN_MODE(I2C1_SCL_PB,MODE_AF) | PIN_MODE(I2C1_SDA_PB,MODE_AF) | PIN_MODE(NB_RTS_PB,MODE_GPOUT);
    GPIOB->OTYPER |= PIN_MASK(I2C1_SCL_PB) | PIN_MASK(I2C1_SDA_PB);
    GPIOB->AFR[1] |= PIN_AFRH(I2C1_SCL_PB,I2C1_AF_NO) | PIN_AFRH(I2C1_SDA_PB,I2C1_AF_NO);

    init_i2c(I2C1,DEFAULT_CLOCK_FREQ);

    printf("I2C configured.\r\n");

    /*
        USART3 is used for communication with the NB-IoT module
    */

    //NB-IoT module PWR_ON and RESET_N inactive
    GPIOC->BSRRH = PIN_MASK(NB_PWR_ON_PC) | PIN_MASK(NB_RESET_PC);
    GPIOC->MODER |= PIN_MODE(USART3_TX_PC,MODE_AF) |
                    PIN_MODE(USART3_RX_PC,MODE_AF) |
                    PIN_MODE(NB_PWR_ON_PC,MODE_GPOUT) |
                    PIN_MODE(NB_RESET_PC,MODE_GPOUT) |
                    PIN_MODE(USER_SW_PC,MODE_GPIN);
    GPIOC->AFR[1] |= PIN_AFRH(USART3_TX_PC,USART3_AF_NO) | PIN_AFRH(USART3_RX_PC,USART3_AF_NO);

    init_uart(USART3,SARA_R410M_DEFAULT_BAUDRATE,DEFAULT_CLOCK_FREQ,1);
    NVIC_EnableIRQ(USART3_IRQn);

    printf("USART3 configured.\r\n");

    /*
        TIM9 is used for an uptime counter and button state check
    */

    TIM9->CCR1 = TMR_CNT_1MSEC;
    TIM9->CCR2 = BTN_POLL_PERIOD;
    TIM9->PSC = TMR_PRESC;
    TIM9->DIER = TIM_DIER_CC1IE | TIM_DIER_CC2IE;
    TIM9->CR1 = TIM_CR1_ARPE | TIM_CR1_CEN;
    TIM9->EGR = TIM_EGR_UG;

    NVIC_EnableIRQ(TIM9_IRQn);

    printf("Uptime timer started.\r\n");

    printf("Calibrating delay loop value...\r\n");
    CYCLES_PER_1SEC = 0;
    calib_start = get_uptime_ms();
    while ((get_uptime_ms() - calib_start)<1000)
    {
        CYCLES_PER_1SEC++;
    }

    printf("%d cycles per second.\r\n",CYCLES_PER_1SEC);
}

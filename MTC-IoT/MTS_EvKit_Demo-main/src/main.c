/**
    @mainpage Версия 2.6 beta, 31.07.2021

    @tableofcontents

    @section p0 Описание

    Основным предназначением прошивки отладочного комплекта МТС NB-IoT является иллюстрация принципов построения устройств и систем
    телеметрии, передающих данные через сеть LTE Cat. NB1 (NB-IoT), а также демонстрация методик работы с поставляемым аппаратным обеспечением.

    Прошивка реализует @link p3 сервисное меню@endlink, обеспечивающее доступ к функциям настройки и диагностики, а также предоставляющее прямой
    доступ к радиомодулю. Кроме этого доступен режим периодической передачи телеметрии.

    Взаимодействие с пользователем осуществляется в текстовом режиме через последовательный порт, доступ к которому обеспечивается с помощью
    преобразователя USB-UART, подключенного к USB-разъему XS8.

    @note Параметры подключения:
    @note До версии 2.1: 38400 бит/с, 8 бит данных, контроль четности не используется.
    @note Начиная с версии 2.1 beta: 115200 бит/с, 8 бит данных, контроль четности не используется.

    @warning Для удобства использования следует включить функцию local echo в используемой программе-эмуляторе терминала.

    С документацией на код можно ознакомиться в разделе "Группы".

    @section p1 Структура и особенности

    Исходный код имеет модульную структуру с выделенными слоями уровня абстракции от оборудования, драйверов установленных на плате устройств, поддержки
    протокола CoAP, а также собственно кода приложения.

    Исходный код не имеет внешних зависимостей и соответствует стандарту ISO C99.

    @note Исключениями являются зависимость от стандартной библиотеки языка Си и использование директив #warning и #error, являющихся расширениями, реализуемыми
    компилятором GCC.

    @note Разработка прошивки производилась в IDE EmBitz версии 1.11, компилятор arm-none-eabi-gcc 5.4.1 20160609.
    @warning При компиляции следует определить макрос __NO_SYSTEM_INIT в свойствах проекта для корректной работы функций инициализации системы тактирования.
    @warning В настройках проекта следует указать подключение опции поддержки чисел с плавающей запятой в printf().
    @warning Рекомендуется установить размер стека не менее 1 кБ.
	@warning В силу того, что на плате изначально расположен определенный набор периферийных устройств, не все выводы Arduino-совместимого разъема доступны для использования.
	Детали приведены в @link ardu_pins описании Arduino-совместимых выводов платы @endlink.

    @section p2 Формат телеметрии

    Телеметрия передается на сервер в формате, совместимом с JSON. Строка телеметрии имеет вид

    {'interface':'telemetry', 'ICCID':'[%s]', 'Tamb_degC':[%d], 'aX':[%d], 'aY':[%d], 'aZ':[%d], 'RSSI_dBm':[%d], 'latitude':[%.6f], 'longitude':[%.6f], 'GNSS_data_valid':[%d]}

    Она содержит маркер используемого интерфейса, идентификационный номер SIM-чипа, установленного на плате, значения температуры, измеренной датчиком,
    установленным на плате, проекций ускорения на оси акселерометра, уровень мощности сигнала сотовой сети, измеренный устройством, а также
    координаты GNSS с маркером их корректности.

    В случае передачи данных через IP строка инкапсулируется в сообщение CoAP, которое затем передается на сервер по протоколу UDP.

    В случае передачи данных без использования технологии IP, строка передается в исходном виде с помощью non-IP сервисов сети.

    Выбор метода передачи данных осуществляется в сервисном меню.

    @section p3 Сервисное меню

    Прошивка реализует меню настроек, предназначенное для установки рабочих параметров, диагностики, а также доступа к некоторым другим возможностям.

    @note Начиная с версии 2.6, доступны два вида меню: версия 1 (классическая) и версия 2, поддерживающая выбор пунктов меню клавишами "вверх"/"вниз"
    на клавиатуре и использующая управляющие последовательности ANSI для вывода текста с использованием цвета. Тип меню по-умолчанию может быть установлен
    в настройках.

    @note Использование меню версии 2
    @note Необходимый пункт меню можно выбрать клавишами "вверх"/"вниз" на клавиатуре. Недоступные функции отображаются приглушенным цветом.
    @note Нажатие клавиши 'r' инициирует полную перерисовку меню. Это может быть полезно, если отображение меню по какой-то причине было нарушено.
    @note Пункт меню можно быстро выбрать, введя его двузначный номер, отображаемый слева от его названия. Необходимо всегда вводить двузначный номер;
    то есть, например, для пункта меню номер 3 следует сначала нажать клавишу '0' и затем '3'.
    @note Троекратное нажатие клавиши Escape завершает меню версии 2 и приводит к выходу в классическое меню.

    @note Если прошивка функционирует в режиме периодической отсылки телеметрии (см. описание ниже), то для входа в сервисное меню следует нажать
    кнопку SB1 и, удерживая ее, перезагрузить контроллер кнопкой SB2, после чего удерживать SB1 до сообщения "(i) The device will enter service menu". Далее
    с помощью функции из сервисного меню (см. ниже) можно выбрать желаемый режим загрузки по умолчанию.

    @warning Перемычки XP10 и XP11 на плате должны быть установлены в положение по умолчанию.

    Если при старте платы в области EEPROM контроллера не обнаружено корректных настроек, либо при компиляции был определен макрос @link ALWAYS_ENTER_SETUP @endlink,
    вход в сервисное меню в любом случае осуществляется автоматически.

    @warning Система не анализирует семантику настроек; под корректными настройками подразумевается наличие структуры верного формата в EEPROM.

    @warning Внешний вид сервисного меню может незначительно отличаться в зависимости от версии прошивки. Актуальное описание функций приведено ниже.

    @note Приведенный ниже список опций соответствует версии прошивки 2.6 beta от 31.07.2021
    @note В случае расхождения этой документации и встроенных сообщений-подсказок прошивки приоритет следует отдавать встроенным сообщениям.

    1  - установить URL, по которому будут передаваться данные телеметрии \n

    @warning URL не должен включать IP-адрес и порт

    @warning URL должен начинаться с символа '/', например /api/v1/<TOKEN>/telemetry

    2  - установить IP-адрес сервера \n
    3  - установить номер UDP-порта, на котором сервер будет ожидать сообщения CoAP \n
    4  - установить APN для доступа с использованием NIDD; эта же опция позволяет включить либо выключить использование NIDD \n

    5  - инициировать однократную передачу пакета телеметрии с выбранными настройками (тест) \n
    6  - тест NIDD: ожидание входящих данных через канал NIDD в течение заданного интервала времени \n
    7  - тест функции OBSERVE протокола CoAP \n
    8  - режим ручной передачи AT-команд модулю \n

    @warning В режиме ручного обмена AT-командами невозможно наблюдение URC-сообщений, поскольку в простое контроллер ожидает ввода данных пользователя и не
    @warning анализирует состояние канала обмена данными с радиомодулем.

    @note Режим ручного обмена AT-командами поддерживает две специальные управляющие команды, не передаваемые радиомодулю:
    @note exit - выйти из режима прямой передачи AT-команд
    @note pwrp - генерировать импульс на выводе PWR_ON модуля; эта функция удобна при включенном режиме использования PSM (позволяет пробудить модуль).

    9  - режим прямой передачи данных радиомодулю \n

    @note В режиме прямой передачи данных радиомодулю возможно наблюдение URC-сообщений.

    @warning Выход из режима прямой передачи данных возможен только путем аппаратной перезагрузки платы.

    @note В режиме прямой передачи данных модули UART, используемые для обмена с пользователем и с радиомодулем, соединяются напрямую посредством блока DMA.
    Таким образом снимаются любые ограничения на вид передаваемой информации; в этом режиме микроконтроллер, установленный на плате, фактически эквивалентен
    отрезку провода; единственное несущественное отличие - задержка передачи на время одного байта. \n
    С другой стороны, поскольку микроконтроллер не осуществляет никакого анализа передаваемой информации, программный выход из режима прямой передачи данных невозможен.

    10  - команда идентификации: вывод ICCID, IMEI, IMSI, серийного номера микроконтроллера и т.п.

    11 - отображение информации о сети: RSSI, SNR, Cell ID, EARFCN \n

    12 - установка интервала передачи для режима периодической отсылки телеметрии \n
    13 - установка режима сокрытия данных об истинном местоположении \n

    @note Если эта настройка активна (значение по умолчанию), реальные данные с GNSS-модуля (при их доступности) не будут передаваться в пакете телеметрии.
    Вместо этого в пакет будут включены произвольные данные. Настройка не влияет на GNSS-координаты, выводимые в консоль.

    14 - установка режима функционирования прошивки \n

    @note Прошивка может запускаться в режиме сервисного меню либо демонстрации автоматической отсылки телеметрии. В режиме автоматической отсылки телеметрии
    происходит отправка на сервер данных с интервалом, определенным в п. 11. Вход в меню осуществляется способом, описанным выше.

    15 - чтение показаний датчиков, установленных на плате, и координат GNSS (если соответствующий модуль установлен и находится в состоянии готовности) \n

    16 - перезагрузка контроллера \n

    17 - программная перезагрузка радиомодуля \n

    18 - отображение настроек, сохраненных в EEPROM \n

    19 - установка версии меню, используемой по-умолчанию \n

    20 - вывод справки по использованию меню версии 2 \n

    21 - запуск меню версии 2 (доступно только из классического меню) \n

    22 - заводской тест и настройка; для использования на производстве (недоступно из меню версии 2). \n

    @section p4 Особенности работы режимов энергосбережения

    Важной особенностью сети LTE Cat NB1 является поддержка режима PSM, по сути предоставляющего возможность полностью отключить радиомодуль,
    но при этом не потерять регистрацию в сети; таким образом, при повторном его включении экономится время на регистрацию.

    С другой стороны, микроконтроллер STM32L152RE поддерживает режим STOP, также позволяющий снизить энергопотребление до незначительных значений.

    @note Для использования возможностей энергосбережения следует соответствующим образом определить макрос @link DO_NOT_GO_STOP @endlink.

    Указанные возможности позволяют эффективно экономить энергию источника питания; однако особенности работы радиомодуля накладывают некоторые ограничения
    на целесообразность использования энергосберегающих функций.

    Модуль переходит в режим PSM только самостоятельно, по истечении шести секунд с момента включения, либо по истечении времени, определяемого
    параметром сети T3324.

    По этой причине в случае, если интервал передачи телеметрии на сервер составляет менее шести секунд, использование режима PSM не имеет смысла,
    поскольку модуль не успеет перейти в него.

    В режиме сервисного меню функции энергосбережения недоступны.
*/

/**
    @file main.c
    @author RedBees, LLC
    @copyright This file is distributed under the terms of the MIT license.

    @defgroup fw_code Код демонстрационной прошивки
    @{
    @defgroup app_code Код логики демонстрационного приложения
    @{
*/

#include "stm32l1xx.h"
#include "board_support_api.h"
#include "menu_engine.h"

/**
    Отладочная опция: отключить переход контроллера в режим STOP между сеансами передачи телеметрии
    (радиомодуль также не конфигурируется для работы с использованием PSM; при активации этой опции следует убедиться, что использование PSM выключено).

    Требуется, если необходимо, чтобы программатор оставался подключенным к контроллеру (при переходе в режим STOP подключение теряется).
*/
#define DO_NOT_GO_STOP              0
/**
    Отладочная опция: всегда входить в сервисное меню
*/
#define ALWAYS_ENTER_SETUP          0

/**
    Использовать старый способ инициализации радиомодуля. Новый способ обеспечивает более быстрый и оптимальный старт.
*/
#define USE_OLD_MODULE_INIT         0

#ifndef __NO_SYSTEM_INIT
#error This firmware must be compiled with __NO_SYSTEM_INIT defined at project level.
#endif

/**
    @brief Инициализация радиомодуля

    Функция пытается включить и инициализировать радиомодуль. Если в течение пяти секунд с момента импульса включения модуль не ответил, то он либо уже был во включенном состоянии
    к моменту старта контроллера (например, если контроллер был перезагружен отдельно от радиомодуля), либо не был сконфигурирован должным образом
    (выключена функция приветствия при старте), либо неисправен. Функция проверяет, отвечает ли радиомодуль на команды. Если отвечает, значит модуль исправен.

    Далее производится проверка текущего профиля радиомодуля. Если профиль не соответствует заданному в настройках, проводится перенастройка модуля с последующей
    его перезагрузкой.

    @param [in] use_NIDD 1 - использовать NIDD для передачи данных; 0 - использовать IP для передачи данных.
    @param [in] NIDD_APN - строка, задающая APN для режима NIDD. В случае использования IP может принимать значение NULL.
    @return 1 - модуль инициализирован и готов к работе; 0 - произошла ошибка при инициализации.
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
    @brief Преобразует данные в формате NMEA (с широтой и долготой в градусах и минутах) в градусы с дробной частью.

    @param [in] NMEA_string_in Строка в формате NMEA (сообщение $GNGLL)
    @param [out] dd_lat_out Указатель на переменную типа double, в которую будет записана широта
    @param [out] dd_lon_out Указатель на переменную типа double, в которую будет записана долгота

    @return 1 - преобразование выполнено успешно; 0 - преобразование выполнить не удалось.
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
    @brief Передача телеметрии

    Телеметрия отправляется на сервер в виде строки следующего формата:
    {"interface":"telemetry", "ICCID":"%s", "Tamb_degC":%d, "aX":%d, "aY":%d, "aZ":%d, "RSSI_dBm":%d}

    Передается ICCID установленного SIM-чипа, температура, измеренная встроенным термодатчиком, проекции ускорения на оси акселерометра, а также
    уровень мощности сигнала сети, регистрируемый устройством.

    В случае использования протокола IP, строка инкапсулируется в сообщение CoAP с установленным методом CoAP POST, после чего отсылается
    в пакете UDP на указанный сервер и порт.

    В случае использования метода NIDD строка передается как есть через non-IP транспорт сети.

    @param [in] target_IP - строка, содержащая IP-адрес сервера, на который будет отослана телеметрия (имеет смысл только для метода IP)
    @param [in] target_url - строка, содержащая URL, на который будет отослана телеметрия (имеет смысл только для метода IP)
    @param [in] target_port - номер порта UDP, на который будет отослан пакет с телеметрией
    @param [in] ICCID_string - строка-индентификатор SIM-чипа для включения в телеметрию
    @param [in] use_NIDD - 1 - использовать метод NIDD для пересылки данных; 0 - использовать метод IP для пересылки данных
    @return 1 - телеметрия успешно отослана; 0 - произошла ошибка
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
    @brief Точка входа приложения.
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

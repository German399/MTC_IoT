#ifndef GNSS_SUPPORT_H_INCLUDED
#define GNSS_SUPPORT_H_INCLUDED

#include "stm32l1xx.h"

/**
    @file gnss_support.h
    @author RedBees, LLC
    @copyright This file is distributed under the terms of the MIT license.
*/

/**
    @ingroup device_drivers
    @defgroup gnss_support  од поддержки приема данных с GNSS-модул€
    @{
*/

#define GNSS_DATA_BUFFER_SIZE       1024

#ifndef NULL
#define NULL                        ((void *)0)
#endif

/**
    @brief ѕрочесть данные о местоположении, полученные от GNSS-приемника

    @param [out] str_out —трока, в которую будут скопированы данные о местоположении
    @param [in] max_length ƒлина передаваемой строки; может принимать значение NULL, если данные не требуютс€
    @return 1 - данные успешно скопированы; 0 - новые данные еще не получены

    @warning ѕрием данных производитс€ асинхронно в буфер, который замораживаетс€ после приема первой корректной строки местоположени€.
    “аким образом, вызов GNSS_ReadLocationData() возвращает первые данные, которые были получены после предыдущего вызова, а не самые свежие полученные данные.
    „тобы получить самые свежие данные, следует сначала перезапустить конечный автомат приема вызовом GNSS_ReadLocationData(NULL,0) и после этого дождатьс€,
    когда последующий вызов завершитс€ успехом.
*/
uint8_t GNSS_ReadLocationData(uint8_t *str_out,uint16_t max_length);

/**
    @}
*/

#endif /* GNSS_SUPPORT_H_INCLUDED */

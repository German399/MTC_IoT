#ifndef MENU_ENGINE_H_INCLUDED
#define MENU_ENGINE_H_INCLUDED

/**
    @ingroup app_code
    @{
    @defgroup service_menu Модуль, реализующий движок сервисного меню
    @{

    @file menu_engine.h
    @author RedBees, LLC
    @copyright This file is distributed under the terms of the MIT license.
*/

#include "menu_handlers.h"
#include "uart_api.h"

#define ESC_CHAR            27

void service_menu_v2(const menu_item_descriptor_t *menu_data);
void service_menu(void);

#endif /* MENU_ENGINE_H_INCLUDED */

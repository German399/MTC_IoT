#include "menu_engine.h"

/*
    ANSI escape sequences
    https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences
*/
#define CLEAR_SCREEN                "2J"
#define CURSOR_HOME                 "4;0H"
#define RESTORE_TERM                "0m"
#define HIDE_CURSOR                 "25l"

#define TEXT_BG_WHITE               "47m"
#define TEXT_COLOR_YELLOW           "33m"
#define TEXT_COLOR_BRIGHT_YELLOW    "93m"
#define TEXT_COLOR_GREEN            "32m"
#define TEXT_COLOR_BLACK            "30m"
#define TEXT_COLOR_WHITE            "37m"
#define TEXT_COLOR_BRIGHT_WHITE     "97m"

#define TEXT_BG_BLUE                "44m"
#define TEXT_BG_YELLOW              "43m"
#define TEXT_BG_GREEN               "42m"
#define TEXT_COLOR_BOLD_RED         "91m"

#define MENU_DRAW_OFFSET            4

/*
    This array contains numbers of items which will not be accessible through the "new" menu.
    The last item of this array must be 0xFFFF.
*/
const uint16_t inactive_menu_items[] = {20,21,0xFFFF};

/*
    Check whether menu item is enabled.
*/
uint8_t is_active(uint16_t n)
{
    uint16_t k;

    k=0;
    while (inactive_menu_items[k] != 0xFFFF)
    {
        if (inactive_menu_items[k] == n)
        {
            return 0;
        }
        k++;
    }

    return 1;
}

void esc(const uint8_t *sequence)
{
    printf("\x1B[%s",sequence);
}

void set_cursor_XY(uint8_t X,uint8_t Y)
{
    uint8_t esc_cmd[20];

    snprintf(esc_cmd,20,"\x1B[%d;%dH",Y,X);
    printf("%s",esc_cmd);
}

/*
    Draws the menu based on the description array.

    menu_data - array which describes menu items; the format and the array itself is shared with the old menu
    hightlighted_item_no - number of the item which will be highlighted
    previous_highlighted_item_no - used when redraw_only == 1; this is the second item to be drawn in this mode;
    redraw_only - draw only highlighted_item_no and previous_highlighted_item_no

    Returns the number of items drawn.
*/
uint16_t draw_menu_items(const menu_item_descriptor_t* menu_data,uint16_t highlighted_item_no,uint16_t previous_highlighted_item_no,uint8_t redraw_only)
{
    uint16_t k;

    esc(HIDE_CURSOR);
    esc(CURSOR_HOME);
    esc(TEXT_COLOR_YELLOW);

    if (redraw_only)
    {
        k=2;

        set_cursor_XY(0,MENU_DRAW_OFFSET+previous_highlighted_item_no);
        esc(TEXT_BG_BLUE);
        if (is_active(previous_highlighted_item_no))
        {
            esc(TEXT_COLOR_BRIGHT_YELLOW);
        }
        else
        {
            esc(TEXT_COLOR_YELLOW);
        }
        printf("                                                                                          \r");
        printf("[%02d] %s",previous_highlighted_item_no + 1,menu_data[previous_highlighted_item_no].item_string_description);
        set_cursor_XY(0,MENU_DRAW_OFFSET+highlighted_item_no);
        esc(TEXT_BG_YELLOW);
        if (is_active(highlighted_item_no))
        {
            esc(TEXT_COLOR_BLACK);
        }
        else
        {
            esc(TEXT_COLOR_WHITE);
        }
        printf("                                                                                          \r");
        printf("[%02d] %s\r",highlighted_item_no + 1,menu_data[highlighted_item_no].item_string_description);

        esc(RESTORE_TERM);

        return k;
    }

    k=0;
    while ((menu_data[k].item_string_description[0] != '\0') || (menu_data[k].menu_item_handler != NULL))
    {
        if (k == highlighted_item_no)
        {
            esc(TEXT_BG_YELLOW);

            if (is_active(k))
            {
                esc(TEXT_COLOR_BLACK);
            }
            else
            {
                esc(TEXT_COLOR_WHITE);
            }
        }
        else
        {
            esc(TEXT_BG_BLUE);
            esc(TEXT_COLOR_YELLOW);

            if (is_active(k))
            {
                esc(TEXT_COLOR_BRIGHT_YELLOW);
            }
            else
            {
                esc(TEXT_COLOR_YELLOW);
            }
        }

        printf("                                                                                          \r");
        printf("[%02d] %s",k+1,menu_data[k].item_string_description);

        printf("\r\n");

        k++;
    }

    esc(RESTORE_TERM);

    return k;
}

void draw_menu_header(void)
{
    esc("0;0H");
    esc(CLEAR_SCREEN);
    esc(TEXT_COLOR_BLACK);
    esc(TEXT_BG_GREEN);
    printf("                *** Welcome to MTS NB-IoT Development Kit service menu ***                \r\n");
    printf("                                                                                          \r");
    printf("Firmware version: %s",FIRMWARE_VERSION);
}

/**
    @brief Функция, реализующая новое сервисное меню. См. описание сервисного меню на главной странице.
*/
void service_menu_v2(const menu_item_descriptor_t *menu_data)
{
    uint8_t ch;
    uint8_t esc_seq_counter=0;
    uint8_t exit_counter=0;
    uint16_t selected_item_no = 0;
    uint16_t previous_selected_item_no=0;
    uint16_t menu_items_count;
    device_setup_data_t settings;
    uint8_t shortcut_number_array[2];
    uint8_t shortcut_number_counter;

    recall_device_settings(&settings);

    printf("\x1B]2;MTS Development Kit Service Menu\x07"); //Setting the window title
    printf("\x1B[?3h"); //Set number of columns to 132

    set_cursor_XY(0,0);
    draw_menu_header();
    menu_items_count = draw_menu_items(menu_data,0,0,0);
    shortcut_number_counter = 0;

    while (1)
    {
        ch = UART_WaitRxByte(USART1);

        /*
            Shortcut mechanism: if two numeric symbols are entered, set selected item to this value
        */
        if ((ch >= '0') && (ch <= '9'))
        {
            shortcut_number_array[shortcut_number_counter] = ch;
            shortcut_number_counter++;

            if (shortcut_number_counter >= 2)
            {
                shortcut_number_counter = 0;
                /*
                    Subtracting 1 to account for shift made while drawing.
                    Array indices are zero-based, but menu numbers are drawn one-based for better user experience.
                */
                selected_item_no = (shortcut_number_array[1] - '0') + (shortcut_number_array[0] - '0')*10 - 1;

                if (selected_item_no >= menu_items_count)
                {
                    selected_item_no = menu_items_count;
                }

                draw_menu_items(menu_data,selected_item_no,previous_selected_item_no,1);
                previous_selected_item_no = selected_item_no;
            }
        }
        else
        {
            shortcut_number_counter = 0;
        }

        /*
            r for "redraw"
        */
        if (ch == 'r')
        {
            printf("\x1B]2;MTS Development Kit Service Menu\x07"); //Setting the window title
            printf("\x1B[?3h"); //Set number of columns to 132

            set_cursor_XY(0,0);
            draw_menu_header();
            menu_items_count = draw_menu_items(menu_data,0,0,0);
        }

        /*
            Enter
        */
        if (ch == 0x0D)
        {
            esc(CLEAR_SCREEN);
            esc(RESTORE_TERM);
            set_cursor_XY(0,0);
            if (is_active(selected_item_no))
            {
                menu_data[selected_item_no].menu_item_handler(&settings);
                printf("\r\n\r\nFunction OK; press enter to return to menu.\r\n");
            }
            else
            {
                printf("This function is not available from this menu; please try calling it from the basic menu.\r\n");
                printf("Press Enter to return.\r\n");
            }
            ch = UART_WaitRxByte(USART1);
            esc(CLEAR_SCREEN);
            esc(RESTORE_TERM);
            draw_menu_header();
            draw_menu_items(menu_data,selected_item_no,previous_selected_item_no,0);
        }

        /*
            Escape. Pressing Escape three times will make this menu finish.
        */
        if (ch == 0x1B)
        {
            esc_seq_counter = 1;
            exit_counter++;
        }

        /*
            Up/Down escape sequence mechanism.
        */
        if ((esc_seq_counter == 1) && (ch == '['))
        {
            esc_seq_counter++;
            exit_counter = 0;
        }

        if (exit_counter == 3)
        {
            break;
        }

        if (esc_seq_counter == 2)
        {
            if (ch == 'A')
            {
                esc_seq_counter = 0;

                if (selected_item_no > 0)
                {
                    selected_item_no--;
                }
            }

            if (ch == 'B')
            {
                esc_seq_counter = 0;

                if (selected_item_no < (menu_items_count - 1))
                {
                    selected_item_no++;
                }
            }

            draw_menu_items(menu_data,selected_item_no,previous_selected_item_no,1);
            previous_selected_item_no = selected_item_no;
        }
    }

    esc(CLEAR_SCREEN);
    set_cursor_XY(0,0);
}

/**
    @brief Функция, реализующая классическое сервисное меню. См. описание сервисного меню на главной странице.
*/
void service_menu(void)
{
    uint8_t user_choice;
    device_setup_data_t settings;
    uint16_t k;
    uint8_t user_input[80];

    printf("\r\n*** Welcome to MTS NB-IoT Development Kit service menu ***\r\nFirmware version: %s\r\n\r\n",FIRMWARE_VERSION);

    printf("Current settings found in EEPROM:\r\n\r\n");

    recall_device_settings(&settings);

    if (settings.target_server_IP[0]=='\0')
    {
        printf("Warning: target server IP not set.\r\n");
    }
    else
    {
        printf("Target IP:\t\t%s\r\n",settings.target_server_IP);
    }

    printf("Target port:\t\t%d\r\n",settings.target_server_port);

    if ((!is_alphanumeric(settings.target_URL[1])) || (settings.target_URL[0]=='\0'))
    {
        printf("Warning: target URL not set.\r\n");
    }
    else
    {
        printf("Target URL:\t\t%s\r\n",settings.target_URL);
    }

    if ((!is_alphanumeric(settings.NIDD_APN_name[1])) || (settings.NIDD_APN_name[0]=='\0'))
    {
        printf("Warning: APN name not set.\r\n");
    }
    else
    {
        printf("NIDD APN:\t\t%s\r\n",settings.NIDD_APN_name);
    }

    printf("Use NIDD for telemetry:\t%d\r\n",settings.use_NIDD);

    printf("Board mode on startup:\t");

    if (settings.logger_mode_on_startup==LOGGER_MODE_VALUE)
    {
        printf("logger\r\n");
    }
    else
    {
        printf("service menu\r\n");
    }

    printf("Telemetry interval\r\n(in logger mode):\t%d ms\r\n",settings.telemetry_interval_ms);
    printf("GNSS privacy mode:\t%d\r\n",settings.gnss_privacy_mode);

    printf("\r\nType in a function number from a list below and press enter.\r\n\r\n");

    k=0;
    while ((menu_items[k].menu_item_handler!=NULL) && (menu_items[k].item_string_description[0]!='\0'))
    {
        if (k==0)
        {
            printf("Target server setup:\r\n");
        }
        if (k==4)
        {
            printf("System functions:\r\n");
        }
        printf("\t%d\t- %s\r\n",k+1,menu_items[k].item_string_description);
        k++;
    }

    printf("> ");
    scanf("%s",user_input);
    apply_backspace(user_input,80);
    user_choice = atoi(user_input);

    user_choice--; //Array numbering is zero-based, but menu items are numbered starting from one not to confuse the user

    printf("\r\n--------------------------------------------------------------------------------\r\n");

    if (user_choice>=k)
    {
        printf("Unknown function - %d",user_input+1);
    }
    else
    {
        menu_items[user_choice].menu_item_handler(&settings);
    }

    printf("\r\n--------------------------------------------------------------------------------\r\n");

    printf("Press Enter to return to service menu.\r\n");
    getchar();
    getchar();
    printf("\r\n");
}

/* @file fm_menu_user.c
 *
 * @brief Este modulo se encarga de la impresión de menús que son accesibles por
 * el usuario del caudalímetro en la pantalla LCD. Para ello hace uso de la
 * librería de mas bajo nivel fm_lcd.h que se encarga de la inteligencia para
 * este fin.
 *
 * IMPORTANTE: En un futuro se creará una máquina de estados basada en punteros
 * a función que se vincule directamente con esta librería para el pasaje entre
 * menús. Por el momento solo se implementará una versión rudimentaria de la
 * misma en una tarea del sistema operativo para chequear su funcionalidad, pero
 * puede que en el futuro esto se pase a otra libreria llamada por ejemplo
 * fm_fsm_menu_user.h o algo similar (sand).
 *
 * COPYRIGHT NOTE: (c) 2023 FLOWMEET. All right reserved.
 *
 */

// Includes.
#include "fm_menu_user.h"
#include "stdio.h"
#include "../fm_debug/fm_debug.h"
#include "../fm_calendar/fm_calendar.h"

// Typedef.

/*
 * The names of all new data types, including structures, unions, and
 * enumerations, shall consist only of lowercase characters and internal
 * underscores and end with ‘_t’.
 *
 * All new structures, unions, and enumerations shall be named via a typedef.
 *
 */

// Const data.
// Defines.
// Debug.
/*
 * To temporally disable a block of code, use preprocessor's conditional
 * compilation features, eg, the following one should be used to increase the
 * the debug output information.
 *
 */
#ifndef NDEBUG
#endif

// Project variables, non-static, at least used in other file.

// External variables.

extern TX_QUEUE event_queue_ptr;

// Global variables, statics.

// Private function prototypes.

// Private function bodies.

// Public function bodies.

/*
 * @brief Función que imprime el menú de acm y rate en la pantalla, con
 * unidades y puntos específicos.
 * @param  Evento de presión de botones o refresh.
 * @retval Puntero al retorno de la función.
 */
ptr_ret_menu_t fm_menu_show_acm_rate(fm_event_t event_id)
{
    static uint8_t new_entry = 1;
    static uint8_t new_exit = 0;

    ptr_ret_menu_t ret_menu = (ptr_ret_menu_t) fm_menu_show_acm_rate;
    fm_event_t event_now;

    if (new_entry == 1)
    {
        fm_lcd_clear();
        new_entry = 0;
    }

    fm_lcd_acm_rate();
    fm_lcd_refresh();

    switch (event_id)
    {
        case EVENT_KEY_UP:
            new_exit = 1;
            ret_menu = (ptr_ret_menu_t) fm_menu_show_ttl_rate;
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_KEY_DOWN:
            new_exit = 1;
            ret_menu = (ptr_ret_menu_t) fm_menu_show_acm_temp;
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_KEY_ENTER:
        break;
        case EVENT_KEY_ESC:
            new_exit = 1;
            ret_menu = (ptr_ret_menu_t) fm_menu_config_pass;
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_LCD_REFRESH:
        break;
        default:
        break;
    }

#ifdef FM_DEBUG_MENU
    char msg_buffer[] = "acm_rate\n";
    fm_debug_msg_uart((uint8_t*) msg_buffer, sizeof(msg_buffer));
#endif

    if (new_exit == 1)
    {
        new_entry = 1;
        new_exit = 0;
    }

    return (ret_menu);
}

/*
 * @brief Función que imprime el menú de acm y temperatura del micro en
 * la pantalla.
 * @param  Evento de presión de botones o refresh.
 * @retval Puntero al retorno de la función.
 */
ptr_ret_menu_t fm_menu_show_acm_temp(fm_event_t event_id)
{
    static uint8_t new_entry = 1;
    static uint8_t new_exit = 0;

    ptr_ret_menu_t ret_menu = (ptr_ret_menu_t) fm_menu_show_acm_temp;
    fm_event_t event_now;

    if (new_entry == 1)
    {
        fm_lcd_clear();
        new_entry = 0;
    }

    fm_lcd_acm_temp();
    fm_lcd_refresh();

    switch (event_id)
    {
        case EVENT_KEY_UP:
            new_exit = 1;
            ret_menu = (ptr_ret_menu_t) fm_menu_show_acm_rate;
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_KEY_DOWN:
            new_exit = 1;
            ret_menu = (ptr_ret_menu_t) fm_menu_show_date_hour;
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_KEY_ENTER:
        break;
        case EVENT_KEY_ESC:
            new_exit = 1;
            ret_menu = (ptr_ret_menu_t) fm_menu_config_pass;
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_LCD_REFRESH:
        break;
        default:
        break;
    }

#ifdef FM_DEBUG_MENU
    char msg_buffer[] = "acm_temp\n";
    fm_debug_msg_uart((uint8_t*) msg_buffer, sizeof(msg_buffer));
#endif

    if (new_exit == 1)
    {
        new_entry = 1;
        new_exit = 0;
    }

    return (ret_menu);
}

/*
 * @brief Función que imprime el menú de fecha y hora en la pantalla, con
 * unidades y puntos específicos.
 * @param  Evento de presión de botones o refresh.
 * @retval Puntero al retorno de la función.
 */
ptr_ret_menu_t fm_menu_show_date_hour(fm_event_t event_id)
{
    static uint8_t new_entry = 1;
    static uint8_t new_exit = 0;

    ptr_ret_menu_t ret_menu = (ptr_ret_menu_t) fm_menu_show_date_hour;
    fm_event_t event_now;

    if (new_entry == 1)
    {
        fm_lcd_clear();
        new_entry = 0;
    }

    fm_lcd_date_hour(USER, event_id, DAY);

    fm_lcd_refresh();

    switch (event_id)
    {
        case EVENT_KEY_UP:
            new_exit = 1;
            ret_menu = (ptr_ret_menu_t) fm_menu_show_acm_temp;
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_KEY_DOWN:
        break;
        case EVENT_KEY_ENTER:
        break;
        case EVENT_KEY_ESC:
            new_exit = 1;
            ret_menu = (ptr_ret_menu_t) fm_menu_config_pass;
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_LCD_REFRESH:
        break;
        default:
        break;
    }

#ifdef FM_DEBUG_MENU
    char msg_buffer3[] = "date_hour\n";
    fm_debug_msg_uart((uint8_t*) msg_buffer3, sizeof(msg_buffer3));
#endif

    if (new_exit == 1)
    {
        new_entry = 1;
        new_exit = 0;
    }

    return (ret_menu);
}

ptr_ret_menu_t fm_menu_show_init(fm_event_t event_id)
{
    static uint8_t new_entry = 1;
    static uint8_t new_exit = 0;
    static uint8_t counter = 0;
    const  uint8_t counter_max = 30;

    ptr_ret_menu_t ret_menu = (ptr_ret_menu_t) fm_menu_show_init;
    fm_event_t event_now;

    if (new_entry == 1)
    {
        fm_lcd_init();
        fm_lcd_clear();
        new_entry = 0;
    }

    pcf8553_write_all(0xFF); // @suppress("Avoid magic numbers")

    switch (event_id)
    {
        case EVENT_KEY_UP:
        break;
        case EVENT_KEY_DOWN:
        break;
        case EVENT_KEY_ENTER:
            tx_queue_flush(&event_queue_ptr);
            ret_menu = (ptr_ret_menu_t) fm_menu_show_version;
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
            new_exit = 1;
        break;
        case EVENT_KEY_ESC:
        break;
        case EVENT_LCD_REFRESH:
            if(counter < counter_max)
            {
                HAL_Delay(100); // @suppress("Avoid magic numbers")
                counter++;
                event_now = EVENT_LCD_REFRESH;
                tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
            }
            else
            {
                tx_queue_flush(&event_queue_ptr);
                ret_menu = (ptr_ret_menu_t) fm_menu_show_version;
                event_now = EVENT_LCD_REFRESH;
                tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
                new_exit = 1;
            }
        break;
        default:
        break;
    }

#ifdef FM_DEBUG_MENU
    char msg_buffer[] = "INIT\n";
    fm_debug_msg_uart((uint8_t*) msg_buffer, sizeof(msg_buffer));
#endif

    if (new_exit == 1)
    {
        counter = 0;
        new_entry = 1;
        new_exit = 0;
    }

    return (ret_menu);
}

/*
 * @brief Función que imprime el menú de ttl y rate en la pantalla, con
 * unidades y puntos específicos.
 * @param  Evento de presión de botones o refresh.
 * @retval Puntero al retorno de la función.
 */
ptr_ret_menu_t fm_menu_show_ttl_rate(fm_event_t event_id)
{
    static uint8_t new_entry = 1;
    static uint8_t new_exit = 0;

    ptr_ret_menu_t ret_menu = (ptr_ret_menu_t) fm_menu_show_ttl_rate;
    fm_event_t event_now;

    if (new_entry == 1)
    {
        fm_lcd_clear();
        new_entry = 0;
    }

    fm_lcd_ttl_rate();
    fm_lcd_refresh();

    switch (event_id)
    {
        case EVENT_KEY_UP:
        break;
        case EVENT_KEY_DOWN:
            new_exit = 1;
            ret_menu = (ptr_ret_menu_t) fm_menu_show_acm_rate;
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_KEY_ENTER:
        break;
        case EVENT_KEY_ESC:
            new_exit = 1;
            ret_menu = (ptr_ret_menu_t) fm_menu_config_pass;
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_LCD_REFRESH:
        break;
        default:
        break;
    }

#ifdef FM_DEBUG_MENU
    char msg_buffer[] = "ttl_rate\n";
    fm_debug_msg_uart((uint8_t*) msg_buffer, sizeof(msg_buffer));
#endif

    if (new_exit == 1)
    {
        new_entry = 1;
        new_exit = 0;
    }

    return (ret_menu);
}

/*
 * @brief Función que imprime el menú con la versión del caudalímetro en la
 * pantalla, con puntos específicos.
 * @param  Evento de presión de botones o refresh.
 * @retval Puntero al retorno de la función.
 */
ptr_ret_menu_t fm_menu_show_version(fm_event_t event_id)
{
    static uint8_t new_entry = 1;
    static uint8_t new_exit = 0;
    static uint8_t counter = 0;
    const  uint8_t counter_max = 30;

    ptr_ret_menu_t ret_menu = (ptr_ret_menu_t) fm_menu_show_version;
    fm_event_t event_now;

    if (new_entry == 1)
    {
        fm_lcd_clear();
        new_entry = 0;
    }

    fm_lcd_version(PNT_4, PNT_5);
    fm_lcd_refresh();

    switch (event_id)
    {
        case EVENT_KEY_UP:
        break;
        case EVENT_KEY_DOWN:
        break;
        case EVENT_KEY_ENTER:
            tx_queue_flush(&event_queue_ptr);
            ret_menu = (ptr_ret_menu_t) fm_menu_show_ttl_rate;
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
            new_exit = 1;
        break;
        case EVENT_KEY_ESC:
        break;
        case EVENT_LCD_REFRESH:
            if(counter < counter_max)
            {
                HAL_Delay(100); // @suppress("Avoid magic numbers")
                counter++;
                event_now = EVENT_LCD_REFRESH;
                tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
            }
            else
            {
                tx_queue_flush(&event_queue_ptr);
                ret_menu = (ptr_ret_menu_t) fm_menu_show_ttl_rate;
                event_now = EVENT_LCD_REFRESH;
                tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
                new_exit = 1;
            }
        break;
        default:
        break;
    }

#ifdef FM_DEBUG_MENU
    char msg_buffer[] = "version\n";
    fm_debug_msg_uart((uint8_t*) msg_buffer, sizeof(msg_buffer));
#endif

    if (new_exit == 1)
    {
        counter = 0;
        new_entry = 1;
        new_exit = 0;
    }

    return (ret_menu);
}

// Interrupts

/*** end of file ***/

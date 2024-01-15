/* @file fm_menu_config.c
 *
 * @brief
 *
 * COPYRIGHT NOTE: (c) 2023 FLOWMEET. All right reserved.
 *
 */

// Includes.
#include "fm_menu_config.h"
#include "stdio.h"
#include "stdlib.h"
#include "../fm_debug/fm_debug.h"
#include "../fm_calendar/fm_calendar.h"
#include "../fm_factory/fm_factory.h"

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
#define PASSWORD_LENGTH 4
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

int freeze_time;
int freeze_date;
uint8_t correct_password;
fm_event_t previous_event;
uint8_t in_configuration;

// External variables.

extern TX_QUEUE event_queue_ptr;

// Global variables, statics.

// Private function prototypes.

// Private function bodies.

// Public function bodies.

/*
 * @brief Función que imprime el menú de configuración de fecha y hora.
 * @param  Evento de presión de botones o refresh.
 * @retval Puntero al retorno de la función.
 */
ptr_ret_menu_t fm_menu_config_date_hour(fm_event_t event_id)
{
    static uint8_t new_entry = 1;
    static uint8_t new_exit = 0;
    static sel_date_time_field_t field;
    static sel_day day_enum;
    static sel_month month_enum;
    static sel_year year_enum;
    static sel_hour hour_enum;
    static sel_minute minute_enum;
    static sel_second second_enum;
    RTC_TimeTypeDef time_final;
    RTC_DateTypeDef date_final;
    extern RTC_HandleTypeDef hrtc;

    ptr_ret_menu_t ret_menu = (ptr_ret_menu_t) fm_menu_config_date_hour;
    fm_event_t event_now;

    if (new_entry == 1)
    {
        field = DAY;
        fm_lcd_clear();
        new_entry = 0;
    }

    fm_lcd_date_hour(CONFIGURATION, event_id, field);
    fm_lcd_refresh();

    day_enum = fm_factory_get_date_time().day;
    month_enum = fm_factory_get_date_time().month;
    year_enum = fm_factory_get_date_time().year;
    hour_enum = fm_factory_get_date_time().hour;
    minute_enum = fm_factory_get_date_time().minute;
    second_enum = fm_factory_get_date_time().second;

    switch (event_id)
    {
        case EVENT_KEY_UP:
            if(correct_password)
            {
                /*
                 * En toda esta sección se realiza el calculo para modificar la
                 * fecha y hora del calendario del RTC. Para eso se debe tener
                 * en cuenta la cantidad de días de cada mes, si es un año
                 * bisiesto o no, y que al pasarse del valor máximo o mínimo de
                 * cada parámetro, se vuelva a iniciar desde el valor mínimo o
                 * máximo respectivamente.
                 */
                if(field == DAY)
                {
                    if(month_enum == JANUARY ||
                    month_enum == MARCH      ||
                    month_enum == MAY        ||
                    month_enum == JULY       ||
                    month_enum == AUGUST     ||
                    month_enum == OCTOBER    ||
                    month_enum == DECEMBER)
                    {
                        if(day_enum < DAY_31)
                        {
                            fm_factory_modify_date(day_enum +
                            1, month_enum,
                            year_enum);
                        }
                        else
                        {
                            fm_factory_modify_date(DAY_1,
                            month_enum,
                            year_enum);
                        }
                    }
                    else if(month_enum == APRIL ||
                    month_enum == JUNE          ||
                    month_enum == SEPTEMBER     ||
                    month_enum == NOVEMBER)
                    {
                        if(day_enum < DAY_30)
                        {
                            fm_factory_modify_date(day_enum + 1, month_enum,
                            year_enum);
                        }
                        else
                        {
                            fm_factory_modify_date(DAY_1, month_enum,
                            year_enum);
                        }
                    }
                    else
                    {
                        if(year_enum % YEAR_4 == 0)
                        {
                            if(day_enum < DAY_29)
                            {
                                fm_factory_modify_date(day_enum + 1, month_enum,
                                year_enum);
                            }
                            else
                            {
                                fm_factory_modify_date(DAY_1, month_enum,
                                year_enum);
                            }
                        }
                        else
                        {
                            if(day_enum < DAY_28)
                            {
                                fm_factory_modify_date(day_enum + 1, month_enum,
                                year_enum);
                            }
                            else
                            {
                                fm_factory_modify_date(DAY_1, month_enum,
                                year_enum);
                            }
                        }
                    }
                }
                else if(field == MONTH)
                {
                    if(month_enum < DECEMBER)
                    {
                        if((month_enum + 1 == APRIL ||
                        month_enum + 1 == JUNE      ||
                        month_enum + 1 == SEPTEMBER ||
                        month_enum + 1 == NOVEMBER)
                        && (day_enum > DAY_30))
                        {
                            day_enum = DAY_30;
                        }
                        else if((month_enum + 1 == FEBRUARY) &&
                        (year_enum % YEAR_4 == 0) &&
                        (day_enum > DAY_29))
                        {
                            day_enum = DAY_29;
                        }
                        else if((month_enum + 1 == FEBRUARY) &&
                        (year_enum % YEAR_4 != 0) &&
                        (day_enum > DAY_28))
                        {
                            day_enum = DAY_28;
                        }
                        fm_factory_modify_date(day_enum, month_enum + 1,
                        year_enum);
                    }
                    else
                    {
                        fm_factory_modify_date(day_enum, JANUARY, year_enum);
                    }
                }
                else if(field == YEAR)
                {
                    if(year_enum < YEAR_99)
                    {
                        if(((year_enum + 1) % YEAR_4 != YEAR_0) &&
                        (month_enum == FEBRUARY) &&
                        (day_enum > DAY_28))
                        {
                            day_enum = DAY_28;
                        }
                        fm_factory_modify_date(day_enum, month_enum, year_enum
                        + 1);
                    }
                    else
                    {
                        fm_factory_modify_date(day_enum, month_enum, YEAR_0);
                    }
                }
                else if(field == HOUR)
                {
                    if(hour_enum < HOUR_23)
                    {
                        fm_factory_modify_time(hour_enum + 1,
                        minute_enum,
                        second_enum);
                    }
                    else
                    {
                        fm_factory_modify_time(HOUR_0, minute_enum,
                        second_enum);
                    }
                }
                else if(field == MINUTE)
                {
                    if(minute_enum < MINU_59)
                    {
                        fm_factory_modify_time(hour_enum,
                        minute_enum + 1,
                        second_enum);
                    }
                    else
                    {
                        fm_factory_modify_time(hour_enum,
                        MINU_0, second_enum);
                    }
                }
                else if(field == SECOND)
                {
                    if(second_enum < SEC_59)
                    {
                        fm_factory_modify_time(hour_enum,
                        minute_enum,
                        second_enum + 1);
                    }
                    else
                    {
                        fm_factory_modify_time(hour_enum,
                        minute_enum,
                        SEC_0);
                    }
                }
                event_now = EVENT_LCD_REFRESH;
                tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
            }
        break;
        case EVENT_KEY_DOWN:
            if(correct_password)
            {
                /*
                 * En toda esta sección se realiza el calculo para modificar la
                 * fecha y hora del calendario del RTC. Para eso se debe tener
                 * en cuenta la cantidad de días de cada mes, si es un año
                 * bisiesto o no, y que al pasarse del valor máximo o mínimo de
                 * cada parámetro, se vuelva a iniciar desde el valor mínimo o
                 * máximo respectivamente.
                 */
                if(field == DAY)
                {
                    if(month_enum == JANUARY || month_enum == MARCH   ||
                    month_enum == MAY        || month_enum == JULY    ||
                    month_enum == AUGUST     || month_enum == OCTOBER ||
                    month_enum == DECEMBER)
                    {
                        if(day_enum > DAY_1)
                        {
                            fm_factory_modify_date(day_enum - 1, month_enum,
                            year_enum);
                        }
                        else
                        {
                            fm_factory_modify_date(DAY_31, month_enum,
                            year_enum);
                        }
                    }
                    else if(month_enum == APRIL || month_enum == JUNE ||
                    month_enum == SEPTEMBER     || month_enum == NOVEMBER)
                    {
                        if(day_enum > DAY_1)
                        {
                            fm_factory_modify_date(day_enum - 1, month_enum,
                            year_enum);
                        }
                        else
                        {
                            fm_factory_modify_date(DAY_30, month_enum,
                            year_enum);
                        }
                    }
                    else
                    {
                        if(year_enum % YEAR_4 == 0)
                        {
                            if(day_enum > DAY_1)
                            {
                                fm_factory_modify_date(day_enum - 1, month_enum,
                                year_enum);
                            }
                            else
                            {
                                fm_factory_modify_date(DAY_29, month_enum,
                                year_enum);
                            }
                        }
                        else
                        {
                            if(day_enum > DAY_1)
                            {
                                fm_factory_modify_date(day_enum - 1, month_enum,
                                year_enum);
                            }
                            else
                            {
                                fm_factory_modify_date(DAY_28, month_enum,
                                year_enum);
                            }
                        }
                    }
                }
                else if(field == MONTH)
                {
                    if(month_enum > JANUARY)
                    {
                        if((month_enum - 1 == APRIL ||
                        month_enum - 1 == JUNE      ||
                        month_enum - 1 == SEPTEMBER ||
                        month_enum - 1 == NOVEMBER) && (day_enum > DAY_30))
                        {
                            day_enum = DAY_30;
                        }
                        else if((month_enum - 1 == FEBRUARY) &&
                        (year_enum % YEAR_4 == 0) &&
                        (day_enum > DAY_29))
                        {
                            day_enum = DAY_29;
                        }
                        else if((month_enum - 1 == FEBRUARY) &&
                        (year_enum % YEAR_4 != 0) &&
                        (day_enum > DAY_28))
                        {
                            day_enum = DAY_28;
                        }
                        fm_factory_modify_date(day_enum, month_enum - 1,
                        year_enum);
                    }
                    else
                    {
                        fm_factory_modify_date(day_enum, DECEMBER, year_enum);
                    }
                }
                else if(field == YEAR)
                {
                    if(year_enum > YEAR_0)
                    {
                        if(((year_enum - 1) % YEAR_4 != YEAR_0) &&
                        (month_enum == FEBRUARY) &&
                        (day_enum > DAY_28))
                        {
                            day_enum = DAY_28;
                        }
                        fm_factory_modify_date(day_enum, month_enum,
                        year_enum - 1);
                    }
                    else
                    {
                        fm_factory_modify_date(day_enum, month_enum, YEAR_99);
                    }
                }
                else if(field == HOUR)
                {
                    if(hour_enum > HOUR_0)
                    {
                        fm_factory_modify_time(hour_enum - 1, minute_enum,
                        second_enum);
                    }
                    else
                    {
                        fm_factory_modify_time(HOUR_23, minute_enum,
                        second_enum);
                    }
                }
                else if(field == MINUTE)
                {
                    if(minute_enum > MINU_0)
                    {
                        fm_factory_modify_time(hour_enum, minute_enum - 1,
                        second_enum);
                    }
                    else
                    {
                        fm_factory_modify_time(hour_enum, MINU_59, second_enum);
                    }
                }
                else if(field == SECOND)
                {
                    if(second_enum > SEC_0)
                    {
                        fm_factory_modify_time(hour_enum, minute_enum,
                        second_enum - 1);
                    }
                    else
                    {
                        fm_factory_modify_time(hour_enum, minute_enum, SEC_59);
                    }
                }
                event_now = EVENT_LCD_REFRESH;
                tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
            }
        break;
        case EVENT_KEY_ENTER:
            if(correct_password)
            {
                /*
                 * Se cambia qué parámetro se va a modificar según cual estaba
                 * seleccionado para modificarse actualmente.
                 */
                if(field == DAY)
                {
                    field = MONTH;
                }
                else if(field == MONTH)
                {
                    field = YEAR;
                }
                else if(field == YEAR)
                {
                    field = HOUR;
                }
                else if(field == HOUR)
                {
                    field = MINUTE;
                }
                else if(field == MINUTE)
                {
                    field = SECOND;
                }
                else if(field == SECOND)
                {
                    field = DAY;
                }
                event_now = EVENT_LCD_REFRESH;
                tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
            }
        break;
        case EVENT_KEY_ESC:
            new_exit = 1;
            ret_menu = (ptr_ret_menu_t) fm_menu_show_init;
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_LCD_REFRESH:
        break;
        default:
        break;
    }

    previous_event = event_id;
    #ifdef FM_DEBUG_MENU
        char msg_buffer[] = "Configurar fecha y hora\n";
        fm_debug_msg_uart((uint8_t*) msg_buffer, sizeof(msg_buffer));
    #endif

    if (new_exit == 1)
    {
        if(correct_password)
        {
            /*
             * Es totalmente necesario que cuando se complete las estructuras
             * del calendario del RTC, al intentar escribir la fecha, se
             * inicialicen todas las variables de dicha estructura (.Date,
             * .Month, .Year, .Weekday) a pesar de que alguna sea irrelevante
             * (como en este caso lo es .WeekDay), ya que de lo contrario, se
             * deja a la suerte la escritura de los registros del periférico y
             * puede que al intentar leer nuevamente el calendario, los datos se
             * encuentren corrompidos y se lea cualquier cosa. Para mas info de
             * esto ver el link siguiente:
             *
             * http://www.efton.sk/STM32/gotcha/g113.html
             */
            date_final.Date = day_enum;
            date_final.Month = month_enum;
            date_final.Year = year_enum;
            date_final.WeekDay = RTC_WEEKDAY_FRIDAY;

            HAL_RTC_SetDate(&hrtc, &date_final, RTC_FORMAT_BIN);

            /*
             * Idem que lo de la fecha escrito arriba, pero para la hora, y en
             * general siempre que se use la HAL completando estructuras.
             */
            time_final.Hours = hour_enum;
            time_final.Minutes = minute_enum;
            time_final.Seconds = second_enum;
            time_final.SecondFraction = 0;
            time_final.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
            time_final.StoreOperation = RTC_STOREOPERATION_RESET;
            time_final.SubSeconds = 0;
            time_final.TimeFormat = RTC_HOURFORMAT12_AM;

            HAL_RTC_SetTime(&hrtc, &time_final, RTC_FORMAT_BIN);

            correct_password = 0;
        }
        in_configuration = 0;
        new_entry = 1;
        new_exit = 0;
    }

    return (ret_menu);
}

/*
 * @brief Función que imprime el menú de configuración de factor de expansión.
 * @param  Evento de presión de botones o refresh.
 * @retval Puntero al retorno de la función.
 */
ptr_ret_menu_t fm_menu_config_expansion(fm_event_t event_id)
{
    static uint8_t new_entry = 1;
    static uint8_t new_exit = 0;

    ptr_ret_menu_t ret_menu = (ptr_ret_menu_t) fm_menu_config_expansion;
    fm_event_t event_now;

    if (new_entry == 1)
    {
        fm_lcd_clear();
        new_entry = 0;
    }

    lcd_set_symbol(BATTERY, 0x00);
    fm_lcd_refresh();

    switch (event_id)
    {
        case EVENT_KEY_UP:
        break;
        case EVENT_KEY_DOWN:
        break;
        case EVENT_KEY_ENTER:
        break;
        case EVENT_KEY_ESC:
            new_exit = 1;
            ret_menu = (ptr_ret_menu_t) fm_menu_show_version;
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_LCD_REFRESH:
        break;
        default:
        break;
    }

    previous_event = event_id;
    #ifdef FM_DEBUG_MENU
        char msg_buffer[] = "Configurar factor de expansion\n";
        fm_debug_msg_uart((uint8_t*) msg_buffer, sizeof(msg_buffer));
    #endif

    if (new_exit == 1)
    {
        new_entry = 1;
        new_exit = 0;
    }

    return (ret_menu);
}

ptr_ret_menu_t fm_menu_config_k_lin_1(fm_event_t event_id)
{
    static uint8_t new_entry = 1;
    static uint8_t new_exit = 0;
    static sel_digit_k_lin_t digit_lin_modify = DIG_LIN_0;

    ptr_ret_menu_t ret_menu = (ptr_ret_menu_t) fm_menu_config_k_lin_1;
    fm_event_t event_now;

    if (new_entry == 1)
    {
        fm_lcd_clear();
        new_entry = 0;
    }

    fm_lcd_k_lin(K_LIN_1, event_id, digit_lin_modify);
    fm_lcd_refresh();

    switch (event_id)
    {
        case EVENT_KEY_UP:
            if (correct_password)
            {
                fm_factory_modify_k_lin_add(digit_lin_modify, K_LIN_1);
            }
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_KEY_DOWN:
            if (correct_password)
            {
                fm_factory_modify_k_lin_subs(digit_lin_modify, K_LIN_1);
            }
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_KEY_ENTER:
            if (correct_password)
            {
                if(digit_lin_modify < DIG_LIN_11)
                {
                    digit_lin_modify++;
                }
                else
                {
                    digit_lin_modify = DIG_LIN_0;
                }
            }
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_KEY_ESC:
            new_exit = 1;
            ret_menu = (ptr_ret_menu_t) fm_menu_config_k_lin_2;
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_LCD_REFRESH:
        break;
        default:
        break;
    }

    previous_event = event_id;
    #ifdef FM_DEBUG_MENU
        char msg_buffer[] = "Configurar parametro K_lin_1\n";
        fm_debug_msg_uart((uint8_t*) msg_buffer, sizeof(msg_buffer));
    #endif

    if (new_exit == 1)
    {
        digit_lin_modify = DIG_LIN_0;
        new_entry = 1;
        new_exit = 0;
    }

    return (ret_menu);
}

ptr_ret_menu_t fm_menu_config_k_lin_2(fm_event_t event_id)
{
    static uint8_t new_entry = 1;
    static uint8_t new_exit = 0;
    static sel_digit_k_lin_t digit_lin_modify = DIG_LIN_0;

    ptr_ret_menu_t ret_menu = (ptr_ret_menu_t) fm_menu_config_k_lin_2;
    fm_event_t event_now;

    if (new_entry == 1)
    {
        fm_lcd_clear();
        new_entry = 0;
    }

    fm_lcd_k_lin(K_LIN_2, event_id, digit_lin_modify);
    fm_lcd_refresh();

    switch (event_id)
    {
        case EVENT_KEY_UP:
            if (correct_password)
            {
                fm_factory_modify_k_lin_add(digit_lin_modify, K_LIN_2);
            }
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_KEY_DOWN:
            if (correct_password)
            {
                fm_factory_modify_k_lin_subs(digit_lin_modify, K_LIN_2);
            }
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_KEY_ENTER:
            if (correct_password)
            {
                if(digit_lin_modify < DIG_LIN_11)
                {
                    digit_lin_modify++;
                }
                else
                {
                    digit_lin_modify = DIG_LIN_0;
                }
            }
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_KEY_ESC:
            new_exit = 1;
            ret_menu = (ptr_ret_menu_t) fm_menu_config_k_lin_3;
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_LCD_REFRESH:
        break;
        default:
        break;
    }

    previous_event = event_id;
    #ifdef FM_DEBUG_MENU
        char msg_buffer[] = "Configurar parametro K_lin_2\n";
        fm_debug_msg_uart((uint8_t*) msg_buffer, sizeof(msg_buffer));
    #endif

    if (new_exit == 1)
    {
        digit_lin_modify = DIG_LIN_0;
        new_entry = 1;
        new_exit = 0;
    }

    return (ret_menu);
}

ptr_ret_menu_t fm_menu_config_k_lin_3(fm_event_t event_id)
{
    static uint8_t new_entry = 1;
    static uint8_t new_exit = 0;
    static sel_digit_k_lin_t digit_lin_modify = DIG_LIN_0;

    ptr_ret_menu_t ret_menu = (ptr_ret_menu_t) fm_menu_config_k_lin_3;
    fm_event_t event_now;

    if (new_entry == 1)
    {
        fm_lcd_clear();
        new_entry = 0;
    }

    fm_lcd_k_lin(K_LIN_3, event_id, digit_lin_modify);
    fm_lcd_refresh();

    switch (event_id)
    {
        case EVENT_KEY_UP:
            if (correct_password)
            {
                fm_factory_modify_k_lin_add(digit_lin_modify, K_LIN_3);
            }
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_KEY_DOWN:
            if (correct_password)
            {
                fm_factory_modify_k_lin_subs(digit_lin_modify, K_LIN_3);
            }
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_KEY_ENTER:
            if (correct_password)
            {
                if(digit_lin_modify < DIG_LIN_11)
                {
                    digit_lin_modify++;
                }
                else
                {
                    digit_lin_modify = DIG_LIN_0;
                }
            }
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_KEY_ESC:
            new_exit = 1;
            ret_menu = (ptr_ret_menu_t) fm_menu_config_k_lin_4;
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_LCD_REFRESH:
        break;
        default:
        break;
    }

    previous_event = event_id;
    #ifdef FM_DEBUG_MENU
        char msg_buffer[] = "Configurar parametro K_lin_3\n";
        fm_debug_msg_uart((uint8_t*) msg_buffer, sizeof(msg_buffer));
    #endif

    if (new_exit == 1)
    {
        digit_lin_modify = DIG_LIN_0;
        new_entry = 1;
        new_exit = 0;
    }

    return (ret_menu);
}

ptr_ret_menu_t fm_menu_config_k_lin_4(fm_event_t event_id)
{
    static uint8_t new_entry = 1;
    static uint8_t new_exit = 0;
    static sel_digit_k_lin_t digit_lin_modify = DIG_LIN_0;

    ptr_ret_menu_t ret_menu = (ptr_ret_menu_t) fm_menu_config_k_lin_4;
    fm_event_t event_now;

    if (new_entry == 1)
    {
        fm_lcd_clear();
        new_entry = 0;
    }

    fm_lcd_k_lin(K_LIN_4, event_id, digit_lin_modify);
    fm_lcd_refresh();

    switch (event_id)
    {
        case EVENT_KEY_UP:
            if (correct_password)
            {
                fm_factory_modify_k_lin_add(digit_lin_modify, K_LIN_4);
            }
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_KEY_DOWN:
            if (correct_password)
            {
                fm_factory_modify_k_lin_subs(digit_lin_modify, K_LIN_4);
            }
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_KEY_ENTER:
            if (correct_password)
            {
                if(digit_lin_modify < DIG_LIN_11)
                {
                    digit_lin_modify++;
                }
                else
                {
                    digit_lin_modify = DIG_LIN_0;
                }
            }
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_KEY_ESC:
            new_exit = 1;
            ret_menu = (ptr_ret_menu_t) fm_menu_config_k_lin_5;
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_LCD_REFRESH:
        break;
        default:
        break;
    }

    previous_event = event_id;
    #ifdef FM_DEBUG_MENU
        char msg_buffer[] = "Configurar parametro K_lin_4\n";
        fm_debug_msg_uart((uint8_t*) msg_buffer, sizeof(msg_buffer));
    #endif

    if (new_exit == 1)
    {
        digit_lin_modify = DIG_LIN_0;
        new_entry = 1;
        new_exit = 0;
    }

    return (ret_menu);
}

ptr_ret_menu_t fm_menu_config_k_lin_5(fm_event_t event_id)
{
    static uint8_t new_entry = 1;
    static uint8_t new_exit = 0;
    static sel_digit_k_lin_t digit_lin_modify = DIG_LIN_0;

    ptr_ret_menu_t ret_menu = (ptr_ret_menu_t) fm_menu_config_k_lin_5;
    fm_event_t event_now;

    if (new_entry == 1)
    {
        fm_lcd_clear();
        new_entry = 0;
    }

    fm_lcd_k_lin(K_LIN_5, event_id, digit_lin_modify);
    fm_lcd_refresh();

    switch (event_id)
    {
        case EVENT_KEY_UP:
            if (correct_password)
            {
                fm_factory_modify_k_lin_add(digit_lin_modify, K_LIN_5);
            }
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_KEY_DOWN:
            if (correct_password)
            {
                fm_factory_modify_k_lin_subs(digit_lin_modify, K_LIN_5);
            }
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_KEY_ENTER:
            if (correct_password)
            {
                if(digit_lin_modify < DIG_LIN_11)
                {
                    digit_lin_modify++;
                }
                else
                {
                    digit_lin_modify = DIG_LIN_0;
                }
            }
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_KEY_ESC:
            new_exit = 1;
            ret_menu = (ptr_ret_menu_t) fm_menu_config_units_vol;
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_LCD_REFRESH:
        break;
        default:
        break;
    }

    previous_event = event_id;
    #ifdef FM_DEBUG_MENU
        char msg_buffer[] = "Configurar parametro K_lin_5\n";
        fm_debug_msg_uart((uint8_t*) msg_buffer, sizeof(msg_buffer));
    #endif

    if (new_exit == 1)
    {
        digit_lin_modify = DIG_LIN_0;
        new_entry = 1;
        new_exit = 0;
    }

    return (ret_menu);
}

/*
 * @brief Función que imprime el menú de configuración del factor de calibración
 * K.
 * @param  Evento de presión de botones o refresh.
 * @retval Puntero al retorno de la función.
 */
ptr_ret_menu_t fm_menu_config_k_param(fm_event_t event_id)
{
    static uint8_t new_entry = 1;
    static uint8_t new_exit = 0;
    static sel_digit_t digit_modify = DIG_0;

    ptr_ret_menu_t ret_menu = (ptr_ret_menu_t) fm_menu_config_k_param;
    fm_event_t event_now;

    if (new_entry == 1)
    {
        fm_lcd_clear();
        new_entry = 0;
    }

    fm_lcd_k_factor(event_id, digit_modify);
    fm_lcd_refresh();

    switch (event_id)
    {
        case EVENT_KEY_UP:
            if (correct_password)
            {
                fm_factory_modify_k_factor_add(digit_modify);
            }
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_KEY_DOWN:
            if (correct_password)
            {
                fm_factory_modify_k_factor_subs(digit_modify);
            }
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_KEY_ENTER:
            if (correct_password)
            {
                if (digit_modify == DIG_0)
                {
                    digit_modify = DIG_1;
                }
                else if (digit_modify == DIG_1)
                {
                    digit_modify = DIG_2;
                }
                else if (digit_modify == DIG_2)
                {
                    digit_modify = DIG_3;
                }
                else if (digit_modify == DIG_3)
                {
                    digit_modify = DIG_4;
                }
                else if (digit_modify == DIG_4)
                {
                    digit_modify = DIG_5;
                }
                else if (digit_modify == DIG_5)
                {
                    digit_modify = DIG_6;
                }
                else if (digit_modify == DIG_6)
                {
                    digit_modify = DIG_7;
                }
                else if (digit_modify == DIG_7)
                {
                    digit_modify = DIG_0;
                }
            }
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_KEY_ESC:
            new_exit = 1;
            ret_menu = (ptr_ret_menu_t) fm_menu_config_k_lin_1;
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_LCD_REFRESH:
        break;
        default:
        break;
    }

    previous_event = event_id;
    #ifdef FM_DEBUG_MENU
        char msg_buffer[] = "Configurar parametro K\n";
        fm_debug_msg_uart((uint8_t*) msg_buffer, sizeof(msg_buffer));
    #endif

    if (new_exit == 1)
    {
        digit_modify = DIG_0;
        new_entry = 1;
        new_exit = 0;
    }

    return (ret_menu);
}

/*
 * @brief Función que imprime el menú de configuración del factor de calibración
 * Ko.
 * @param  Evento de presión de botones o refresh.
 * @retval Puntero al retorno de la función.
 */
ptr_ret_menu_t fm_menu_config_ko_param(fm_event_t event_id)
{
    static uint8_t new_entry = 1;
    static uint8_t new_exit = 0;

    ptr_ret_menu_t ret_menu = (ptr_ret_menu_t) fm_menu_config_ko_param;
    fm_event_t event_now;

    if (new_entry == 1)
    {
        fm_lcd_clear();
        new_entry = 0;
    }

    lcd_set_symbol(KO, 0x00);
    fm_lcd_refresh();

    switch (event_id)
    {
        case EVENT_KEY_UP:
        break;
        case EVENT_KEY_DOWN:
        break;
        case EVENT_KEY_ENTER:
        break;
        case EVENT_KEY_ESC:
            new_exit = 1;
            ret_menu = (ptr_ret_menu_t) fm_menu_config_units_vol;
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_LCD_REFRESH:
        break;
        default:
        break;
    }

    previous_event = event_id;
    #ifdef FM_DEBUG_MENU
        char msg_buffer[] = "Configurar parametro Ko\n";
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
 * @brief Función que imprime el menú que permite introducir la contraseña del
 * caudalímetro. Posee un algoritmo que levanta una flag si la contraseña,
 * compuesta por las 4 teclas del teclado mecánico, es la misma que la
 * almacenada en memoria (DOWN -> UP -> UP -> ENTER).
 * @param  Evento de presión de botones o refresh.
 * @retval Puntero al retorno de la función.
 */
ptr_ret_menu_t fm_menu_config_pass(fm_event_t event_id)
{
    static uint8_t new_entry = 1;
    static uint8_t new_exit = 0;

    /*
     * Arreglo estático que almacena la contraseña que ingresa el usuario.
     * Inicialmente está relleno con ceros.
     */
    static uint8_t password[PASSWORD_LENGTH] =
    {
        0,
        0,
        0,
        0
    };
    static uint8_t password_index = 0;

    ptr_ret_menu_t ret_menu = (ptr_ret_menu_t) fm_menu_config_pass;
    fm_event_t event_now;

    if (new_entry == 1)
    {
        in_configuration = 1;
        correct_password = 0;
        fm_lcd_clear();
        new_entry = 0;
    }

    lcd_set_symbol(PASS, 0x00);

    /*
     * Cada vez que se introduce un dígito de la contraseña, aparece un 8 nuevo
     * en la pantalla, hasta 3 veces (luego de la cuarta vez, se pasa al menú
     * de configuración del factor K).
     */
    if (password_index == 1)
    {
        lcd_set_symbol(PASS1, 0x00);
    }
    else if (password_index == 2)
    {
        lcd_set_symbol(PASS2, 0x00);
    }
    else if (password_index == PASSWORD_LENGTH - 1)
    {
        lcd_set_symbol(PASS3, 0x00);
    }
    fm_lcd_refresh();

    switch (event_id)
    {
        case EVENT_KEY_UP:
            password[password_index] = 1;

            if (password_index < PASSWORD_LENGTH - 1)
            {
                event_now = EVENT_LCD_REFRESH;
                tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
                password_index++;
            }
            else
            {
                new_exit = 1;
                ret_menu = (ptr_ret_menu_t) fm_menu_config_k_param;
                event_now = EVENT_LCD_REFRESH;
                tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
            }
        break;
        case EVENT_KEY_DOWN:
            password[password_index] = 2;

            if (password_index < PASSWORD_LENGTH - 1)
            {
                event_now = EVENT_LCD_REFRESH;
                tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
                password_index++;
            }
            else
            {
                new_exit = 1;
                ret_menu = (ptr_ret_menu_t) fm_menu_config_k_param;
                event_now = EVENT_LCD_REFRESH;
                tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
            }
        break;
        case EVENT_KEY_ENTER:
            password[password_index] = 3; // @suppress("Avoid magic numbers")

            if (password_index < PASSWORD_LENGTH - 1)
            {
                event_now = EVENT_LCD_REFRESH;
                tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
                password_index++;
            }
            else
            {
                new_exit = 1;
                ret_menu = (ptr_ret_menu_t) fm_menu_config_k_param;
                event_now = EVENT_LCD_REFRESH;
                tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
            }
        break;
        case EVENT_KEY_ESC:
            password[password_index] = 4; // @suppress("Avoid magic numbers")

            if (password_index < PASSWORD_LENGTH - 1)
            {
                event_now = EVENT_LCD_REFRESH;
                tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
                password_index++;
            }
            else
            {
                new_exit = 1;
                ret_menu = (ptr_ret_menu_t) fm_menu_config_k_param;
                event_now = EVENT_LCD_REFRESH;
                tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
            }
        break;
        case EVENT_LCD_REFRESH:
        break;
        default:
        break;
    }

    previous_event = event_id;
    #ifdef FM_DEBUG_MENU
        char msg_buffer[] = "Password\n";
        fm_debug_msg_uart((uint8_t*) msg_buffer, sizeof(msg_buffer));
    #endif

    if (new_exit == 1 && password_index >= PASSWORD_LENGTH - 1)
    {
        /*
         * Si la contraseña ingresada es correcta, se activa una flag global que
         * permite modificar los parámetros de los menús de configuración.
         */
        if (password[0] == 2 && password[1] == 1 && password[2] == 1
        && password[PASSWORD_LENGTH - 1] == 3)
        {
            correct_password = 1;
        }
        fm_factory_modify_date(fm_calendar_get_day(),
        fm_calendar_get_month(), fm_calendar_get_year());

        fm_factory_modify_time(fm_calendar_get_hour(),
        fm_calendar_get_minute(), fm_calendar_get_second());
        /*
         * Reinicio el arreglo de la contraseña ingresada.
         */
        password_index = 0;
        password[0] = 0;
        password[1] = 0;
        password[2] = 0;
        password[PASSWORD_LENGTH - 1] = 0;
        new_entry = 1;
        new_exit = 0;
    }

    return (ret_menu);
}

/*
 * @brief Función que imprime el menú de configuración del factor de Span.
 * @param  Evento de presión de botones o refresh.
 * @retval Puntero al retorno de la función.
 */
ptr_ret_menu_t fm_menu_config_span(fm_event_t event_id)
{
    static uint8_t new_entry = 1;
    static uint8_t new_exit = 0;

    ptr_ret_menu_t ret_menu = (ptr_ret_menu_t) fm_menu_config_span;
    fm_event_t event_now;

    if (new_entry == 1)
    {
        fm_lcd_clear();
        new_entry = 0;
    }

    lcd_set_symbol(E, 0x00);
    fm_lcd_refresh();

    switch (event_id)
    {
        case EVENT_KEY_UP:
        break;
        case EVENT_KEY_DOWN:
        break;
        case EVENT_KEY_ENTER:
        break;
        case EVENT_KEY_ESC:
            new_exit = 1;
            ret_menu = (ptr_ret_menu_t) fm_menu_config_expansion;
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_LCD_REFRESH:
        break;
        default:
        break;
    }

    previous_event = event_id;
    #ifdef FM_DEBUG_MENU
        char msg_buffer[] = "Configurar Span\n";
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
 * @brief Función que imprime el menú de configuración tanto de las unidades de
 * volumen y tiempo, como de la resolución de las medidas.
 * @param  Evento de presión de botones o refresh.
 * @retval Puntero al retorno de la función.
 */
ptr_ret_menu_t fm_menu_config_units_tim(fm_event_t event_id)
{
    static uint8_t new_entry = 1;
    static uint8_t new_exit = 0;

    ptr_ret_menu_t ret_menu = (ptr_ret_menu_t) fm_menu_config_units_tim;
    fm_event_t event_now;

    if (new_entry == 1)
    {
        fm_lcd_clear();
        new_entry = 0;
    }

    fm_lcd_units_tim(event_id);
    fm_lcd_refresh();

    switch (event_id)
    {
        case EVENT_KEY_UP:
            if (correct_password)
            {
                if (fm_factory_get_rate().unit_time == H)
                {
                    fm_factory_modify_time_units(D);
                }
                else if (fm_factory_get_rate().unit_time == D)
                {
                    fm_factory_modify_time_units(S);
                }
                else if (fm_factory_get_rate().unit_time == S)
                {
                    fm_factory_modify_time_units(M);
                }
                else if (fm_factory_get_rate().unit_time == M)
                {
                    fm_factory_modify_time_units(H);
                }
                fm_lcd_clear();
            }
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_KEY_DOWN:
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_KEY_ENTER:
            if (correct_password)
            {
                if (fm_factory_get_units_tim().res == RES_0)
                {
                    fm_factory_modify_res_rate(RES_1, RES_1);
                }
                else if (fm_factory_get_units_tim().res == RES_1)
                {
                    fm_factory_modify_res_rate(RES_2, RES_2);
                }
                else if (fm_factory_get_units_tim().res == RES_2)
                {
                    fm_factory_modify_res_rate(RES_3, RES_3);
                }
                else if (fm_factory_get_units_tim().res == RES_3)
                {
                    fm_factory_modify_res_rate(RES_0, RES_0);
                }
            }
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_KEY_ESC:
            new_exit = 1;
            ret_menu = (ptr_ret_menu_t) fm_menu_config_date_hour;
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_LCD_REFRESH:
        break;
        default:
        break;
    }

    previous_event = event_id;
    #ifdef FM_DEBUG_MENU
        char msg_buffer[] = "Configurar unidades de tiempo y resolucion\n";
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
 * @brief Función que imprime el menú de configuración tanto de las unidades de
 * volumen y tiempo, como de la resolución de las medidas.
 * @param  Evento de presión de botones o refresh.
 * @retval Puntero al retorno de la función.
 */
ptr_ret_menu_t fm_menu_config_units_vol(fm_event_t event_id)
{
    static uint8_t new_entry = 1;
    static uint8_t new_exit = 0;

    ptr_ret_menu_t ret_menu = (ptr_ret_menu_t) fm_menu_config_units_vol;
    fm_event_t event_now;

    if (new_entry == 1)
    {
        fm_lcd_clear();
        new_entry = 0;
    }

    fm_lcd_units_vol(event_id);
    fm_lcd_refresh();

    switch (event_id)
    {
        case EVENT_KEY_UP:
            if (correct_password)
            {
                if (fm_factory_get_acm().unit_volume == LT)
                {
                    fm_factory_modify_volume_units(M3);
                }
                else if (fm_factory_get_acm().unit_volume == M3)
                {
                    fm_factory_modify_volume_units(KG);
                }
                else if (fm_factory_get_acm().unit_volume == KG)
                {
                    fm_factory_modify_volume_units(GL);
                }
                else if (fm_factory_get_acm().unit_volume == GL)
                {
                    fm_factory_modify_volume_units(BR);
                }
                else if (fm_factory_get_acm().unit_volume == BR)
                {
                    fm_factory_modify_volume_units(NOTHING);
                }
                else if (fm_factory_get_acm().unit_volume == NOTHING)
                {
                    fm_factory_modify_volume_units(LT);
                }
                fm_lcd_clear();
            }
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_KEY_DOWN:
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_KEY_ENTER:
            if (correct_password)
            {
                if (fm_factory_get_units_vol().res == RES_0)
                {
                    fm_factory_modify_res_acm_ttl(RES_1, RES_1, RES_1);
                }
                else if (fm_factory_get_units_vol().res == RES_1)
                {
                    fm_factory_modify_res_acm_ttl(RES_2, RES_2, RES_2);
                }
                else if (fm_factory_get_units_vol().res == RES_2)
                {
                    fm_factory_modify_res_acm_ttl(RES_3, RES_3, RES_3);
                }
                else if (fm_factory_get_units_vol().res == RES_3)
                {
                    fm_factory_modify_res_acm_ttl(RES_0, RES_0, RES_0);
                }
            }
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_KEY_ESC:
            new_exit = 1;
            ret_menu = (ptr_ret_menu_t) fm_menu_config_units_tim;
            event_now = EVENT_LCD_REFRESH;
            tx_queue_send(&event_queue_ptr, &event_now, TX_NO_WAIT);
        break;
        case EVENT_LCD_REFRESH:
        break;
        default:
        break;
    }

    previous_event = event_id;
    #ifdef FM_DEBUG_MENU
        char msg_buffer[] = "Configurar unidades de volumen y resolucion\n";
        fm_debug_msg_uart((uint8_t*) msg_buffer, sizeof(msg_buffer));
    #endif

    if (new_exit == 1)
    {
        new_entry = 1;
        new_exit = 0;
    }

    return (ret_menu);
}
// Interrupts

/*** end of file ***/

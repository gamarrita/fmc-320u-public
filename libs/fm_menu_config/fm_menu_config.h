/* @file fm_menu_config.h
 *
 * @brief A description of the module’s purpose.
 *
 * COPYRIGHT NOTE: (c) 2023 FLOWMEET. All right reserved.
 */

#ifndef     FM_MENU_CONFIG_H_
#define     FM_MENU_CONFIG_H_

// includes

#include "main.h"
#include "../fm_lcd/fm_lcd.h"
#include "../fm_event/fm_event.h"
#include "../fm_menu_user/fm_menu_user.h"

// Macros, defines, microcontroller pins (dhs).

// Typedef.

typedef void* (*ptr_ret_menu_t)(fm_event_t);            // Un puntero a funcion
typedef ptr_ret_menu_t (*ptr_fun_menu_t)(fm_event_t);

// Defines.

// Function prototypes

ptr_ret_menu_t fm_menu_config_date_hour(fm_event_t);
ptr_ret_menu_t fm_menu_config_expansion(fm_event_t);
ptr_ret_menu_t fm_menu_config_k_lin_1(fm_event_t);
ptr_ret_menu_t fm_menu_config_k_lin_2(fm_event_t);
ptr_ret_menu_t fm_menu_config_k_lin_3(fm_event_t);
ptr_ret_menu_t fm_menu_config_k_lin_4(fm_event_t);
ptr_ret_menu_t fm_menu_config_k_lin_5(fm_event_t);
ptr_ret_menu_t fm_menu_config_k_param(fm_event_t);
ptr_ret_menu_t fm_menu_config_ko_param(fm_event_t);
ptr_ret_menu_t fm_menu_config_pass(fm_event_t);
ptr_ret_menu_t fm_menu_config_span(fm_event_t);
ptr_ret_menu_t fm_menu_config_units_tim(fm_event_t);
ptr_ret_menu_t fm_menu_config_units_vol(fm_event_t);

#endif /* MENU_CONFIG_H */

/*** end of file ***/

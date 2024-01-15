/* @file fm_calendar.h
 *
 * Este modulo se encarga de recibir los datos de la fecha y hora actuales,
 * provenientes del periférico RTC, y de formatearlos para que puedan ser
 * correctamente impresos en la pantalla LCD.
 *
 * COPYRIGHT NOTE: (c) 2023 FLOWMEET. All right reserved.
 */

#ifndef     FM_CALENDAR_H_
#define     FM_CALENDAR_H_

// includes
#include "main.h"

// Macros, defines, microcontroller pins (dhs).

// Typedef.

// Defines.

// Function prototypes

void fm_calendar_get();
int fm_calendar_get_hour();
int fm_calendar_get_minute();
int fm_calendar_get_second();
int fm_calendar_get_day();
int fm_calendar_get_month();
int fm_calendar_get_year();
void fm_calendar_format_time();
void fm_calendar_format_date();

#endif /* FM_CALENDAR_H */

/*** end of file ***/

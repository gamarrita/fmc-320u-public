/* @file fm_computer.h
 *
 * @brief Este es el módulo que se encarga de obtener datos de los sensores,
 * relojes, y registros incorporados al hardware del caudalímetro, vease,
 * sensores de temperatura, de caudal, de volumen, y registros que indiquen
 * por ejemplo la versión actual del caudalímetro.
 *
 * IMPORTANTE: Actualmente esta información no se obtiene de dichos sensores o
 * registros, sinó que se definen valores constantes para las pruebas
 * iniciales (sand).
 * 
 * COPYRIGHT NOTE: (c) 2023 FLOWMEET. All right reserved.
 */

#ifndef     FM_COMPUTER_H_
#define     FM_COMPUTER_H_

// includes

#include "main.h"
#include "string.h"

// Macros, defines, microcontroller pins (dhs).

// Typedef.

/*
 * Todos los símbolos que es posible imprimir en la pantalla LCD.
 */
typedef enum
{
    BATTERY,
    POWER,
    RATE,
    E,
    BATCH,
    TTL,
    ACM,
    BACKSLASH,
    POINT,
    VE,
    PASS,
    K,
    KO,
    PASS1,
    PASS2,
    PASS3,
    F,
    LIN_1,
    LIN_2,
    LIN_3,
    LIN_4,
    LIN_5
} symbols_t;

typedef enum
{
    LT, M3, KG, GL, BR, CELSIUS, NOTHING
} fmc_unit_volume_t;

typedef enum
{
    H, D, S, M, UNIT_TIME_END
} fmc_unit_time_t;

// Typedef.

typedef struct
{
    uint32_t num;
    uint8_t res;
} fmc_fp_t;

typedef struct
{
    uint64_t pulse;
    fmc_fp_t volume;
    fmc_fp_t factor; // Factor en pulsos / unidad de volumen
    fmc_unit_volume_t unit_volume;
    fmc_unit_time_t unit_time;
} fmc_totalizer_t;

typedef struct
{
    fmc_fp_t temperature;
    fmc_unit_volume_t unit_volume_temp;
} fmc_temp_t;

typedef struct
{
    int day;
    int month;
    int year;
    int hour;
    int minute;
    int second;
} fmc_date_time_t;

// Defines.

// Function prototypes

fmc_totalizer_t fmc_get_acm();
fmc_date_time_t fmc_get_date_time();
fmc_temp_t fmc_get_stm32_temp();
fmc_totalizer_t fmc_get_rate();
fmc_totalizer_t fmc_get_ttl();
uint32_t fmc_get_version();
void fmc_totalizer_clear_pulse(fmc_totalizer_t *p_totalizer);
fmc_totalizer_t fmc_totalizer_init(fmc_totalizer_t totalizer);
void fmc_totalizer_refresh(fmc_totalizer_t *p_totalizer);

#endif /* FM_COMPUTER_H_ */

/*** end of file ***/

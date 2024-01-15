/* @file fm_computer.c
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
 *
 */

// Includes.
#include "fmc.h"
#include "../fm_factory/fm_factory.h"
#include "../fm_temp_stm32/fm_temp_stm32.h"

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
static const uint32_t g_scalar[] =
{
    1,
    10,
    100,
    1000,
    10000,
    100000,
    1000000
};

// Defines.

#define EXT_TEMP_INIT_VALUE 253

//Debug.

/*
 * To temporally disable a block of code, use preprocessor's conditional
 * compilation features, eg, the following one should be used to increase the
 * the debug output information.
 *
 */
#ifndef NDEBUG
#endif

// Project variables, non-static, at least used in other file.

/*
 * Inicializo variables de la estructura fmc_totalizer_t para trabajar
 * con los parámetros acm, rate y ttl.
 */
fmc_totalizer_t acm;
fmc_totalizer_t rate;
fmc_totalizer_t ttl;
fmc_temp_t int_temperature;
fmc_date_time_t date_time;

// External variables.

// Global variables, statics.

// Private function prototypes.

// Private function bodies.

// Public function bodies.

/*
 * @brief Función que obtiene el valor del caudal acumulado y lo devuelve como
 * parámetro de retorno.
 * @param  None
 * @retval Volumen acumulado como estructura.
 */
fmc_totalizer_t fmc_get_acm()
{
    acm = fm_factory_get_acm();

    acm = fmc_totalizer_init(acm);

    return (acm);
}

/*
 * @brief Función que obtiene el valor de la temperatura interna y la devuelve
 * como parámetro de retorno.
 * @param  None
 * @retval temperatura interna del microcontrolador como una estructura que
 * contiene su valor, resolución y unidad.
 * global.
 */
fmc_temp_t fmc_get_stm32_temp()
{
    int_temperature.temperature.num = fm_temp_stm32_format();
    int_temperature.temperature.res = fm_factory_get_temp().temperature.res;
    int_temperature.unit_volume_temp = fm_factory_get_temp().unit_volume_temp;

    return (int_temperature);
}

/*
 * @brief Función que obtiene el valor del caudal instantaneo 'rate' y lo
 * devuelve como parámetro de retorno.
 * @param  None
 * @retval caudal instantaneo como estructura.
 */
fmc_totalizer_t fmc_get_rate()
{
    rate = fm_factory_get_rate();

    rate = fmc_totalizer_init(rate);

    return (rate);
}

/*
 * @brief Función que obtiene el valor del caudal histórico y lo devuelve como
 * parámetro de retorno.
 * @param  None
 * @retval volumen histórico como estructura.
 */
fmc_totalizer_t fmc_get_ttl()
{
    ttl = fm_factory_get_ttl();

    ttl = fmc_totalizer_init(ttl);

    return (ttl);
}

/*
 * @brief Esta función limpia la cantidad de pulsos leidos por el contador de
 * pulsos.
 * @param  puntero a estructura que contiene datos como la cantidad de pulsos
 * leidos, valor y resolución del caudal/volumen, entre otros.
 * @retval None
 */
void fmc_totalizer_clear_pulse(fmc_totalizer_t *p_totalizer)
{
    p_totalizer->pulse = 0;
    fmc_totalizer_refresh(p_totalizer);
}

/*
 * @brief
 * @param
 * @retval None
 */
fmc_totalizer_t fmc_totalizer_init(fmc_totalizer_t totalizer)
{
    fmc_totalizer_refresh(&totalizer);

    return (totalizer);
}

/*
 * @brief Esta función refresca el volumen obtenido de dividir los pulsos leidos
 * por el factor correspondiente.
 * @param  puntero a estructura que contiene datos como la cantidad de pulsos
 * leidos, valor y resolución del caudal/volumen, entre otros.
 * @retval None
 */
void fmc_totalizer_refresh(fmc_totalizer_t *p_totalizer)
{
    uint64_t result;

    /*
     * result es la cantidad de pulsos almacenados en la estructura p_totalizer.
     */
    result = (uint64_t) p_totalizer->pulse;

    /*
     * Pulsos escalados en el factor y en la resolucion a mostrar.
     */
    result *= g_scalar[p_totalizer->factor.res + p_totalizer->volume.res];

    /*
     * Obtengo el valor numérico del volumen dividiendo los pulsos escalados
     * por el factor.
     */
    result /= p_totalizer->factor.num;

    p_totalizer->volume.num = (uint32_t) result;

}

// Interrupts

/*** end of file ***/

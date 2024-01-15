/* @file fm_factory.c
 *
 * @brief Aquí se tienen los valores de fabrica, se usan al menos en las
 * siguientes situaciones:
 * Al encender el equipo por primera vez, las variables de entorno tomarán estos
 * valores.
 * Si el usuario tiene problemas puede resetear a valores de fabrica.
 *
 * COPYRIGHT NOTE: (c) 2023 FLOWMEET. All right reserved.
 *
 */

// Includes.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fm_factory.h"
#include "../fmc/fmc.h"
#include "../fm_lcd/fm_lcd.h"

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
/*Los valores de fabrica por el momento coinciden con los de configuracion,
 no con los de entorno. En algún momento se puede necesitar separar valor
 de fabrica con los de configuración.
 */
static fmc_totalizer_t ttl_config =
{
    .pulse = 12000,
    .volume.num = 0,
    .volume.res = 2,
    .factor.num = 3000, // pulsos/unidad_volumen.
    .factor.res = 1,
    .unit_volume = LT,
    .unit_time = S,
};

static fmc_totalizer_t acm_config =
{
    .pulse = 6000,
    .volume.num = 0,
    .volume.res = 2,
    .factor.num = 3000, // pulsos/unidad_volumen.
    .factor.res = 1,
    .unit_volume = LT,
    .unit_time = S,
};

static fmc_totalizer_t rate_config =
{
    .pulse = 123500,
    .volume.num = 0,
    .volume.res = 0,
    .factor.num = 125000,
    .factor.res = 3,
    .unit_volume = LT,
    .unit_time = S,
};

static fmc_temp_t temperature_config =
{
    .temperature.num = 20,
    .temperature.res = 0,
    .unit_volume_temp = CELSIUS,
};

static fmc_date_time_t date_time_config =
{
    .day = 17,
    .month = 4,
    .year = 23,
    .hour = 17,
    .minute = 4,
    .second = 20,
};

static fmc_fp_t units_digits_tim =
{
    .num = 0,
    .res = 2,
};

static fmc_fp_t units_digits_vol =
{
    .num = 0,
    .res = 2,
};

static fmc_fp_t frec_lin_1 =
{
    .num = 100,
    .res = 0,
};

static fmc_fp_t frec_lin_2 =
{
    .num = 200,
    .res = 0,
};

static fmc_fp_t frec_lin_3 =
{
    .num = 500,
    .res = 0,
};

static fmc_fp_t frec_lin_4 =
{
    .num = 1000,
    .res = 0,
};

static fmc_fp_t frec_lin_5 =
{
    .num = 1500,
    .res = 0,
};

static fmc_fp_t k_lin_1_config =
{
    .num = 14170,
    .res = 2,
};

static fmc_fp_t k_lin_2_config =
{
    .num = 14170,
    .res = 2,
};

static fmc_fp_t k_lin_3_config =
{
    .num = 14170,
    .res = 2,
};

static fmc_fp_t k_lin_4_config =
{
    .num = 14170,
    .res = 2,
};

static fmc_fp_t k_lin_5_config =
{
    .num = 14170,
    .res = 2,
};

static fmc_fp_t k_factor_config =
{
    .num = 14171,
    .res = 2,
};

static fmc_fp_t time_user =
{
    .num = 93000,
    .res = 0,
};

static fmc_fp_t date_user =
{
    .num = 19042023,
    .res = 0,
};

static fmc_fp_t time_config =
{
    .num = 93000,
    .res = 0,
};

static fmc_fp_t date_config =
{
    .num = 19042023,
    .res = 0,
};

// Defines.

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

sel_value_t k_array[LINE_1_DIGITS]; //Arreglo que almacena al factor K.
sel_value_t k_lin_array[LINE_1_DIGITS]; //Arreglo que almacena los K_lin.
/*
 * Arreglo que modifica las frecuencias a las que se linealiza.
 */
sel_value_t frec_array[LINE_1_DIGITS - 1];

// External variables.

// Global variables, statics.

// Private function prototypes.

// Private function bodies.

// Public function bodies.

/*
 * @brief Función que devuelve el parámetro ACM almacenado en fm_factory.
 * @param None
 * @retval Parámetro ACM como estructura de tipo fmc_totalizer_t.
 */
fmc_totalizer_t fm_factory_get_acm()
{
    return (acm_config);
}

/*
 * @brief Función que devuelve el parámetro TTL almacenado en fm_factory.
 * @param None
 * @retval Parámetro TTL como estructura de tipo fmc_totalizer_t.
 */
fmc_totalizer_t fm_factory_get_ttl()
{
    return (ttl_config);
}

/*
 * @brief Función que devuelve el parámetro RATE almacenado en fm_factory.
 * @param None
 * @retval Parámetro RATE como estructura de tipo fmc_totalizer_t.
 */
fmc_totalizer_t fm_factory_get_rate()
{
    return (rate_config);
}

/*
 * @brief Función que devuelve el la temperatura interna del micro almacenado en
 * fm_factory.
 * @param None
 * @retval Temperatura interna del micro como estructura de tipo fmc_temp_t.
 */
fmc_temp_t fm_factory_get_temp()
{
    return (temperature_config);
}

/*
 * @brief Función que devuelve la configuración de resoluciones almacenado en
 * fm_factory.
 * @param None
 * @retval Configuración de resoluciones como estructura de tipo fmc_fp_t.
 */
fmc_fp_t fm_factory_get_units_tim()
{
    return (units_digits_tim);
}

/*
 * @brief Función que devuelve la configuración de resoluciones almacenado en
 * fm_factory.
 * @param None
 * @retval Configuración de resoluciones como estructura de tipo fmc_fp_t.
 */
fmc_fp_t fm_factory_get_units_vol()
{
    return (units_digits_vol);
}

/*
 * @brief Función que devuelve el factor K almacenado en fm_factory.
 * @param None
 * @retval Factor K.
 */
fmc_fp_t fm_factory_get_k_factor(sel_k k_sel)
{
    static fmc_fp_t k_selected;
    if(k_sel == K_FACTOR)
    {
        k_selected = k_factor_config;
    }
    else if(k_sel == K_LIN_1)
    {
        k_selected = k_lin_1_config;
    }
    else if(k_sel == K_LIN_2)
    {
        k_selected = k_lin_2_config;
    }
    else if(k_sel == K_LIN_3)
    {
        k_selected = k_lin_3_config;
    }
    else if(k_sel == K_LIN_4)
    {
        k_selected = k_lin_4_config;
    }
    else if(k_sel == K_LIN_5)
    {
        k_selected = k_lin_5_config;
    }

    return (k_selected);
}

fmc_fp_t fm_factory_get_frec_lin(sel_k k_sel)
{
    static fmc_fp_t frec_lin_selected;

    if(k_sel == K_LIN_1)
    {
        frec_lin_selected = frec_lin_1;
    }
    else if(k_sel == K_LIN_2)
    {
        frec_lin_selected = frec_lin_2;
    }
    else if(k_sel == K_LIN_3)
    {
        frec_lin_selected = frec_lin_3;
    }
    else if(k_sel == K_LIN_4)
    {
        frec_lin_selected = frec_lin_4;
    }
    else if(k_sel == K_LIN_5)
    {
        frec_lin_selected = frec_lin_5;
    }

    return (frec_lin_selected);
}

/*
 * @brief Función que devuelve la fecha y la hora almacenada en fm_factory, como
 * parámetros individuales.
 * @param None
 * @retval Hora, minutos, segundos, día, mes y año almacenados en
 * date_time_config.
 */
fmc_date_time_t fm_factory_get_date_time()
{
    return(date_time_config);
}

/*
 * @brief Función que devuelve la fecha almacenada en fm_factory, como un
 * parámetro de tipo punto fijo con 0 decimales.
 * @param None
 * @retval Punto fijo que almacena la fecha.
 */
fmc_fp_t fm_factory_get_fp_date()
{
    return(date_user);
}

/*
 * @brief Función que devuelve la hora almacenada en fm_factory, como un
 * parámetro de tipo punto fijo con 0 decimales.
 * @param None
 * @retval Punto fijo que almacena la hora.
 */
fmc_fp_t fm_factory_get_fp_time()
{
    return(time_user);
}

/*
 * @brief Función que devuelve la fecha congelada al entrar al menú de
 * configuración, almacenada en fm_factory, como un parámetro de tipo punto fijo
 * con 0 decimales.
 * @param None
 * @retval Punto fijo que almacena la fecha congelada.
 */
fmc_fp_t fm_factory_get_fp_date_conf()
{
    return(date_config);
}

/*
 * @brief Función que devuelve la hora congelada al entrar al menú de
 * configuración, almacenada en fm_factory, como un parámetro de tipo punto fijo
 * con 0 decimales.
 * @param None
 * @retval Punto fijo que almacena la hora congelada.
 */
fmc_fp_t fm_factory_get_fp_time_conf()
{
    return(time_config);
}

/*
 * @brief Función que permite modificar la fecha almacenada como un punto
 * fijo.
 * @param Fecha a almacenar como un número concatenado.
 * @retval None
 */
void fm_factory_modify_fp_date(int date)
{
    date_user.num = date;
}

/*
 * @brief Función que permite modificar la hora almacenada como un punto
 * fijo.
 * @param Hora a almacenar como un número concatenado.
 * @retval None
 */
void fm_factory_modify_fp_time(int time)
{
    time_user.num = time;
}

/*
 * @brief Función que permite modificar la fecha como parámetros individuales y
 * concatenarlos para que sean introducidos en la fecha congelada de tipo punto
 * fijo.
 * @param día, mes y año leídos del calendario.
 * @retval None
 */
void fm_factory_modify_date(int mod_day, int mod_month, int mod_year)
{
    date_time_config.day = mod_day;
    date_time_config.month = mod_month;
    date_time_config.year = mod_year;

    date_config.num = mod_day * 1000000 + mod_month * 10000 + 2000 + mod_year;
}

/*
 * @brief Función que permite modificar la hora como parámetros individuales y
 * concatenarlos para que sean introducidos en la hora congelada de tipo punto
 * fijo.
 * @param Hora, minutos y segundos leídos del calendario.
 * @retval None
 */
void fm_factory_modify_time(int mod_hour, int mod_minute, int mod_second)
{
    date_time_config.hour = mod_hour;
    date_time_config.minute = mod_minute;
    date_time_config.second = mod_second;

    time_config.num = mod_hour * 10000 + mod_minute * 100 + mod_second;
}

/*
 * @brief Función que suma uno al dígito pasado como parámetro del factor K.
 * @param Digito a modificar del factor K de la enumeración sel_digit_t.
 * @retval None
 */
void fm_factory_modify_k_factor_add(sel_digit_t digit_k)
{
    uint32_t k_new_num = 0;
    fm_factory_separate_k_factor();
    if (k_array[LINE_1_DIGITS - 1 - digit_k] < VAL_9)
    {
        k_array[LINE_1_DIGITS - 1 - digit_k]++;
    }
    else
    {
        k_array[LINE_1_DIGITS - 1 - digit_k] = VAL_0;
    }

    for (int i = 0; i <= LINE_1_DIGITS - 1; i++)
    {
        k_new_num = (k_new_num * 10) + k_array[i];
    }

    k_factor_config.num = k_new_num;
}

/*
 * @brief Función que resta uno al dígito pasado como parámetro del factor K.
 * @param Digito a modificar del factor K de la enumeración sel_digit_t.
 * @retval None
 */
void fm_factory_modify_k_factor_subs(sel_digit_t digit_k)
{
    uint32_t k_new_num = 0;
    fm_factory_separate_k_factor();
    if (k_array[LINE_1_DIGITS - 1 - digit_k] > VAL_0)
    {
        k_array[LINE_1_DIGITS - 1 - digit_k]--;
    }
    else
    {
        k_array[LINE_1_DIGITS - 1 - digit_k] = VAL_9;
    }

    for (int i = 0; i <= LINE_1_DIGITS - 1; i++)
    {
        k_new_num = (k_new_num * 10) + k_array[i];
    }

    k_factor_config.num = k_new_num;
}

void fm_factory_modify_k_lin_add(sel_digit_k_lin_t digit_k_lin, sel_k k_sel)
{
    uint32_t k_lin_new_num = 0;
    uint32_t frec_lin_new_num = 0;

    fm_factory_separate_k_lin_and_frec(k_sel);
    if(digit_k_lin <= DIG_LIN_7)
    {
        if(k_lin_array[LINE_1_DIGITS - 1 - digit_k_lin] < VAL_9)
        {
            k_lin_array[LINE_1_DIGITS - 1 - digit_k_lin]++;
        }
        else
        {
            k_lin_array[LINE_1_DIGITS - 1 - digit_k_lin] = VAL_0;
        }

        for(int i = 0; i <= LINE_1_DIGITS - 1; i++)
        {
            k_lin_new_num = (k_lin_new_num * 10) + k_lin_array[i];
        }

        if(k_sel == K_LIN_1)
        {
            k_lin_1_config.num = k_lin_new_num;
        }
        else if(k_sel == K_LIN_2)
        {
            k_lin_2_config.num = k_lin_new_num;
        }
        else if(k_sel == K_LIN_3)
        {
            k_lin_3_config.num = k_lin_new_num;
        }
        else if(k_sel == K_LIN_4)
        {
            k_lin_4_config.num = k_lin_new_num;
        }
        else if(k_sel == K_LIN_5)
        {
            k_lin_5_config.num = k_lin_new_num;
        }
    }
    else if(digit_k_lin > DIG_LIN_7)
    {
        if(LINE_1_DIGITS - 2 - digit_k_lin + DIG_LIN_8 != DIG_LIN_3)
        {
            if(frec_array[LINE_1_DIGITS - 2 - digit_k_lin + DIG_LIN_8] < VAL_9)
            {
                frec_array[LINE_1_DIGITS - 2 - digit_k_lin + DIG_LIN_8]++;
            }
            else
            {
                frec_array[LINE_1_DIGITS - 2 - digit_k_lin + DIG_LIN_8] = VAL_0;
            }
        }
        else
        {
            if(frec_array[LINE_1_DIGITS - 2 - digit_k_lin + DIG_LIN_8] < VAL_1)
            {
                frec_array[LINE_1_DIGITS - 2 - digit_k_lin + DIG_LIN_8]++;
            }
            else
            {
                frec_array[LINE_1_DIGITS - 2 - digit_k_lin + DIG_LIN_8] = VAL_0;
            }
        }

        for(int j = 0; j <= LINE_1_DIGITS - 2; j++)
        {
            frec_lin_new_num = (frec_lin_new_num * 10) + frec_array[j];
        }

        if(k_sel == K_LIN_1)
        {
            frec_lin_1.num = frec_lin_new_num;
        }
        else if(k_sel == K_LIN_2)
        {
            frec_lin_2.num = frec_lin_new_num;
        }
        else if(k_sel == K_LIN_3)
        {
            frec_lin_3.num = frec_lin_new_num;
        }
        else if(k_sel == K_LIN_4)
        {
            frec_lin_4.num = frec_lin_new_num;
        }
        else if(k_sel == K_LIN_5)
        {
            frec_lin_5.num = frec_lin_new_num;
        }
    }
}

void fm_factory_modify_k_lin_subs(sel_digit_k_lin_t digit_k_lin, sel_k k_sel)
{
    uint32_t k_lin_new_num = 0;
    uint32_t frec_lin_new_num = 0;

    fm_factory_separate_k_lin_and_frec(k_sel);
    if(digit_k_lin <= DIG_LIN_7)
    {
        if(k_lin_array[LINE_1_DIGITS - 1 - digit_k_lin] > VAL_0)
        {
            k_lin_array[LINE_1_DIGITS - 1 - digit_k_lin]--;
        }
        else
        {
            k_lin_array[LINE_1_DIGITS - 1 - digit_k_lin] = VAL_9;
        }

        for(int i = 0; i <= LINE_1_DIGITS - 1; i++)
        {
            k_lin_new_num = (k_lin_new_num * 10) + k_lin_array[i];
        }

        if(k_sel == K_LIN_1)
        {
            k_lin_1_config.num = k_lin_new_num;
        }
        else if(k_sel == K_LIN_2)
        {
            k_lin_2_config.num = k_lin_new_num;
        }
        else if(k_sel == K_LIN_3)
        {
            k_lin_3_config.num = k_lin_new_num;
        }
        else if(k_sel == K_LIN_4)
        {
            k_lin_4_config.num = k_lin_new_num;
        }
        else if(k_sel == K_LIN_5)
        {
            k_lin_5_config.num = k_lin_new_num;
        }
    }
    else if(digit_k_lin > DIG_LIN_7)
    {
        if(LINE_1_DIGITS - 2 - digit_k_lin + DIG_LIN_8 != DIG_LIN_3)
        {
            if(frec_array[LINE_1_DIGITS - 2 - digit_k_lin + DIG_LIN_8] > VAL_0)
            {
                frec_array[LINE_1_DIGITS - 2 - digit_k_lin + DIG_LIN_8]--;
            }
            else
            {
                frec_array[LINE_1_DIGITS - 2 - digit_k_lin + DIG_LIN_8] = VAL_9;
            }
        }
        else
        {
            if(frec_array[LINE_1_DIGITS - 2 - digit_k_lin + DIG_LIN_8] > VAL_0)
            {
                frec_array[LINE_1_DIGITS - 2 - digit_k_lin + DIG_LIN_8]--;
            }
            else
            {
                frec_array[LINE_1_DIGITS - 2 - digit_k_lin + DIG_LIN_8] = VAL_1;
            }
        }

        for(int j = 0; j <= LINE_1_DIGITS - 2; j++)
        {
            frec_lin_new_num = (frec_lin_new_num * 10) + frec_array[j];
        }

        if(k_sel == K_LIN_1)
        {
            frec_lin_1.num = frec_lin_new_num;
        }
        else if(k_sel == K_LIN_2)
        {
            frec_lin_2.num = frec_lin_new_num;
        }
        else if(k_sel == K_LIN_3)
        {
            frec_lin_3.num = frec_lin_new_num;
        }
        else if(k_sel == K_LIN_4)
        {
            frec_lin_4.num = frec_lin_new_num;
        }
        else if(k_sel == K_LIN_5)
        {
            frec_lin_5.num = frec_lin_new_num;
        }
    }
}

/*
 * @brief modifica la resolución de los factores acm y ttl.
 * @param Resoluciones de fabrica, y de los parámetros ACM y TTL.
 * @retval None
 */
void fm_factory_modify_res_acm_ttl(sel_resolution_t units_res,
sel_resolution_t acm_res, sel_resolution_t ttl_res)
{
    units_digits_vol.res = units_res;
    acm_config.volume.res = acm_res;
    ttl_config.volume.res = ttl_res;
}

/*
 * @brief modifica la resolución del factor rate.
 * @param Resolución de fabrica, y del parámetro rate.
 * @retval None
 */
void fm_factory_modify_res_rate(sel_resolution_t units_res,
sel_resolution_t rate_res)
{
    units_digits_tim.res = units_res;
    rate_config.volume.res = rate_res;
}

/*
 * @brief Modifica las unidades de tiempo que se muestran en los menús de
 * usuario.
 * @param A que unidad se quiere cambiar.
 * @retval None
 */
void fm_factory_modify_time_units(fmc_unit_time_t time_units)
{
    acm_config.unit_time = time_units;
    ttl_config.unit_time = time_units;
    rate_config.unit_time = time_units;
}

/*
 * @brief Modifica las unidades de volumen que se muestran en los menús de
 * usuario.
 * @param A que unidad se quiere cambiar.
 * @retval None
 */
void fm_factory_modify_volume_units(fmc_unit_volume_t volume_units)
{
    acm_config.unit_volume = volume_units;
    ttl_config.unit_volume = volume_units;
    rate_config.unit_volume = volume_units;
}

/*
 * @brief Función que separa en dígitos el factor K y los guarda en un arreglo
 * global.
 * @param None
 * @retval None
 */
void fm_factory_separate_k_factor()
{
    uint32_t k_num;
    int i = 7;
    k_num = fm_factory_get_k_factor(K_FACTOR).num;

    while (i >= 0)
    {
        if (k_num > 0)
        {
            k_array[i] = k_num % 10;
            k_num /= 10;
        }
        else
        {
            k_array[i] = 0;
        }
        i--;
    }
}

void fm_factory_separate_k_lin_and_frec(sel_k k_sel)
{
    uint32_t k_lin_num;
    uint32_t frec_num;

    int i = 7; //Contador que funciona solo en el while de esta función.
    int j = 6; //Contador que funciona solo en el while de esta función.
    k_lin_num = fm_factory_get_k_factor(k_sel).num;
    frec_num = fm_factory_get_frec_lin(k_sel).num;

    while (i >= 0) //Introduzco el factor k linealizado en un arreglo.
    {
        if (k_lin_num > 0)
        {
            k_lin_array[i] = k_lin_num % 10;
            k_lin_num /= 10;
        }
        else
        {
            k_lin_array[i] = 0;
        }
        i--;
    }

    while(j >= 0) //Introduzco la frecuencia de linealización en otro arreglo.
    {
        if(frec_num > 0)
        {
            frec_array[j] = frec_num % 10;
            frec_num /= 10;
        }
        else
        {
            frec_array[j] = 0;
        }
        j--;
    }
}

// Interrupts

/*** end of file ***/

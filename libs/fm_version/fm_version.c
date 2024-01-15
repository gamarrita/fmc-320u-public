/* @file fm_version.c
 *
 * @brief En este módulo se irán actualizando los dígitos correspondientes a la
 * versión del firmware en formato V.R.R (Version.Revision.Release). De aquí
 * también se extraerá la versión para enviarla a la pantalla inicial de
 * fm_lcd_version.
 *
 * COPYRIGHT NOTE: (c) 2023 FLOWMEET. All right reserved.
 *
 */

// Includes.

#include "fm_version.h"
#include "stdio.h"
#include "stdlib.h"
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

// Defines.

#define VERSION     1
#define REVISION    1
#define RELEASE     2

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

// External variables.

// Global variables, statics.

// Private function prototypes.

// Private function bodies.

// Public function bodies.

/*
 * @brief Esta función devuelve la versión actual del firmware, mantener
 * actualizado este valor.
 * @param None
 * @retval Número de versión en formato VRR.
 */
uint32_t fm_version_get()
{
    uint32_t version = 0;
    char version_arr[PCF8553_DATA_SIZE];

    sprintf(version_arr, "%d%d%d", VERSION, REVISION, RELEASE);

    /*
     * Esta instrucción está de mas ya que después se vuelve a formatear el
     * entero en un arreglo, pero por ahora se va a quedar así hasta que se
     * elimine el formatter de la libreria fm_lcd.h
     */
    version = atoi(version_arr);

    return (version);
}

// Interrupts

/*** end of file ***/

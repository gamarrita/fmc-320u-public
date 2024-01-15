/* @file fm_debug.c
 *
 * @brief Este modulo envía mensajes al UART o consola ITM
 *
 * Las funciones que terminan con _itm los mensajes de envian a la consola
 * de debugger, para poder ver los mensajes se tienen que configurar:
 *
 * En Cube MX:
 * - Sys->Debug->Trace Asynchronous Sw
 * - En Clock configuration puede elegir cualquier frecuencia para HCLK, pero
 * deberá recordarla.
 *
 * En Debug Configuration:
 * Debugger->Serial Wire Viewer SVW->Enable(marcar).
 * Clock Core (Mhz)- El valor que debía recordar, HCLK en Clock Configuration.
 *
 * Iniciada la sesión de debug: en la parte inferior deberia aparecer la
 * solapa ITM console, si no aparece se la busca y se la agrega, dos pasos
 * necesarias, habilitar el port_0 y click el boton rojo para iniciar "trace".
 *
 *
 * COPYRIGHT NOTE: (c) 2023 FLOWMEET. All right reserved.
 *
 */

// Includes.
#include "fm_debug.h"

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

/*
 * Default minimo 1 mili-segundos,  por defecto 10 milisegundos, para todo
 * otro delay hay que justificar.
 *
 */
#define WAIT_FOR_UART_10MS  10

/*
 * Las siguientes macros definen: cual es el tamaño maximo de caracteres,
 * incluyendo el terminador nulo, corresponde para los diferentes tipos de
 * datos. Las siguientes macros como las rutinas de conversiones son de uso
 * tan frecuente que deberian estar es una libreria especializada.
 *
 */
#define SIZE_OF_UINT8	4 	// "255"
#define SIZE_OF_INT8	5 	// "-128"
#define SIZE_OF_UINT16	6 	// "65534"
#define SIZE_OF_INT16	7 	// "-32768"
#define SIZE_OF_UINT32	11	// "4294967295"
#define SIZE_OF_INT32	12 	// "-2147483648"
#define SIZE_OF_FLOAT	10	// "-123456.7", ojo, este tamaño es arbitrario.

//Debug.

// Project variables, non-static, at least used in other file.

// Extern variables.

extern UART_HandleTypeDef huart1;

// Global variables, statics.

// Private function prototypes.

// Public function bodies.

/*
 * @brief Esta funcion envía mensajes de debug, se puede ver al momento de
 * debugger. Se tienen que llevar ajuste en tres etapas diferentes:
 *
 * Al configurar con cubemx:
 * - Sys->Debug->Trace Asynchronous Sw
 * - En Clock configuration puede elegir cualquier frecuencia para HCLK, pero
 * deberá recordarla.
 *
 * En Debug Configuration:
 * Debugger->Serial Wire Viewer SVW->Enable(marcar).
 * Clock Core (MHz)- El valor que debía recordar, HCLK en Clock Configuration.
 *
 * Iniciada la sesión de debug: en la parte inferior debería aparecer la
 * solapa ITM console, si no aparece se la busca y se la agrega, dos pasos
 * necesarias, habilitar el port_0 y click el boton rojo para iniciar "trace".
 *
 */
void fm_debug_msg_itm(const char *msg, uint8_t len)
{
    while (*msg && len)
    {
        ITM_SendChar((uint32_t) *msg);
        len--;
        msg++;
    }
}

/*
 * @brief Envía mensajes de debug por uart, un handle al uart es declarado en otro
 * moudulo, esto modulo lo acceso como extern. Usar esta unica funcion como comunicacion
 * al UART de debug.
 *
 * @param Mensaje a transmitir y su longitud.
 *
 * @retval None
 *
 *
 */
void fm_debug_msg_uart(const char *p_msg, uint8_t len)
{
    HAL_StatusTypeDef ret = HAL_BUSY;


    /*
     * La funcion HAL_UART_Transmit retorna HAL_BUSY, si el UART esta en uso,
     * no se logro tramismitir.
     *
     */
    ret = HAL_UART_Transmit(&huart1, (const uint8_t *)p_msg, len - 1, WAIT_FOR_UART_10MS);

    if(ret == HAL_BUSY)
	{
		HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_SET);
	}
    else
    {
    	ret = HAL_UART_Transmit(&huart1, '\n', 1, WAIT_FOR_UART_10MS);
    }




}

/*
 * @brief Función que envía un número de 8 bits por el puerto UART.
 * @param Número a imprimir.
 * @retval None
 */
void fm_debug_uint8_uart(uint8_t num)
{
    char str[SIZE_OF_UINT8];
    snprintf(str, sizeof(str), "%u\n", num);
    fm_debug_msg_uart(str, strlen(str));
}

/*
 * @brief Función que envía un número de 16 bits por el puerto UART.
 * @param Número a imprimir.
 * @retval None
 */
void fm_debug_uint16_uart(uint16_t num)
{
    char str[SIZE_OF_UINT16];
    snprintf(str, sizeof(str), "%u\n", num);
    fm_debug_msg_uart(str, SIZE_OF_UINT16);
}

/*
 * @brief Función que envía un número de 32 bits por el puerto UART.
 * @param Número a imprimir.
 * @retval None
 */
void fm_debug_uint32_uart(uint32_t num)
{
    char str[SIZE_OF_UINT32];
    snprintf(str, sizeof(str), "%lu\n", num);
    fm_debug_msg_uart(str, SIZE_OF_UINT32);
}

void fm_debug_int32_uart(int32_t num)
{
    char str[SIZE_OF_INT32];
    snprintf(str, sizeof(str), "%ld\n", num);
    fm_debug_msg_uart(str, SIZE_OF_INT32);
}

/*
 * @brief Función que envía un número flotante por el puerto UART.
 * @param Número a imprimir.
 * @retval None
 */
void fm_debug_float_uart(float num)
{
    char str[SIZE_OF_FLOAT]; // @suppress("Avoid magic numbers")
    snprintf((char *)str, SIZE_OF_FLOAT, "%0.2f\n", num);
    fm_debug_msg_uart(str, SIZE_OF_FLOAT);
}

// Private function bodies.

/*** end of file ***/

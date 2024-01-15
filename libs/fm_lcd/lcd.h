/* @file lcd.h
 *
 * @brief Este es un driver intermedio entre fm_lcd.h y pcf8553.h. Entre las
 * responsabilidades de este modulo están:
 * Dar accesos a la escritura y borrado de cada caracter, punto y simbolo en el
 * LCD custom:
 * Fila HIGH de 8 caracteres
 * Fila LOW de 7 caracteres
 * Símbolos RATE, BACH, TTL, ACM, H, M, S, D....
 * Puntos para números decimales de cada fila (7 puntos en la fila HIGH, y
 * 6 en la fila LOW).
 * Tambien formatea los datos pasados como parámetros en funciones de fm_lcd.c
 * con la función lcd_format_number_in_line().
 *
 * Para que los parámetros pasados a las funciones sean genéricos y
 * representativos, se utilizan enumeraciones cuyos elementos y tipo de datos
 * son clave para su entendimiento y posterior uso. (sand)
 *
 * COPYRIGHT NOTE: (c) 2023 FLOWMEET. All right reserved.
 *
 */

#ifndef     LCD_H_
#define     LCD_H_

// includes

#include "main.h"
#include "../fmc/fmc.h"
#include "pcf8553.h"

// Typedefs.

/*
 * Enumeración de las dos filas de dígitos que tiene la pantalla LCD.
 * HIGH_ROW es la fila de arriba, y LOS_ROW es la de abajo.
 */
typedef enum
{
    HIGH_ROW, LOW_ROW
} rows_t;

/*
 * Enumeración de los puntos que se puede poner para representar números
 * con coma o en otros formatos, de la pantalla LCD. Se enumeran de izquierda a
 * derecha.
 * IMPORTANTE: si bien la enumeración tiene 7 elementos, la fila de abajo de la
 * pantalla LCD solo posee 6 puntos a ser imprimidos, por lo que al intentar
 * colocar PNT_6 en una función de impresión de la fila de abajo, no tendrá
 * efecto.
 *
 */
typedef enum
{
    PNT_0, PNT_1, PNT_2, PNT_3, PNT_4, PNT_5, PNT_6
} point_t;

/*
 * Enumeración de los bits que se puede escribir de los registros de la
 * pantalla LCD, sirve para elegir que segmento se selecciona de cada registro.
 */
typedef enum
{
    BIT_0, BIT_1, BIT_2, BIT_3, BIT_4, BIT_5, BIT_6, BIT_7
} bit_t;

/*
 * Enumeración de los registros que corresponden a los segmentos del LCD.
 * Cada registro tiene 8 bits, haciendo que 20 registros x 8 bits = 160
 * segmentos.
 */
typedef enum
{
    REG_0,
    REG_1,
    REG_2,
    REG_3,
    REG_4,
    REG_5,
    REG_6,
    REG_7,
    REG_8,
    REG_9,
    REG_10,
    REG_11,
    REG_12,
    REG_13,
    REG_14,
    REG_15,
    REG_16,
    REG_17,
    REG_18,
    REG_19
} reg_t;

typedef enum
{
    DIGIT_0, DIGIT_1, DIGIT_2, DIGIT_3, DIGIT_4, DIGIT_5, DIGIT_6, DIGIT_7
} screen_digits_t;

// Defines.

/*
 * Cantidad de filas y columnas de la pantalla, notar que se tiene dos filas
 * donde:
 * La fila 0 tiene 8 caracteres, equivalente a 8 columnas
 * La fila 1 tiene 7 caracteres, equivalente a 7 columnas
 *
 * Aun asi estas macros son para definir un buffer de 2x8, luego los límites
 * para cada fila del buffer serán 8 y 7 elementos para las filas 0 y 1
 * respectivamente.
 *
 */
#define LCD_ROWS    2
#define LCD_COLS    8

// Public function prototypes.

void lcd_clear_all();
void lcd_clear_digit(screen_digits_t digit, rows_t line);
void lcd_clear_point(rows_t line, point_t point);
void lcd_clear_symbol(symbols_t symbol, blink_t blink_speed);
void lcd_clear_time_unit(fmc_unit_time_t time_unit, blink_t blink_speed);
void lcd_clear_vol_unit(fmc_unit_volume_t vol_unit, blink_t blink_speed);
void lcd_init();
void lcd_put_char(char c, uint8_t col, rows_t row);
void lcd_refresh();
void lcd_set_point(rows_t line, point_t point);
void lcd_set_symbol(symbols_t symbol, blink_t blink_speed);
void lcd_set_time_unit(fmc_unit_time_t time_unit, blink_t blink_speed);
void lcd_set_vol_unit(fmc_unit_volume_t vol_unit, blink_t blink_speed);

#endif /* FM_LCD_H_ */

/*** end of file ***/

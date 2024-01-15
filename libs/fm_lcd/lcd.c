/* @file lcd.c
 *
 * @brief Este es un driver intermedio entre fm_lcd.c y pcf8553.c. Entre las
 * responsabilidades de este modulo están:
 * Dar accesos a la escritura y borrado de cada caracter, punto y simbolo en el
 * LCD custom:
 * Fila HIGH de 8 caracteres
 * Fila LOW de 7 caracteres
 * Símbolos RATE, BACH, TTL, ACM, H, M, S, D....
 * Puntos para números decimales de cada fila (7 puntos en la fila HIGH, y
 * 6 en la fila LOW).
 *
 * COPYRIGHT NOTE: (c) 2023 FLOWMEET. All right reserved.
 *
 */

// Includes.
#include "lcd.h"
#include "ctype.h"
#include "stdio.h"

// Typedef.

typedef struct
{
    uint8_t pos;
    uint8_t reg;
} octal_t;

// Defines.

/*
 * Ver hoja de datos del lcd, cada segmento se identifica con una letra, se
 * codifica a numero. Cada segmento esta conectado a un pin del pcf8553, luego
 * cada pin esta relacionado con bit de un registro de 8 bits en el mapa de
 * memoria del pcf8553. Se debe construir una matiz que relacione los segmentos
 * con el bit que se quiere encender, esto se hará para un solo digito, luego
 * por se obtendrán las posiciones de los siguientes dígitos.
 *
 */
#define SEG_A               0
#define SEG_B               1
#define SEG_C               2
#define SEG_D               3
#define SEG_E               4
#define SEG_F               5
#define SEG_G               6
#define SEG_H               7

#define LCD_MSG_LENGTH      12

// Project variables, non-static, at least used in other file.

// Extern variables.

extern uint8_t g_lcd_map[PCF8553_DATA_SIZE];
extern uint8_t in_configuration;

// Global variables, statics.

/*
 * Lo que se quiera escribir en las líneas 1 y 2 primero se vuelca a este
 * buffer. Leer el buffer es la unica manera practica que se tiene para
 * saber que esta escrito en la pantalla, no se debe corromper esta condicion.
 */
static uint8_t g_buf[LCD_ROWS][LCD_COLS];
static uint8_t g_col;
static uint8_t g_row;

/*
 *  Para un caracter octal, en la linea superior del LCD, inicializo el
 *  siguiente arreglo. Ver tabla memoria del PCF8553, el encendido/apagado
 *  de cada segmento esta controlado por un bit en esta tabla, son 20 registros
 *  de 8 bits cada uno, 160 bits par controlar la misma cantidad de segmentos.
 *  Los valores de inicialización de la siguiente tabla corresponde al caracter
 *  ubicado mas a la derecha en la tabla superior, ver datasheet del LCD.
 *  Cada par de valores pos y reg se corresponden a la direccion de un registro
 *  y el bit correspondiente que controlan los segmentos de A a G.
 *  Solo se necesitan los datos de este caracter, las posiciones de los demas
 *  se obtienen por aritmetica dentro de la funcion lcd_write_line.
 */
octal_t octal_1[LINE_0_DIGITS - 1] =
{
    {
        .pos = 6,
        .reg = 7
    },
    {
        .pos = 6,
        .reg = 2
    },
    {
        .pos = 7,
        .reg = 2
    },
    {
        .pos = 6,
        .reg = 17
    },
    {
        .pos = 6,
        .reg = 12
    },
    {
        .pos = 7,
        .reg = 17
    },
    {
        .pos = 7,
        .reg = 12
    },
    {
        .pos = 7,
        .reg = 7
    },
};

octal_t octal_2[LINE_0_DIGITS - 1] =
{
    {
        .pos = 7,
        .reg = 5
    },
    {
        .pos = 7,
        .reg = 0
    },
    {
        .pos = 6,
        .reg = 0
    },
    {
        .pos = 7,
        .reg = 15
    },
    {
        .pos = 7,
        .reg = 10
    },
    {
        .pos = 6,
        .reg = 15
    },
    {
        .pos = 6,
        .reg = 10
    },
    {
        .pos = 6,
        .reg = 5
    }
};

// Private function prototypes.

void lcd_write_line(uint8_t seg, uint8_t data);

// Public function bodies.

/*
 * @brief Función que limpia toda la pantalla.
 * @param  None
 * @retval None
 */
void lcd_clear_all()
{
    pcf8553_clear_buff();

    /*
     * Limpia el buffer intermedio de los números que aparecen en las filas HIGH
     * y LOW de la pantalla LCD. No es muy eficiente si se pretende usar al
     * refrescar la pantalla cada x tiempo, pero será necesario usarla por el
     * momento al pasar de una pantalla a otra.
     */
    for (int cont_buff_row = 0; cont_buff_row < LCD_ROWS; cont_buff_row++)
    {
        for (int cont_buff_col = 0; cont_buff_col < LCD_COLS; cont_buff_col++)
        {
            g_buf[cont_buff_row][cont_buff_col] = 0;
        }
    }

    pcf8553_write_all(NONE_SEGMENTS);
}

void lcd_clear_digit(screen_digits_t digit, rows_t line)
{
    if(line == HIGH_ROW)
    {
        switch(digit)
        {
            case DIGIT_0:
                g_lcd_map[REG_12] &= ~(1 << BIT_7);
                g_lcd_map[REG_12] &= ~(1 << BIT_6);
                g_lcd_map[REG_17] &= ~(1 << BIT_7);
                g_lcd_map[REG_17] &= ~(1 << BIT_6);
                g_lcd_map[REG_2] &= ~(1 << BIT_6);
                g_lcd_map[REG_2] &= ~(1 << BIT_7);
                g_lcd_map[REG_7] &= ~(1 << BIT_6);
            break;
            case DIGIT_1:
                g_lcd_map[REG_13] &= ~(1 << BIT_1);
                g_lcd_map[REG_13] &= ~(1 << BIT_0);
                g_lcd_map[REG_18] &= ~(1 << BIT_1);
                g_lcd_map[REG_18] &= ~(1 << BIT_0);
                g_lcd_map[REG_3] &= ~(1 << BIT_0);
                g_lcd_map[REG_3] &= ~(1 << BIT_1);
                g_lcd_map[REG_8] &= ~(1 << BIT_0);
            break;
            case DIGIT_2:
                g_lcd_map[REG_13] &= ~(1 << BIT_3);
                g_lcd_map[REG_13] &= ~(1 << BIT_2);
                g_lcd_map[REG_18] &= ~(1 << BIT_3);
                g_lcd_map[REG_18] &= ~(1 << BIT_2);
                g_lcd_map[REG_3] &= ~(1 << BIT_2);
                g_lcd_map[REG_3] &= ~(1 << BIT_3);
                g_lcd_map[REG_8] &= ~(1 << BIT_2);
            break;
            case DIGIT_3:
                g_lcd_map[REG_13] &= ~(1 << BIT_5);
                g_lcd_map[REG_13] &= ~(1 << BIT_4);
                g_lcd_map[REG_18] &= ~(1 << BIT_5);
                g_lcd_map[REG_18] &= ~(1 << BIT_4);
                g_lcd_map[REG_3] &= ~(1 << BIT_4);
                g_lcd_map[REG_3] &= ~(1 << BIT_5);
                g_lcd_map[REG_8] &= ~(1 << BIT_4);
            break;
            case DIGIT_4:
                g_lcd_map[REG_13] &= ~(1 << BIT_7);
                g_lcd_map[REG_13] &= ~(1 << BIT_6);
                g_lcd_map[REG_18] &= ~(1 << BIT_7);
                g_lcd_map[REG_18] &= ~(1 << BIT_6);
                g_lcd_map[REG_3] &= ~(1 << BIT_6);
                g_lcd_map[REG_3] &= ~(1 << BIT_7);
                g_lcd_map[REG_8] &= ~(1 << BIT_6);
            break;
            case DIGIT_5:
                g_lcd_map[REG_14] &= ~(1 << BIT_1);
                g_lcd_map[REG_14] &= ~(1 << BIT_0);
                g_lcd_map[REG_19] &= ~(1 << BIT_1);
                g_lcd_map[REG_19] &= ~(1 << BIT_0);
                g_lcd_map[REG_4] &= ~(1 << BIT_0);
                g_lcd_map[REG_4] &= ~(1 << BIT_1);
                g_lcd_map[REG_9] &= ~(1 << BIT_0);
            break;
            case DIGIT_6:
                g_lcd_map[REG_14] &= ~(1 << BIT_3);
                g_lcd_map[REG_14] &= ~(1 << BIT_2);
                g_lcd_map[REG_19] &= ~(1 << BIT_3);
                g_lcd_map[REG_19] &= ~(1 << BIT_2);
                g_lcd_map[REG_4] &= ~(1 << BIT_2);
                g_lcd_map[REG_4] &= ~(1 << BIT_3);
                g_lcd_map[REG_9] &= ~(1 << BIT_2);
            break;
            case DIGIT_7:
                g_lcd_map[REG_14] &= ~(1 << BIT_5);
                g_lcd_map[REG_14] &= ~(1 << BIT_4);
                g_lcd_map[REG_19] &= ~(1 << BIT_5);
                g_lcd_map[REG_19] &= ~(1 << BIT_4);
                g_lcd_map[REG_4] &= ~(1 << BIT_4);
                g_lcd_map[REG_4] &= ~(1 << BIT_5);
                g_lcd_map[REG_9] &= ~(1 << BIT_4);
            break;
            default:
            break;
        }
    }
    else if(line == LOW_ROW)
    {
        switch(digit)
        {
            case DIGIT_0:
                g_lcd_map[REG_12] &= ~(1 << BIT_2);
                g_lcd_map[REG_12] &= ~(1 << BIT_3);
                g_lcd_map[REG_17] &= ~(1 << BIT_2);
                g_lcd_map[REG_17] &= ~(1 << BIT_3);
                g_lcd_map[REG_2] &= ~(1 << BIT_2);
                g_lcd_map[REG_2] &= ~(1 << BIT_3);
                g_lcd_map[REG_7] &= ~(1 << BIT_3);
            break;
            case DIGIT_1:
                g_lcd_map[REG_12] &= ~(1 << BIT_0);
                g_lcd_map[REG_12] &= ~(1 << BIT_1);
                g_lcd_map[REG_17] &= ~(1 << BIT_0);
                g_lcd_map[REG_17] &= ~(1 << BIT_1);
                g_lcd_map[REG_2] &= ~(1 << BIT_0);
                g_lcd_map[REG_2] &= ~(1 << BIT_1);
                g_lcd_map[REG_7] &= ~(1 << BIT_1);
            break;
            case DIGIT_2:
                g_lcd_map[REG_11] &= ~(1 << BIT_6);
                g_lcd_map[REG_11] &= ~(1 << BIT_7);
                g_lcd_map[REG_16] &= ~(1 << BIT_6);
                g_lcd_map[REG_16] &= ~(1 << BIT_7);
                g_lcd_map[REG_1] &= ~(1 << BIT_6);
                g_lcd_map[REG_1] &= ~(1 << BIT_7);
                g_lcd_map[REG_6] &= ~(1 << BIT_7);
            break;
            case DIGIT_3:
                g_lcd_map[REG_11] &= ~(1 << BIT_4);
                g_lcd_map[REG_11] &= ~(1 << BIT_5);
                g_lcd_map[REG_16] &= ~(1 << BIT_4);
                g_lcd_map[REG_16] &= ~(1 << BIT_5);
                g_lcd_map[REG_1] &= ~(1 << BIT_4);
                g_lcd_map[REG_1] &= ~(1 << BIT_5);
                g_lcd_map[REG_6] &= ~(1 << BIT_5);
            break;
            case DIGIT_4:
                g_lcd_map[REG_11] &= ~(1 << BIT_2);
                g_lcd_map[REG_11] &= ~(1 << BIT_3);
                g_lcd_map[REG_16] &= ~(1 << BIT_2);
                g_lcd_map[REG_16] &= ~(1 << BIT_3);
                g_lcd_map[REG_1] &= ~(1 << BIT_2);
                g_lcd_map[REG_1] &= ~(1 << BIT_3);
                g_lcd_map[REG_6] &= ~(1 << BIT_3);
            break;
            case DIGIT_5:
                g_lcd_map[REG_11] &= ~(1 << BIT_0);
                g_lcd_map[REG_11] &= ~(1 << BIT_1);
                g_lcd_map[REG_16] &= ~(1 << BIT_0);
                g_lcd_map[REG_16] &= ~(1 << BIT_1);
                g_lcd_map[REG_1] &= ~(1 << BIT_0);
                g_lcd_map[REG_1] &= ~(1 << BIT_1);
                g_lcd_map[REG_6] &= ~(1 << BIT_1);
            break;
            case DIGIT_6:
                g_lcd_map[REG_10] &= ~(1 << BIT_6);
                g_lcd_map[REG_10] &= ~(1 << BIT_7);
                g_lcd_map[REG_15] &= ~(1 << BIT_6);
                g_lcd_map[REG_15] &= ~(1 << BIT_7);
                g_lcd_map[REG_0] &= ~(1 << BIT_6);
                g_lcd_map[REG_0] &= ~(1 << BIT_7);
                g_lcd_map[REG_5] &= ~(1 << BIT_7);
            break;
            default:
            break;
        }
    }
}

/*
 * @brief Función que borra un punto en específico que estaba escrito en la
 * pantalla.
 * @param Enumeraciones rows_t y point_t de lcd.h
 * @retval None
 */
void lcd_clear_point(rows_t line, point_t point)
{
    if (line == HIGH_ROW)
    {
        switch (point)
        {
            case PNT_0:
                g_lcd_map[REG_7] &= ~(1 << BIT_7);
            break;
            case PNT_1:
                g_lcd_map[REG_8] &= ~(1 << BIT_1);
            break;
            case PNT_2:
                g_lcd_map[REG_8] &= ~(1 << BIT_3);
            break;
            case PNT_3:
                g_lcd_map[REG_8] &= ~(1 << BIT_5);
            break;
            case PNT_4:
                g_lcd_map[REG_8] &= ~(1 << BIT_7);
            break;
            case PNT_5:
                g_lcd_map[REG_9] &= ~(1 << BIT_1);
            break;
            case PNT_6:
                g_lcd_map[REG_9] &= ~(1 << BIT_3);
            break;
            default:
            break;
        }
    }
    else if (line == LOW_ROW)
    {
        switch (point)
        {
            case PNT_0:
                g_lcd_map[REG_7] &= ~(1 << BIT_2);
            break;
            case PNT_1:
                g_lcd_map[REG_7] &= ~(1 << BIT_0);
            break;
            case PNT_2:
                g_lcd_map[REG_6] &= ~(1 << BIT_6);
            break;
            case PNT_3:
                g_lcd_map[REG_6] &= ~(1 << BIT_4);
            break;
            case PNT_4:
                g_lcd_map[REG_6] &= ~(1 << BIT_2);
            break;
            case PNT_5:
                g_lcd_map[REG_6] &= ~(1 << BIT_0);
            break;
            default:
            break;
        }
    }
}

/*
 * @brief Función que borra un símbolo en específico que estaba escrito en la
 * pantalla.
 * @param Enumeraciones symbols_t y blink_t de lcd.h
 * @retval None
 */
void lcd_clear_symbol(symbols_t symbol, blink_t blink_speed)
{
    switch (symbol)
    {
        case POWER:
            g_lcd_map[REG_2] &= ~(1 << BIT_5);
        break;
        case BATTERY:
            g_lcd_map[REG_7] &= ~(1 << BIT_5);
        break;
        case BATCH:
            g_lcd_map[REG_12] &= ~(1 << BIT_5);
        break;
        case RATE:
            g_lcd_map[REG_17] &= ~(1 << BIT_5);
        break;
        case E:
            g_lcd_map[REG_2] &= ~(1 << BIT_4);
        break;
        case POINT:
            g_lcd_map[REG_7] &= ~(1 << BIT_4);
        break;
        case ACM:
            g_lcd_map[REG_12] &= ~(1 << BIT_4);
        break;
        case TTL:
            g_lcd_map[REG_17] &= ~(1 << BIT_4);
        break;
        case BACKSLASH:
            g_lcd_map[REG_10] &= ~(1 << BIT_1);
        break;
        case VE:
            g_lcd_map[REG_15] &= ~(1 << BIT_5);
            g_lcd_map[REG_15] &= ~(1 << BIT_4);
            g_lcd_map[REG_0] &= ~(1 << BIT_5);
            g_lcd_map[REG_0] &= ~(1 << BIT_2);

            g_lcd_map[REG_14] &= ~(1 << BIT_7);
            g_lcd_map[REG_15] &= ~(1 << BIT_1);
            g_lcd_map[REG_19] &= ~(1 << BIT_6);
            g_lcd_map[REG_0] &= ~(1 << BIT_0);
            g_lcd_map[REG_0] &= ~(1 << BIT_1);
            g_lcd_map[REG_5] &= ~(1 << BIT_1);
        break;
        case PASS:
            g_lcd_map[REG_13] &= ~(1 << BIT_1);
            g_lcd_map[REG_13] &= ~(1 << BIT_0);
            g_lcd_map[REG_18] &= ~(1 << BIT_1);
            g_lcd_map[REG_18] &= ~(1 << BIT_0);
            g_lcd_map[REG_3] &= ~(1 << BIT_0);

            g_lcd_map[REG_13] &= ~(1 << BIT_3);
            g_lcd_map[REG_13] &= ~(1 << BIT_2);
            g_lcd_map[REG_18] &= ~(1 << BIT_3);
            g_lcd_map[REG_18] &= ~(1 << BIT_2);
            g_lcd_map[REG_3] &= ~(1 << BIT_2);
            g_lcd_map[REG_3] &= ~(1 << BIT_3);

            g_lcd_map[REG_13] &= ~(1 << BIT_5);
            g_lcd_map[REG_13] &= ~(1 << BIT_4);
            g_lcd_map[REG_18] &= ~(1 << BIT_4);
            g_lcd_map[REG_3] &= ~(1 << BIT_5);
            g_lcd_map[REG_8] &= ~(1 << BIT_4);

            g_lcd_map[REG_13] &= ~(1 << BIT_7);
            g_lcd_map[REG_13] &= ~(1 << BIT_6);
            g_lcd_map[REG_18] &= ~(1 << BIT_6);
            g_lcd_map[REG_3] &= ~(1 << BIT_7);
            g_lcd_map[REG_8] &= ~(1 << BIT_6);

        break;
        case K:
            g_lcd_map[REG_15] &= ~(1 << BIT_5);
            g_lcd_map[REG_15] &= ~(1 << BIT_4);
            g_lcd_map[REG_15] &= ~(1 << BIT_2);
            g_lcd_map[REG_0] &= ~(1 << BIT_5);
            g_lcd_map[REG_5] &= ~(1 << BIT_4);
        break;
        case KO:
            g_lcd_map[REG_15] &= ~(1 << BIT_5);
            g_lcd_map[REG_15] &= ~(1 << BIT_4);
            g_lcd_map[REG_15] &= ~(1 << BIT_2);
            g_lcd_map[REG_0] &= ~(1 << BIT_5);
            g_lcd_map[REG_5] &= ~(1 << BIT_4);

            g_lcd_map[REG_19] &= ~(1 << BIT_6);
            g_lcd_map[REG_0] &= ~(1 << BIT_0);
            g_lcd_map[REG_0] &= ~(1 << BIT_1);
            g_lcd_map[REG_4] &= ~(1 << BIT_7);
            g_lcd_map[REG_5] &= ~(1 << BIT_1);
        break;
        case PASS1:
            g_lcd_map[REG_12] &= ~(1 << BIT_0);
            g_lcd_map[REG_12] &= ~(1 << BIT_1);
            g_lcd_map[REG_17] &= ~(1 << BIT_0);
            g_lcd_map[REG_17] &= ~(1 << BIT_1);
            g_lcd_map[REG_2] &= ~(1 << BIT_1);
            g_lcd_map[REG_2] &= ~(1 << BIT_0);
            g_lcd_map[REG_7] &= ~(1 << BIT_1);
        break;
        case PASS2:
            g_lcd_map[REG_11] &= ~(1 << BIT_6);
            g_lcd_map[REG_11] &= ~(1 << BIT_7);
            g_lcd_map[REG_16] &= ~(1 << BIT_6);
            g_lcd_map[REG_16] &= ~(1 << BIT_7);
            g_lcd_map[REG_1] &= ~(1 << BIT_7);
            g_lcd_map[REG_1] &= ~(1 << BIT_6);
            g_lcd_map[REG_6] &= ~(1 << BIT_7);
        break;
        case PASS3:
            g_lcd_map[REG_11] &= ~(1 << BIT_4);
            g_lcd_map[REG_11] &= ~(1 << BIT_5);
            g_lcd_map[REG_16] &= ~(1 << BIT_4);
            g_lcd_map[REG_16] &= ~(1 << BIT_5);
            g_lcd_map[REG_1] &= ~(1 << BIT_5);
            g_lcd_map[REG_1] &= ~(1 << BIT_4);
            g_lcd_map[REG_6] &= ~(1 << BIT_5);
        break;
        default:
        break;
    }

    pcf8553_blink(blink_speed);
}

void lcd_clear_time_unit(fmc_unit_time_t time_unit, blink_t blink_speed)
{
    switch (time_unit)
    {
        case H:
            g_lcd_map[REG_5] &= ~(1 << BIT_6);
        break;
        case D:
            g_lcd_map[REG_9] &= ~(1 << BIT_5);
        break;
        case S:
            g_lcd_map[REG_10] &= ~(1 << BIT_5);
        break;
        case M:
            g_lcd_map[REG_9] &= ~(1 << BIT_7);
        break;
        default:
        break;
    }
    pcf8553_blink(blink_speed);
}

void lcd_clear_vol_unit(fmc_unit_volume_t vol_unit, blink_t blink_speed)
{
    switch (vol_unit)
    {
        case LT:
            g_lcd_map[REG_15] &= ~(1 << BIT_5);
            g_lcd_map[REG_0] &= ~(1 << BIT_5);
            g_lcd_map[REG_5] &= ~(1 << BIT_5);

            g_lcd_map[REG_14] &= ~(1 << BIT_7);
            g_lcd_map[REG_10] &= ~(1 << BIT_0);
            g_lcd_map[REG_9] &= ~(1 << BIT_6);
        break;
        case KG:
            g_lcd_map[REG_15] &= ~(1 << BIT_5);
            g_lcd_map[REG_15] &= ~(1 << BIT_4);
            g_lcd_map[REG_15] &= ~(1 << BIT_2);
            g_lcd_map[REG_0] &= ~(1 << BIT_5);
            g_lcd_map[REG_5] &= ~(1 << BIT_4);

            g_lcd_map[REG_14] &= ~(1 << BIT_7);
            g_lcd_map[REG_15] &= ~(1 << BIT_1);
            g_lcd_map[REG_0] &= ~(1 << BIT_0);
            g_lcd_map[REG_0] &= ~(1 << BIT_1);
            g_lcd_map[REG_4] &= ~(1 << BIT_7);
            g_lcd_map[REG_5] &= ~(1 << BIT_1);
        break;
        case M3:
            g_lcd_map[REG_15] &= ~(1 << BIT_5);
            g_lcd_map[REG_10] &= ~(1 << BIT_2);
            g_lcd_map[REG_15] &= ~(1 << BIT_4);
            g_lcd_map[REG_15] &= ~(1 << BIT_3);
            g_lcd_map[REG_0] &= ~(1 << BIT_5);
            g_lcd_map[REG_0] &= ~(1 << BIT_3);

            g_lcd_map[REG_14] &= ~(1 << BIT_7);
            g_lcd_map[REG_19] &= ~(1 << BIT_7);
            g_lcd_map[REG_19] &= ~(1 << BIT_6);
            g_lcd_map[REG_0] &= ~(1 << BIT_0);
            g_lcd_map[REG_4] &= ~(1 << BIT_7);
            g_lcd_map[REG_5] &= ~(1 << BIT_1);
        break;
        case BR:
            g_lcd_map[REG_10] &= ~(1 << BIT_3);
            g_lcd_map[REG_10] &= ~(1 << BIT_4);
            g_lcd_map[REG_15] &= ~(1 << BIT_3);
            g_lcd_map[REG_0] &= ~(1 << BIT_4);
            g_lcd_map[REG_5] &= ~(1 << BIT_2);
            g_lcd_map[REG_0] &= ~(1 << BIT_3);
            g_lcd_map[REG_5] &= ~(1 << BIT_5);

            g_lcd_map[REG_14] &= ~(1 << BIT_7);
            g_lcd_map[REG_15] &= ~(1 << BIT_1);
            g_lcd_map[REG_19] &= ~(1 << BIT_7);
            g_lcd_map[REG_19] &= ~(1 << BIT_6);
            g_lcd_map[REG_0] &= ~(1 << BIT_0);
            g_lcd_map[REG_0] &= ~(1 << BIT_1);
            g_lcd_map[REG_5] &= ~(1 << BIT_0);
        break;
        case GL:
            g_lcd_map[REG_10] &= ~(1 << BIT_3);
            g_lcd_map[REG_15] &= ~(1 << BIT_5);
            g_lcd_map[REG_0] &= ~(1 << BIT_4);
            g_lcd_map[REG_0] &= ~(1 << BIT_5);
            g_lcd_map[REG_0] &= ~(1 << BIT_3);
            g_lcd_map[REG_5] &= ~(1 << BIT_5);

            g_lcd_map[REG_15] &= ~(1 << BIT_1);
            g_lcd_map[REG_0] &= ~(1 << BIT_1);
            g_lcd_map[REG_5] &= ~(1 << BIT_1);
        break;
        case CELSIUS:
            g_lcd_map[REG_10] &= ~(1 << BIT_3);
            g_lcd_map[REG_15] &= ~(1 << BIT_5);
            g_lcd_map[REG_15] &= ~(1 << BIT_3);
            g_lcd_map[REG_15] &= ~(1 << BIT_2);
            g_lcd_map[REG_0] &= ~(1 << BIT_4);

            g_lcd_map[REG_14] &= ~(1 << BIT_7);
            g_lcd_map[REG_15] &= ~(1 << BIT_1);
            g_lcd_map[REG_0] &= ~(1 << BIT_1);
            g_lcd_map[REG_5] &= ~(1 << BIT_1);
        break;
        case NOTHING:
            g_lcd_map[REG_15] &= ~(1 << BIT_2);
            g_lcd_map[REG_0] &= ~(1 << BIT_4);
            g_lcd_map[REG_19] &= ~(1 << BIT_6);
            g_lcd_map[REG_0] &= ~(1 << BIT_0);
        break;
        default:
        break;
    }
    pcf8553_blink(blink_speed);
}

/*
 * @brief Inicialización de la pantalla LCD escribiéndola por completo y luego
 * borrándola.
 * @param  None
 * @retval None
 */
void lcd_init()
{
    pcf8553_init();
}

/*
 * @brief Función que escribe en pantalla un caracter en la columna y fila
 * elegida.
 * @param caracter a escribir tipo char, columna y fila de tipo uint8_t
 * @retval None
 */
void lcd_put_char(char c, uint8_t col, uint8_t row)
{
    g_row = row + 1;

    /*
     * Si el buffer contiene lo mismo que se va a escribir salteo la escritura.
     * Aunque esto mejora mucho la eficiencia, no tengo que escribir por SPI
     * este caracter, considero muy riesgosa esta tecnica, la sincronización
     * entre el buffer y el contenido de la memoria del pcf8553 se debe
     * asegurar. El uso del return prematuro no se si es buena práctica.
     *
     * No se activa esta funcionalidad si se está en el menú de configuración,
     * debido a que entra en conflicto con la funcionalidad del parpadeo de los
     * campos a modificar.
     */
    if(in_configuration == 0)
    {
        if (g_buf[row][col] == c)
        {
            return;
        }
    }

    g_buf[row][col] = c;

    switch (row)
    {
        case 0:
            if (col < LINE_0_DIGITS - 1)
            {
                g_col = col;
            }
            else
            {
                return;
            }
        break;
        case 1:
            if (col < LINE_1_DIGITS - 1)
            {
                g_col = 6 - col; // @suppress("Avoid magic numbers")
            }
            else
            {
                return;
            }

        break;
        default:
            return;
        break;
    }

    switch (c)
    {
        case ' ':
            lcd_write_line(SEG_A, 0);
            lcd_write_line(SEG_B, 0);
            lcd_write_line(SEG_C, 0);
            lcd_write_line(SEG_D, 0);
            lcd_write_line(SEG_E, 0);
            lcd_write_line(SEG_F, 0);
            lcd_write_line(SEG_G, 0);
            lcd_write_line(SEG_H, 0);
        break;
        case '.':
            lcd_write_line(SEG_H, 1);
        break;
        case '0':
            lcd_write_line(SEG_A, 1);
            lcd_write_line(SEG_B, 1);
            lcd_write_line(SEG_C, 1);
            lcd_write_line(SEG_D, 0);
            lcd_write_line(SEG_E, 1);
            lcd_write_line(SEG_F, 1);
            lcd_write_line(SEG_G, 1);
            lcd_write_line(SEG_H, 0);
        break;
        case '1':
            lcd_write_line(SEG_A, 0);
            lcd_write_line(SEG_B, 0);
            lcd_write_line(SEG_C, 1);
            lcd_write_line(SEG_D, 0);
            lcd_write_line(SEG_E, 0);
            lcd_write_line(SEG_F, 1);
            lcd_write_line(SEG_G, 0);
            lcd_write_line(SEG_H, 0);
        break;
        case '2':
            lcd_write_line(SEG_A, 1);
            lcd_write_line(SEG_B, 1);
            lcd_write_line(SEG_C, 0);
            lcd_write_line(SEG_D, 1);
            lcd_write_line(SEG_E, 0);
            lcd_write_line(SEG_F, 1);
            lcd_write_line(SEG_G, 1);
            lcd_write_line(SEG_H, 0);
        break;
        case '3':
            lcd_write_line(SEG_A, 1);
            lcd_write_line(SEG_B, 0);
            lcd_write_line(SEG_C, 1);
            lcd_write_line(SEG_D, 1);
            lcd_write_line(SEG_E, 0);
            lcd_write_line(SEG_F, 1);
            lcd_write_line(SEG_G, 1);
            lcd_write_line(SEG_H, 0);
        break;
        case '4':
            lcd_write_line(SEG_A, 0);
            lcd_write_line(SEG_B, 0);
            lcd_write_line(SEG_C, 1);
            lcd_write_line(SEG_D, 1);
            lcd_write_line(SEG_E, 1);
            lcd_write_line(SEG_F, 1);
            lcd_write_line(SEG_G, 0);
            lcd_write_line(SEG_H, 0);
        break;
        case '5':
            lcd_write_line(SEG_A, 1);
            lcd_write_line(SEG_B, 0);
            lcd_write_line(SEG_C, 1);
            lcd_write_line(SEG_D, 1);
            lcd_write_line(SEG_E, 1);
            lcd_write_line(SEG_F, 0);
            lcd_write_line(SEG_G, 1);
            lcd_write_line(SEG_H, 0);
        break;
        case '6':
            lcd_write_line(SEG_A, 1);
            lcd_write_line(SEG_B, 1);
            lcd_write_line(SEG_C, 1);
            lcd_write_line(SEG_D, 1);
            lcd_write_line(SEG_E, 1);
            lcd_write_line(SEG_F, 0);
            lcd_write_line(SEG_G, 1);
            lcd_write_line(SEG_H, 0);
        break;
        case '7':
            lcd_write_line(SEG_A, 0);
            lcd_write_line(SEG_B, 0);
            lcd_write_line(SEG_C, 1);
            lcd_write_line(SEG_D, 0);
            lcd_write_line(SEG_E, 0);
            lcd_write_line(SEG_F, 1);
            lcd_write_line(SEG_G, 1);
            lcd_write_line(SEG_H, 0);
        break;
        case '8':
            lcd_write_line(SEG_A, 1);
            lcd_write_line(SEG_B, 1);
            lcd_write_line(SEG_C, 1);
            lcd_write_line(SEG_D, 1);
            lcd_write_line(SEG_E, 1);
            lcd_write_line(SEG_F, 1);
            lcd_write_line(SEG_G, 1);
            lcd_write_line(SEG_H, 0);
        break;
        case '9':
            lcd_write_line(SEG_A, 0);
            lcd_write_line(SEG_B, 0);
            lcd_write_line(SEG_C, 1);
            lcd_write_line(SEG_D, 1);
            lcd_write_line(SEG_E, 1);
            lcd_write_line(SEG_F, 1);
            lcd_write_line(SEG_G, 1);
            lcd_write_line(SEG_H, 0);
        break;
        default:
        break;
    }
}

/*
 * @brief Función que envía por medio de un buffer lo escrito en la pantalla
 * anteriormente. Es un método mas eficiente primero escribir lo que se quiera
 * enviar en dicho buffer y luego escribir todo junto.
 * @param Enumeraciones rows_t y point_t de lcd.h
 * @retval None
 */
void lcd_refresh()
{
    pcf8553_dump();
}

/*
 * @brief Función que escribe un punto en específico en la pantalla.
 * @param Enumeraciones rows_t y point_t de lcd.h
 * @retval None
 */
void lcd_set_point(rows_t line, point_t point)
{
    if (line == HIGH_ROW)
    {
        switch (point)
        {
            case PNT_0:
                g_lcd_map[REG_7] |= (1 << BIT_7);
            break;
            case PNT_1:
                g_lcd_map[REG_8] |= (1 << BIT_1);
            break;
            case PNT_2:
                g_lcd_map[REG_8] |= (1 << BIT_3);
            break;
            case PNT_3:
                g_lcd_map[REG_8] |= (1 << BIT_5);
            break;
            case PNT_4:
                g_lcd_map[REG_8] |= (1 << BIT_7);
            break;
            case PNT_5:
                g_lcd_map[REG_9] |= (1 << BIT_1);
            break;
            case PNT_6:
                g_lcd_map[REG_9] |= (1 << BIT_3);
            break;
            default:
            break;
        }
    }
    else if (line == LOW_ROW)
    {
        switch (point)
        {
            case PNT_0:
                g_lcd_map[REG_7] |= (1 << BIT_2);
            break;
            case PNT_1:
                g_lcd_map[REG_7] |= (1 << BIT_0);
            break;
            case PNT_2:
                g_lcd_map[REG_6] |= (1 << BIT_6);
            break;
            case PNT_3:
                g_lcd_map[REG_6] |= (1 << BIT_4);
            break;
            case PNT_4:
                g_lcd_map[REG_6] |= (1 << BIT_2);
            break;
            case PNT_5:
                g_lcd_map[REG_6] |= (1 << BIT_0);
            break;
            default:
            break;
        }
    }
}

/*
 * @brief Función que escribe un símbolo en específico en la pantalla.
 * @param Enumeraciones symbols_t y blink_t de lcd.h
 * @retval None
 */
void lcd_set_symbol(symbols_t symbol, blink_t blink_speed)
{
    switch (symbol)
    {
        case POWER:
            g_lcd_map[REG_2] |= (1 << BIT_5);
        break;
        case BATTERY:
            g_lcd_map[REG_7] |= (1 << BIT_5);
        break;
        case BATCH:
            g_lcd_map[REG_12] |= (1 << BIT_5);
        break;
        case RATE:
            g_lcd_map[REG_17] |= (1 << BIT_5);
        break;
        case E:
            g_lcd_map[REG_2] |= (1 << BIT_4);
        break;
        case POINT:
            g_lcd_map[REG_7] |= (1 << BIT_4);
        break;
        case ACM:
            g_lcd_map[REG_12] |= (1 << BIT_4);
        break;
        case TTL:
            g_lcd_map[REG_17] |= (1 << BIT_4);
        break;
        case BACKSLASH:
            g_lcd_map[REG_10] |= (1 << BIT_1);
        break;
        case VE:
            g_lcd_map[REG_15] |= (1 << BIT_5);
            g_lcd_map[REG_15] |= (1 << BIT_4);
            g_lcd_map[REG_0] |= (1 << BIT_5);
            g_lcd_map[REG_0] |= (1 << BIT_2);

            g_lcd_map[REG_14] |= (1 << BIT_7);
            g_lcd_map[REG_15] |= (1 << BIT_1);
            g_lcd_map[REG_19] |= (1 << BIT_6);
            g_lcd_map[REG_0] |= (1 << BIT_0);
            g_lcd_map[REG_0] |= (1 << BIT_1);
            g_lcd_map[REG_5] |= (1 << BIT_1);
        break;
        case PASS:
            g_lcd_map[REG_13] |= (1 << BIT_1);
            g_lcd_map[REG_13] |= (1 << BIT_0);
            g_lcd_map[REG_18] |= (1 << BIT_1);
            g_lcd_map[REG_18] |= (1 << BIT_0);
            g_lcd_map[REG_3] |= (1 << BIT_0);

            g_lcd_map[REG_13] |= (1 << BIT_3);
            g_lcd_map[REG_13] |= (1 << BIT_2);
            g_lcd_map[REG_18] |= (1 << BIT_3);
            g_lcd_map[REG_18] |= (1 << BIT_2);
            g_lcd_map[REG_3] |= (1 << BIT_2);
            g_lcd_map[REG_3] |= (1 << BIT_3);

            g_lcd_map[REG_13] |= (1 << BIT_5);
            g_lcd_map[REG_13] |= (1 << BIT_4);
            g_lcd_map[REG_18] |= (1 << BIT_4);
            g_lcd_map[REG_3] |= (1 << BIT_5);
            g_lcd_map[REG_8] |= (1 << BIT_4);

            g_lcd_map[REG_13] |= (1 << BIT_7);
            g_lcd_map[REG_13] |= (1 << BIT_6);
            g_lcd_map[REG_18] |= (1 << BIT_6);
            g_lcd_map[REG_3] |= (1 << BIT_7);
            g_lcd_map[REG_8] |= (1 << BIT_6);

        break;
        case K:
            g_lcd_map[REG_15] |= (1 << BIT_5);
            g_lcd_map[REG_15] |= (1 << BIT_4);
            g_lcd_map[REG_15] |= (1 << BIT_2);
            g_lcd_map[REG_0] |= (1 << BIT_5);
            g_lcd_map[REG_5] |= (1 << BIT_4);
        break;
        case KO:
            g_lcd_map[REG_15] |= (1 << BIT_5);
            g_lcd_map[REG_15] |= (1 << BIT_4);
            g_lcd_map[REG_15] |= (1 << BIT_2);
            g_lcd_map[REG_0] |= (1 << BIT_5);
            g_lcd_map[REG_5] |= (1 << BIT_4);

            g_lcd_map[REG_19] |= (1 << BIT_6);
            g_lcd_map[REG_0] |= (1 << BIT_0);
            g_lcd_map[REG_0] |= (1 << BIT_1);
            g_lcd_map[REG_4] |= (1 << BIT_7);
            g_lcd_map[REG_5] |= (1 << BIT_1);
        break;
        case PASS1:
            g_lcd_map[REG_12] |= (1 << BIT_0);
            g_lcd_map[REG_12] |= (1 << BIT_1);
            g_lcd_map[REG_17] |= (1 << BIT_0);
            g_lcd_map[REG_17] |= (1 << BIT_1);
            g_lcd_map[REG_2] |= (1 << BIT_1);
            g_lcd_map[REG_2] |= (1 << BIT_0);
            g_lcd_map[REG_7] |= (1 << BIT_1);
        break;
        case PASS2:
            g_lcd_map[REG_11] |= (1 << BIT_6);
            g_lcd_map[REG_11] |= (1 << BIT_7);
            g_lcd_map[REG_16] |= (1 << BIT_6);
            g_lcd_map[REG_16] |= (1 << BIT_7);
            g_lcd_map[REG_1] |= (1 << BIT_7);
            g_lcd_map[REG_1] |= (1 << BIT_6);
            g_lcd_map[REG_6] |= (1 << BIT_7);
        break;
        case PASS3:
            g_lcd_map[REG_11] |= (1 << BIT_4);
            g_lcd_map[REG_11] |= (1 << BIT_5);
            g_lcd_map[REG_16] |= (1 << BIT_4);
            g_lcd_map[REG_16] |= (1 << BIT_5);
            g_lcd_map[REG_1] |= (1 << BIT_5);
            g_lcd_map[REG_1] |= (1 << BIT_4);
            g_lcd_map[REG_6] |= (1 << BIT_5);
        break;
        case F:
            g_lcd_map[REG_10] |= (1 << BIT_3);
            g_lcd_map[REG_15] |= (1 << BIT_5);
            g_lcd_map[REG_15] |= (1 << BIT_2);
            g_lcd_map[REG_0]  |= (1 << BIT_5);
        break;
        case LIN_1:
            g_lcd_map[REG_15] |= (1 << BIT_1);
            g_lcd_map[REG_0] |= (1 << BIT_1);
        break;
        case LIN_2:
            g_lcd_map[REG_14] |= (1 << BIT_7);
            g_lcd_map[REG_19] |= (1 << BIT_7);
            g_lcd_map[REG_0]  |= (1 << BIT_0);
            g_lcd_map[REG_19] |= (1 << BIT_6);
            g_lcd_map[REG_0]  |= (1 << BIT_1);
            g_lcd_map[REG_5]  |= (1 << BIT_1);
        break;
        case LIN_3:
            g_lcd_map[REG_14] |= (1 << BIT_7);
            g_lcd_map[REG_19] |= (1 << BIT_7);
            g_lcd_map[REG_0]  |= (1 << BIT_0);
            g_lcd_map[REG_19] |= (1 << BIT_6);
            g_lcd_map[REG_4]  |= (1 << BIT_7);
            g_lcd_map[REG_5]  |= (1 << BIT_1);
        break;
        case LIN_4:
            g_lcd_map[REG_15] |= (1 << BIT_1);
            g_lcd_map[REG_19] |= (1 << BIT_7);
            g_lcd_map[REG_0]  |= (1 << BIT_0);
            g_lcd_map[REG_19] |= (1 << BIT_6);
            g_lcd_map[REG_4]  |= (1 << BIT_7);
        break;
        case LIN_5:
            g_lcd_map[REG_14] |= (1 << BIT_7);
            g_lcd_map[REG_15] |= (1 << BIT_1);
            g_lcd_map[REG_0]  |= (1 << BIT_0);
            g_lcd_map[REG_19] |= (1 << BIT_6);
            g_lcd_map[REG_4]  |= (1 << BIT_7);
            g_lcd_map[REG_5]  |= (1 << BIT_1);
        break;
        default:
        break;
    }

    pcf8553_blink(blink_speed);
}

/*
 * @brief Función que imprime una de las 4 unidades de tiempo disponibles en la
 * pantalla LCD custom.
 * @param Unidad a imprimir y velocidad de parpadeo.
 * @retval None
 */
void lcd_set_time_unit(fmc_unit_time_t time_unit, blink_t blink_speed)
{
    switch (time_unit)
    {
        case H:
            g_lcd_map[REG_5] |= (1 << BIT_6);
        break;
        case D:
            g_lcd_map[REG_9] |= (1 << BIT_5);
        break;
        case S:
            g_lcd_map[REG_10] |= (1 << BIT_5);
        break;
        case M:
            g_lcd_map[REG_9] |= (1 << BIT_7);
        break;
        default:
        break;
    }
    pcf8553_blink(blink_speed);
}

/*
 * @brief Función que imprime la unidad de volumen a utilizar.
 * @param Unidad a imprimir y velocidad de parpadeo.
 * @retval None
 */
void lcd_set_vol_unit(fmc_unit_volume_t vol_unit, blink_t blink_speed)
{
    switch (vol_unit)
    {
        case LT:
            g_lcd_map[REG_15] |= (1 << BIT_5);
            g_lcd_map[REG_0] |= (1 << BIT_5);
            g_lcd_map[REG_5] |= (1 << BIT_5);

            g_lcd_map[REG_14] |= (1 << BIT_7);
            g_lcd_map[REG_10] |= (1 << BIT_0);
            g_lcd_map[REG_9] |= (1 << BIT_6);
        break;
        case KG:
            g_lcd_map[REG_15] |= (1 << BIT_5);
            g_lcd_map[REG_15] |= (1 << BIT_4);
            g_lcd_map[REG_15] |= (1 << BIT_2);
            g_lcd_map[REG_0] |= (1 << BIT_5);
            g_lcd_map[REG_5] |= (1 << BIT_4);

            g_lcd_map[REG_14] |= (1 << BIT_7);
            g_lcd_map[REG_15] |= (1 << BIT_1);
            g_lcd_map[REG_0] |= (1 << BIT_0);
            g_lcd_map[REG_0] |= (1 << BIT_1);
            g_lcd_map[REG_4] |= (1 << BIT_7);
            g_lcd_map[REG_5] |= (1 << BIT_1);
        break;
        case M3:
            g_lcd_map[REG_15] |= (1 << BIT_5);
            g_lcd_map[REG_10] |= (1 << BIT_2);
            g_lcd_map[REG_15] |= (1 << BIT_4);
            g_lcd_map[REG_15] |= (1 << BIT_3);
            g_lcd_map[REG_0] |= (1 << BIT_5);
            g_lcd_map[REG_0] |= (1 << BIT_3);

            g_lcd_map[REG_14] |= (1 << BIT_7);
            g_lcd_map[REG_19] |= (1 << BIT_7);
            g_lcd_map[REG_19] |= (1 << BIT_6);
            g_lcd_map[REG_0] |= (1 << BIT_0);
            g_lcd_map[REG_4] |= (1 << BIT_7);
            g_lcd_map[REG_5] |= (1 << BIT_1);
        break;
        case BR:
            g_lcd_map[REG_10] |= (1 << BIT_3);
            g_lcd_map[REG_10] |= (1 << BIT_4);
            g_lcd_map[REG_15] |= (1 << BIT_3);
            g_lcd_map[REG_0] |= (1 << BIT_4);
            g_lcd_map[REG_5] |= (1 << BIT_2);
            g_lcd_map[REG_0] |= (1 << BIT_3);
            g_lcd_map[REG_5] |= (1 << BIT_5);

            g_lcd_map[REG_14] |= (1 << BIT_7);
            g_lcd_map[REG_15] |= (1 << BIT_1);
            g_lcd_map[REG_19] |= (1 << BIT_7);
            g_lcd_map[REG_19] |= (1 << BIT_6);
            g_lcd_map[REG_0] |= (1 << BIT_0);
            g_lcd_map[REG_0] |= (1 << BIT_1);
            g_lcd_map[REG_5] |= (1 << BIT_0);
        break;
        case GL:
            g_lcd_map[REG_10] |= (1 << BIT_3);
            g_lcd_map[REG_15] |= (1 << BIT_5);
            g_lcd_map[REG_0] |= (1 << BIT_4);
            g_lcd_map[REG_0] |= (1 << BIT_5);
            g_lcd_map[REG_0] |= (1 << BIT_3);
            g_lcd_map[REG_5] |= (1 << BIT_5);

            g_lcd_map[REG_15] |= (1 << BIT_1);
            g_lcd_map[REG_0] |= (1 << BIT_1);
            g_lcd_map[REG_5] |= (1 << BIT_1);
        break;
        case CELSIUS:
            g_lcd_map[REG_10] |= (1 << BIT_3);
            g_lcd_map[REG_15] |= (1 << BIT_5);
            g_lcd_map[REG_15] |= (1 << BIT_3);
            g_lcd_map[REG_15] |= (1 << BIT_2);
            g_lcd_map[REG_0] |= (1 << BIT_4);

            g_lcd_map[REG_14] |= (1 << BIT_7);
            g_lcd_map[REG_15] |= (1 << BIT_1);
            g_lcd_map[REG_0] |= (1 << BIT_1);
            g_lcd_map[REG_5] |= (1 << BIT_1);
        break;
        case NOTHING:
            g_lcd_map[REG_15] |= (1 << BIT_2);
            g_lcd_map[REG_0] |= (1 << BIT_4);
            g_lcd_map[REG_19] |= (1 << BIT_6);
            g_lcd_map[REG_0] |= (1 << BIT_0);
        break;
        default:
        break;
    }
    pcf8553_blink(blink_speed);
}

// Private function bodies.

/*
 * @brief Esta funcion ajusta el estado de un segmento, encendido o apagado,
 * por cada llamada. El caracter a manipular, linea 1 o linea 2 y la posicion
 * del cursor, son informaciones guardadas en dos variables globales, g_cursor
 * y g_line. Con el mapeo de los segmentos, y sabiendo que segmento se quiere
 * manipular, notar que para el primer caracter de cada linea se tiene una matriz
 * con la posicion en el mapa de memoria para el pcf8553, con lo cual no se
 * requiere calculo, el calculo es de utilidad para los caracteres que no ocupan
 * la primera posicion de cada linea.
 * @param
 * @retval None
 */
void lcd_write_line(uint8_t seg, uint8_t data)
{
    uint8_t reg = 0;
    uint8_t pos = 0;

    /*
     *
     *
     */
    switch (g_row)
    {
        case 1:
            pos = octal_1[seg].pos;
            pos += g_col * 2;
            reg = (pos / 8) + octal_1[seg].reg; // @suppress("Avoid magic numbers")
            pos = pos % 8; // @suppress("Avoid magic numbers")
        break;
        case 2:
            pos = octal_2[seg].pos;
            pos += g_col * 2;
            reg = (pos / 8) + octal_2[seg].reg; // @suppress("Avoid magic numbers")
            pos = pos % 8; // @suppress("Avoid magic numbers")
        break;
        default:
        break;
    }

    if (data)
    {
        g_lcd_map[reg] |= 1 << pos;
    }
    else
    {
        g_lcd_map[reg] &= ~(1 << pos);
    }
}

/*** end of file ***/

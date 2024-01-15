/* @file lcd_module.h
 *
 * @brief Libreria para manejo de lcd driver de la marca nxp pcf8553.
 *
 *
 * @Excepcion Para la regla "el nombre de un modulo que fabricamos en FLOMEET
 * comienza con fm_nombre_modulo", no se aplica en este caso por ser nombre
 * singular.
 *
 * COPYRIGHT NOTE: (c) 2023 FLOWMEET. All right reserved.
 */

#ifndef     PCF8553_H_
#define     PCF8553_H_

// Includes.

#include "main.h"

// Macros for microcontroller taken pins (dhs).

/*
 * Pines dedicados al PCF8553, no me confío de siempre usar el cubemx y de su
 * funcion de label name para los pines. De ser posible siempre declaro los
 * recursos usados del hardware lo mas arriba posible en los módulos, debe ser
 * coherente con el code style del proyecto.
 *
 */
#define PCF8553_CE_PORT		GPIOA
#define PCF8553_CE_PIN 		GPIO_PIN_4
#define PCF8553_RESET_PORT	GPIOA
#define PCF8553_RESET_PIN	GPIO_PIN_5

// Typedef.

/*
 * Enumeración de las velocidades disponibles para hacer parpadear la
 * pantalla en su totalidad.
 */
typedef enum
{
    OFF_SPEED, LOW_SPEED, MED_SPEED, HIGH_SPEED
} blink_t;

/*
 *  Modeling PCF8553 Device_ctrl register.
 */
typedef union
{
    uint8_t reg_data;
    struct
    {
        uint8_t clock_ouput :1;
        uint8_t internal_oscilator :1;
        uint8_t frame_frequency :2;
        uint8_t default_value :4; // @suppress("Avoid magic numbers")
    } reg_bits;
} device_ctrl_t;

/*
 * Modeling PCF8553 Display_ctrl_1 register.
 *
 * VERY IMPORTANT: DE (Display Enabled bit) default is Power-On
 * is disabled, must be change to 1 to turn on Display.
 *
 */
typedef union
{
    uint8_t reg_data;
    struct
    {
        uint8_t display_enabled :1;
        uint8_t bias_mode :1;
        uint8_t mux :2;
        uint8_t boost :1;
        uint8_t default_value :3; // @suppress("Avoid magic numbers")
    } reg_bits;
} display_ctrl_1_t;

/*
 * Modeling PCF8553 Display_ctrl_2 register.
 *
 */
typedef union
{
    uint8_t reg_data;
    struct
    {
        uint8_t inversion :1;
        uint8_t blink :2;
        uint8_t default_value;
    } reg_bits;
} display_ctrl_2_t;

// Defines.

/*
 * Tamaño de la memoria interna del pcf8553 para controlar el encendido/apagado
 * de los segmentos, con 20 bytes se controlan 20 * 8 =  160 segmentos.
 */
#define PCF8553_DATA_SIZE   20

#define LINE_0_DIGITS       (8+1) // Dígitos linea 1 + terminador nulo.
#define LINE_1_DIGITS       (7+1) // Dígitos linea 2 + terminador nulo.

#define ALL_SEGMENTS        0xFF   // Macro para escribir 1 en todos los segmentos
#define NONE_SEGMENTS       0x00  // Macro para escribir 0 en todos los segmentos

// Public function prototypes.

void pcf8553_blink(blink_t mode);
void pcf8553_clear_buff();
void pcf8553_dump();
void pcf8553_init();
void pcf8553_reset();
void pcf8553_write_all(uint8_t data);
void pcf8553_write_byte(uint8_t add, uint8_t data);

#endif /* FM_PCF8553_H_ */

/*** end of file ***/

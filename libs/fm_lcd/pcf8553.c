/* @file pcf8553.c
 *
 * @brief Driver para integrado pcf8553 usando SPI.
 *
 * Board: NUCLEO-L452  Micro-controller: STM32L452RET
 *
 * CUBEIDE MX CONFIG:
 * SDIO  -> PA7
 * CE -> PA4
 * SCL -> PA1
 * MSI -> 24MHZ
 *
 * Nota: Es importante completar las configuracion de cube MX que falten, si
 * se inicia un proyecto de cero se deberían tener los datos de configuracion.
 *
 * (datasheet pcf8553)
 * Input data are sampled on the rising edge of SCL:
 * (STM32L4 -> CPOL = 0, CPHA = 1, CPHA can take values 1 and 2)
 * Output data are valid after the falling edge of SCL.
 * When HIGH, the interface is reset. The SPI-bus is initialized.
 * whenever the chip enable line pin CE is pulled down.
 * RESET -> Power-On enabled.
 * Conected to pint PA6, just in case it is need in future.
 *
 * COPYRIGHT NOTE: (c) 2023 FLOWMEET. All right reserved.
 *
 */

// Includes.
#include "pcf8553.h"

// Typedef.

/*
 * (datasheet) The first byte transmitted is the register address.
 *
 *  Modelo el primer byte a ser transmitido.
 *
 */
typedef union
{
    uint8_t data;
    struct
    {
        uint8_t address :5;  // @suppress("Avoid magic numbers")
        uint8_t not_used :2;
        uint8_t read_write :1;
    } bits;
} register_address_t;

// Data global constants.

const uint32_t DEFAULT_DELAY = 20;

// Defines.

/*
 * To temporally disable a block of code, use preprocessor's conditional
 * compilation features, eg, the following one should be used to increase the
 * the debug output information.
 *
 */
#ifndef NDEBUG
#endif

#define DATA_ADDRESS            4 /* First data address */

/*
 * Los dos siguientes #defines se crean para poder inicializar el miembro
 * read_write de struct register_address_t, la solucion actual no me parece
 * buena, dos bloques separados, estimo que incluyendo un enum declarado
 * dentro de struct register_address_t se tendría una mejor solucion.
 */
#define WRITE_DATA	            0
#define READ_DATA	            1

/*
 * Direcciones de memoria para el PCF8553.
 */
#define DEVICE_CTRL_ADDRESS 	0x1
#define DISPLAY_CTRL_1_ADDRESS 	0x2
#define DISPLAY_CTRL_2_ADDRESS 	0x3
#define COM_0_ADDRESS			0x4
#define COM_1_ADDRESS 			0x9
#define COM_3_ADDRESS 			0xE
#define COM_4_ADDRESS 			0x13

// Project variables, non-static, at least used in other file.

// Extern data, defined on other modules.
SPI_HandleTypeDef h_spi1;

// Non-static global, used on other modules.

/*
 *
 */
uint8_t g_lcd_map[PCF8553_DATA_SIZE];

// Global variables, statics.

/*
 * Device_ctrl register, valores de inicio.
 */
static device_ctrl_t g_device_ctrl =
{
    .reg_bits.clock_ouput = 0, /* disabled (default)*/
    .reg_bits.internal_oscilator = 0, /* disabled (default)*/
    .reg_bits.frame_frequency = 0, /* 64Hz (custom)*/
    .reg_bits.default_value = 0
};

/*
 * Display_ctrl_1 register, valores de inicio.
 * Display disabled at Power-On, DE must be change to 1.
 *
 */
static display_ctrl_1_t g_display_ctrl_1 =
{
    .reg_bits.display_enabled = 1, /* enable (not default)*/
    .reg_bits.bias_mode = 0, /* 1/3 bias (default) */
    .reg_bits.mux = 0, /* 1:4 (default) */
    .reg_bits.boost = 0, /* standard power drive, no large display (default) */
    .reg_bits.default_value = 0
/* blinking off (default) */
};

/*
 *  Display_ctrl_2 start up values.
 *
 */
static display_ctrl_2_t g_display_ctrl_2 =
{
    .reg_bits.inversion = 0, /* line inversion (default) */
    .reg_bits.blink = 0,
    .reg_bits.default_value = 0
};

// Private function prototypes.

static void prepare_to_send(uint8_t add);
static void spi1_init(void);

// Private function bodies.

/*
 * @brief The first byte transmitted is the register address comprising
 * of the address pointer and the R/W bit
 *
 * @param Address to write
 *
 * DEVICE_CTRL_ADDRESS
 * DISPLAY_CTRL_1_ADDRESS
 * DISPLAY_CTRL_2_ADDRESS
 * COM_0_ADDRESS
 * COM_1_ADDRESS
 * COM_0_ADDRESS
 * COM_1_ADDRESS
 *
 */
void static prepare_to_send(uint8_t add)
{
    register_address_t reg;
    reg.bits.address = add;
    reg.bits.not_used = 0;
    reg.bits.read_write = WRITE_DATA;
    HAL_SPI_Transmit(&h_spi1, &(reg.data), 1, DEFAULT_DELAY);
}

/**
 * @brief SPI1 Initialization Function
 * @param None
 * @retval None
 */
static void spi1_init(void)
{
    h_spi1.Instance = SPI1;
    h_spi1.Init.Mode = SPI_MODE_MASTER;
    h_spi1.Init.Direction = SPI_DIRECTION_1LINE;
    h_spi1.Init.DataSize = SPI_DATASIZE_8BIT;
    h_spi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    h_spi1.Init.CLKPhase = SPI_PHASE_1EDGE;
    h_spi1.Init.NSS = SPI_NSS_SOFT;
    h_spi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
    h_spi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    h_spi1.Init.TIMode = SPI_TIMODE_DISABLE;
    h_spi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    h_spi1.Init.CRCPolynomial = 7; // @suppress("Avoid magic numbers")
    h_spi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
    h_spi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
    if (HAL_SPI_Init(&h_spi1) != HAL_OK)
    {
        Error_Handler();
    }
}

// Public function bodies.

/*
 * @brief Modify the actual blinking mode, all segment.
 *
 * @param
 * PCF_BLINK_OFF = 0,
 * PCF_BLINK_TWO_SECOND
 * PCF_BLINK_ONE_SECOND,
 * PCF_BLINK_HALF_SECOND
 * @retval None
 */
void pcf8553_blink(blink_t mode)
{
    HAL_GPIO_WritePin(PCF8553_CE_PORT, PCF8553_CE_PIN, GPIO_PIN_RESET);
    g_display_ctrl_2.reg_bits.blink = mode;
    prepare_to_send(DISPLAY_CTRL_2_ADDRESS);
    HAL_SPI_Transmit(&h_spi1, &(g_display_ctrl_2.reg_data), 1, DEFAULT_DELAY);
    HAL_GPIO_WritePin(PCF8553_CE_PORT, PCF8553_CE_PIN, GPIO_PIN_SET);
}

void pcf8553_clear_buff()
{
    /*
     * Limpia el buffer final, que es mandado directamente al controlador de la
     * pantalla LCD.
     */
    for (int cont_buff_t = 0; cont_buff_t < PCF8553_DATA_SIZE; cont_buff_t++)
    {
        g_lcd_map[cont_buff_t] = 0;
    }
}

/*
 * @brief Aunque es posible encender o apagar los segmentos/símbolos/caracteres
 * en forma individual, lo mas practico es escribir todos los cambios en un
 * buffer y luego volcar el buffer completo al driver pcf83553. Esto responde
 * a dos motivos:
 *
 * 1. Los segmentos pertenecientes a un caracter estan distribuidos en
 * diferentes posiciones de memoria del pcf8553 no consecutivas.
 *
 * 2. La forma mas sencilla de modificar varias posiciones en el mapa de memoria
 * del pcf8553 es ir de manera secuencial una posicion tras otra.
 *
 * Esta funcion permite devolver el contenido completo de un buffer a la memoria
 * del pcf8553. El buffer mantendrá un mapa completo de los caracteres que se
 * desean en el lcd y la memoria del pcf8553.
 *
 * @param None
 * @retval None
 */
void pcf8553_dump()
{
    HAL_GPIO_WritePin(PCF8553_CE_PORT, PCF8553_CE_PIN, GPIO_PIN_RESET);
    prepare_to_send(DATA_ADDRESS);
    for (int i = 0; i < PCF8553_DATA_SIZE; i++)
    {
        HAL_SPI_Transmit(&h_spi1, g_lcd_map + i, 1, DEFAULT_DELAY);
    }
    HAL_GPIO_WritePin(PCF8553_CE_PORT, PCF8553_CE_PIN, GPIO_PIN_SET);
}

/*
 * @brief Llamar a esta función antes de empezar a trabajar con el driver.
 * @param None
 * @retval None
 */
void pcf8553_init()
{
    spi1_init();
    pcf8553_reset();
    HAL_Delay(DEFAULT_DELAY);

    /*
     * @brief El pcf8553 tiene un pin de chip select, la siguiente instruccion
     * habilita el chip, el parametro GPIO_PIN_RESET es muy parecido a un
     * numero magico, se debe buscar una mejor solucion.
     *
     */
    HAL_GPIO_WritePin(PCF8553_CE_PORT, PCF8553_CE_PIN, GPIO_PIN_RESET);

    /*
     *  Display_crtl_2 = 1 to enable display.
     *  Luego de un tiempo olvidé porque hice este comentario, no parece
     *  tener sentido.
     *
     */
    prepare_to_send(0x1);
    HAL_SPI_Transmit(&h_spi1, &(g_device_ctrl.reg_data), 1, DEFAULT_DELAY);
    HAL_SPI_Transmit(&h_spi1, &(g_display_ctrl_1.reg_data), 1, DEFAULT_DELAY);
    HAL_SPI_Transmit(&h_spi1, &(g_display_ctrl_2.reg_data), 1, DEFAULT_DELAY);

    /*
     *  Chip disable.
     *
     */
    HAL_GPIO_WritePin(PCF8553_CE_PORT, PCF8553_CE_PIN, GPIO_PIN_SET);
}

/*
 * @brief If the internal POR is disabled by connecting pin PORE to VSS,
 * the chip must be reset by driving the RST pin to logic 0 for
 * at least 10 μs.
 * @param  None
 * @retval None
 */
void pcf8553_reset()
{
    HAL_GPIO_WritePin(PCF8553_RESET_PORT, PCF8553_RESET_PIN, GPIO_PIN_RESET);
    HAL_Delay(DEFAULT_DELAY);
    HAL_GPIO_WritePin(PCF8553_RESET_PORT, PCF8553_RESET_Pin, GPIO_PIN_SET);
}

/*
 * @brief Escribe las 160 posiciones de memoria del pcf8553 con el mismo
 * byte, notar que no hay relacion 1 a 1 entre las posiciones de memoria
 * y los segmentos de los caracteres, esta funcion es util mas que nada
 * para borrar o llenar la pantalla.
 *
 * @param data es los bits a escribir, al ser "write_all" debería ser o 0xFF o
 * 0x00 para imprimir todos los caracteres o ninguno, respectivamente. En un
 * futuro se hará con una enumeración que lo deje claro, ya sea en esta función
 * o en una de mas alto nivel.
 * @retval None
 */
void pcf8553_write_all(uint8_t data)
{
    for (int i = 0; i < PCF8553_DATA_SIZE; i++)
    {
        g_lcd_map[i] = data;
    }
    pcf8553_dump();
}

/*
 * @brief Write any data address. Esta funcion fue usada al comenzar el
 * desarrollo del drive. Desde etapas muy tempranas dejo de ser usada.
 *
 * @param add es la direccion de memoria a escribir, y data es el valor a
 * escribir.
 * @retval None
 */
void pcf8553_write_byte(uint8_t add, uint8_t data)
{
    HAL_GPIO_WritePin(PCF8553_CE_PORT, PCF8553_CE_PIN, GPIO_PIN_RESET);
    prepare_to_send(add);
    HAL_SPI_Transmit(&h_spi1, &data, 1, DEFAULT_DELAY);
    HAL_GPIO_WritePin(PCF8553_CE_PORT, PCF8553_CE_PIN, GPIO_PIN_SET);
}

/*** end of file ***/

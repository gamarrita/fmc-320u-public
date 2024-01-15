/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define RTD_ADC1_1_Pin GPIO_PIN_0
#define RTD_ADC1_1_GPIO_Port GPIOC
#define BATTERY_ADC1_2_Pin GPIO_PIN_1
#define BATTERY_ADC1_2_GPIO_Port GPIOC
#define PCF8553_SCL_Pin GPIO_PIN_1
#define PCF8553_SCL_GPIO_Port GPIOA
#define PCF8553_CE_Pin GPIO_PIN_4
#define PCF8553_CE_GPIO_Port GPIOA
#define PCF8553_RESET_Pin GPIO_PIN_6
#define PCF8553_RESET_GPIO_Port GPIOA
#define PCF8553_SDIO_Pin GPIO_PIN_7
#define PCF8553_SDIO_GPIO_Port GPIOA
#define KEY_DOWN_Pin GPIO_PIN_10
#define KEY_DOWN_GPIO_Port GPIOE
#define KEY_DOWN_EXTI_IRQn EXTI10_IRQn
#define KEY_UP_Pin GPIO_PIN_11
#define KEY_UP_GPIO_Port GPIOE
#define KEY_UP_EXTI_IRQn EXTI11_IRQn
#define KEY_ENTER_Pin GPIO_PIN_12
#define KEY_ENTER_GPIO_Port GPIOE
#define KEY_ENTER_EXTI_IRQn EXTI12_IRQn
#define KEY_ESC_Pin GPIO_PIN_13
#define KEY_ESC_GPIO_Port GPIOE
#define KEY_ESC_EXTI_IRQn EXTI13_IRQn
#define FLOW_DETECTOR_Pin GPIO_PIN_15
#define FLOW_DETECTOR_GPIO_Port GPIOE
#define PULSE_LPTIM4_IN_Pin GPIO_PIN_13
#define PULSE_LPTIM4_IN_GPIO_Port GPIOD
#define LED_1_ERROR_Pin GPIO_PIN_5
#define LED_1_ERROR_GPIO_Port GPIOB
#define LED_2_ACTIVE_Pin GPIO_PIN_6
#define LED_2_ACTIVE_GPIO_Port GPIOB
#define LED_3_SIGNAL_1_Pin GPIO_PIN_7
#define LED_3_SIGNAL_1_GPIO_Port GPIOB
#define LCD_BACLLIGHT_Pin GPIO_PIN_8
#define LCD_BACLLIGHT_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

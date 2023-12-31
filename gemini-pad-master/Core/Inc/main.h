/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32f3xx_hal.h"

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
#define ALPHA_Pin GPIO_PIN_13
#define ALPHA_GPIO_Port GPIOC
#define MENU_Pin GPIO_PIN_14
#define MENU_GPIO_Port GPIOC
#define KEY4_Pin GPIO_PIN_0
#define KEY4_GPIO_Port GPIOC
#define KEY3_Pin GPIO_PIN_1
#define KEY3_GPIO_Port GPIOC
#define KEY2_Pin GPIO_PIN_2
#define KEY2_GPIO_Port GPIOC
#define KEY1_Pin GPIO_PIN_3
#define KEY1_GPIO_Port GPIOC
#define SHIFT_Pin GPIO_PIN_4
#define SHIFT_GPIO_Port GPIOF
#define SPI2_CS4_Pin GPIO_PIN_1
#define SPI2_CS4_GPIO_Port GPIOB
#define SPI2_CS3_Pin GPIO_PIN_2
#define SPI2_CS3_GPIO_Port GPIOB
#define SPI2_CS2_Pin GPIO_PIN_10
#define SPI2_CS2_GPIO_Port GPIOB
#define SPI2_CS1_Pin GPIO_PIN_11
#define SPI2_CS1_GPIO_Port GPIOB
#define WHEEL_Pin GPIO_PIN_8
#define WHEEL_GPIO_Port GPIOC
#define KNOB_Pin GPIO_PIN_9
#define KNOB_GPIO_Port GPIOC
#define RGB_Pin GPIO_PIN_3
#define RGB_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

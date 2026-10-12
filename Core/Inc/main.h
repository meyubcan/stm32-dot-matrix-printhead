/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "stm32f4xx_hal.h"

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
#define head9_Pin GPIO_PIN_0
#define head9_GPIO_Port GPIOC
#define head8_Pin GPIO_PIN_1
#define head8_GPIO_Port GPIOC
#define head7_Pin GPIO_PIN_2
#define head7_GPIO_Port GPIOC
#define head6_Pin GPIO_PIN_3
#define head6_GPIO_Port GPIOC
#define head5_Pin GPIO_PIN_0
#define head5_GPIO_Port GPIOA
#define head4_Pin GPIO_PIN_1
#define head4_GPIO_Port GPIOA
#define head3_Pin GPIO_PIN_2
#define head3_GPIO_Port GPIOA
#define head2_Pin GPIO_PIN_3
#define head2_GPIO_Port GPIOA
#define head1_Pin GPIO_PIN_4
#define head1_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

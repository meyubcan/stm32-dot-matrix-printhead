/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  * @author         : M. Eyub Can, TECH-OHM (https://tech-ohm.com)
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
// --- SETTINGS ---
#define PULSE_WIDTH_US   1000   // solenoid pulse width (us)
#define COL_DELAY_MS     50  // wait between columns (time to move the head by hand)
#define CHAR_SPACE_MS    100  // wait between characters
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */
typedef struct {
    char c;
    uint16_t cols[5];
} CharDef;

const CharDef font[] = {
    {'M', {0b111111111, 0b000000110, 0b000011000, 0b000000110, 0b111111111}},
    {'E', {0b111111111, 0b100100001, 0b100100001, 0b100100001, 0b100000001}},
    {'R', {0b111111111, 0b100010001, 0b100010001, 0b100011001, 0b011100110}},
    {'H', {0b111111111, 0b000100000, 0b000100000, 0b000100000, 0b111111111}},
    {'A', {0b011111110, 0b100000001, 0b100100001, 0b100000001, 0b011111110}},
    {'B', {0b111111111, 0b100100001, 0b100100001, 0b100100001, 0b011011110}},
    {'T', {0b100000000, 0b100000000, 0b111111111, 0b100000000, 0b100000000}},
    {'I', {0b000000000, 0b100000001, 0b111111111, 0b100000001, 0b000000000}},
    {' ', {0b000000000, 0b000000000, 0b000000000, 0b000000000, 0b000000000}},
};
#define FONT_SIZE (sizeof(font) / sizeof(font[0]))

// Text to print
const char* printText = "HI THERE";

// --- HELPERS ---

// Microsecond delay (uses TIM2)
void delay_us(uint16_t us) {
    __HAL_TIM_SET_COUNTER(&htim2, 0);
    while (__HAL_TIM_GET_COUNTER(&htim2) < us);
}

// Fire one pin (pin number 1-9)
void firePin(uint8_t pin) {
    switch(pin) {
        case 1: HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); break;
        case 2: HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET); break;
        case 3: HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET); break;
        case 4: HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET); break;
        case 5: HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET); break;
        case 6: HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET); break;
        case 7: HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET); break;
        case 8: HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET); break;
        case 9: HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET); break;
    }
}

// Release one pin
void releasePin(uint8_t pin) {
    switch(pin) {
        case 1: HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET); break;
        case 2: HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET); break;
        case 3: HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET); break;
        case 4: HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET); break;
        case 5: HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET); break;
        case 6: HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET); break;
        case 7: HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET); break;
        case 8: HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET); break;
        case 9: HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET); break;
    }
}

// Print one column. The bitmap says which pins fire.
void printColumn(uint16_t colBitmap) {
    // Fire the pins whose bits are set
    for (uint8_t pin = 1; pin <= 9; pin++) {
        if (colBitmap & (1 << (pin - 1))) {
            firePin(pin);
        }
    }

    // Hold for the pulse width
    delay_us(PULSE_WIDTH_US);

    // Release all pins
    HAL_GPIO_WritePin(GPIOA,
        GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|
        GPIO_PIN_3|GPIO_PIN_4,
        GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC,
        GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,
        GPIO_PIN_SET);
}

// Look up a character in the font table
const CharDef* findChar(char c) {
    for (int i = 0; i < FONT_SIZE; i++) {
        if (font[i].c == c) return &font[i];
    }
    return &font[FONT_SIZE - 1]; // not found: print a space
}

// Print one character
void printChar(char c) {
    const CharDef* ch = findChar(c);

    for (int col = 0; col < 5; col++) {
        printColumn(ch->cols[col]);

        // Wait between columns. This is when the head gets moved by hand.
        HAL_Delay(COL_DELAY_MS);
    }

    // Extra wait between characters
    HAL_Delay(CHAR_SPACE_MS);
}

// Print a string
void printString(const char* str) {
    while (*str) {
        printChar(*str++);
    }
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  // Start TIM2
  HAL_TIM_Base_Start(&htim2);

  // All pins high (MOSFETs off)
  HAL_GPIO_WritePin(GPIOA,
      GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|
      GPIO_PIN_3|GPIO_PIN_4,
      GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOC,
      GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,
      GPIO_PIN_SET);

  // 3 seconds to bring the head to the start position
  HAL_Delay(3000);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  printString(printText);

	     // Line done, start again in 5 seconds
	     HAL_Delay(5000);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 84-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 65535;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, head9_Pin|head8_Pin|head7_Pin|head6_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, head5_Pin|head4_Pin|head3_Pin|head2_Pin
                          |head1_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : head9_Pin head8_Pin head7_Pin head6_Pin */
  GPIO_InitStruct.Pin = head9_Pin|head8_Pin|head7_Pin|head6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : head5_Pin head4_Pin head3_Pin head2_Pin
                           head1_Pin */
  GPIO_InitStruct.Pin = head5_Pin|head4_Pin|head3_Pin|head2_Pin
                          |head1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

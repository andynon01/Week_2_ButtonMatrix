/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
//save status of Button Matrix
uint16_t ButtonMatrixState = 0;

//Button TimeStamp
uint32_t ButtonMatrixTimestamp = 0;

uint16_t EnterNumber = 0;
uint8_t CountRound = 0;
uint16_t InputCheck[11] = {};
uint32_t TimeStamp = 0;
uint8_t Login = 0;
uint8_t LED2 = 0;
uint16_t Check[2] = {};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
//scan and update data of Button Matrix
void ButtonMatrixUpdate();
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
	MX_USART2_UART_Init();
	/* USER CODE BEGIN 2 */

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		ButtonMatrixUpdate();

		//Transform to Number
		Check[0] = ButtonMatrixState;
		if (Check[0] != 0 && Check[1] == 0)
		{
			if (ButtonMatrixState == 4096){EnterNumber = 0;} // 0
			else if (ButtonMatrixState == 256){EnterNumber = 1;} // 1
			else if (ButtonMatrixState == 512){EnterNumber = 2;} // 2
			else if (ButtonMatrixState == 1024){EnterNumber = 3;} // 3
			else if (ButtonMatrixState == 16){EnterNumber = 4;} // 4
			else if (ButtonMatrixState == 32){EnterNumber = 5;} // 5
			else if (ButtonMatrixState == 64){EnterNumber = 6;} // 6
			else if (ButtonMatrixState == 1){EnterNumber = 7;} // 7
			else if (ButtonMatrixState == 2){EnterNumber = 8;} // 8
			else if (ButtonMatrixState == 4){EnterNumber = 9;} // 9
			else if (ButtonMatrixState == 8){Login = 0; CountRound = -1;} // Clear


			InputCheck[CountRound] = EnterNumber;
			CountRound += 1;
			if (ButtonMatrixState == 32768 && CountRound == 12) // Login
			{
				CountRound -= 1;
				if (   InputCheck[0] == 6
					&& InputCheck[1] == 2
					&& InputCheck[2] == 3
					&& InputCheck[3] == 4
					&& InputCheck[4] == 0
					&& InputCheck[5] == 5
					&& InputCheck[6] == 0
					&& InputCheck[7] == 0
					&& InputCheck[8] == 0
					&& InputCheck[9] == 6
					&& InputCheck[10] == 1)
				{
					Login = 1;
				}
			}

			if (ButtonMatrixState == 2048){CountRound -= 1;} // Empty
			else if (ButtonMatrixState == 8192){CountRound -= 1;} // Empty
			else if (ButtonMatrixState == 16384){CountRound -= 1;} // Empty
			else if (ButtonMatrixState == 128){CountRound -= 1;} // Empty

		}
		Check[1] = Check[0];

		if (HAL_GetTick() - TimeStamp > 100)
		{
			TimeStamp = HAL_GetTick();
			LED2 ^= 1;
		}
		if (Login == 1){HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, LED2);}
		if (Login == 0){HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);}

	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct =
	{ 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct =
	{ 0 };

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
	RCC_OscInitStruct.PLL.PLLM = 16;
	RCC_OscInitStruct.PLL.PLLN = 336;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
	RCC_OscInitStruct.PLL.PLLQ = 4;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
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
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART2_UART_Init(void)
{

	/* USER CODE BEGIN USART2_Init 0 */

	/* USER CODE END USART2_Init 0 */

	/* USER CODE BEGIN USART2_Init 1 */

	/* USER CODE END USART2_Init 1 */
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart2) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN USART2_Init 2 */

	/* USER CODE END USART2_Init 2 */

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct =
	{ 0 };

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7 | GPIO_PIN_9, GPIO_PIN_SET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);

	/*Configure GPIO pin : B1_Pin */
	GPIO_InitStruct.Pin = B1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : LD2_Pin */
	GPIO_InitStruct.Pin = LD2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : PA7 PA9 */
	GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pin : PC7 */
	GPIO_InitStruct.Pin = GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pin : PA10 */
	GPIO_InitStruct.Pin = GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : PB3 PB4 PB5 */
	GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pin : PB6 */
	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
//port/pin array , 0-3 input , 4-7 output
GPIO_TypeDef *ButtonMatrixPort[8] =
{ GPIOA, GPIOB, GPIOB, GPIOB, GPIOA, GPIOC, GPIOB, GPIOA };

uint16_t ButtonMatrixPin[8] =
{ GPIO_PIN_10, GPIO_PIN_3, GPIO_PIN_5, GPIO_PIN_4, GPIO_PIN_9, GPIO_PIN_7,
GPIO_PIN_6, GPIO_PIN_7 };

uint8_t ButtonMatrixRow = 0;  //What  R Now
void ButtonMatrixUpdate()
{
	if (HAL_GetTick() - ButtonMatrixTimestamp >= 50)
	{
		ButtonMatrixTimestamp = HAL_GetTick();
		int i;
		for (i = 0; i < 4; i += 1)
		{ //0-3
			GPIO_PinState PinState = HAL_GPIO_ReadPin(ButtonMatrixPort[i],
					ButtonMatrixPin[i]);
			if (PinState == GPIO_PIN_RESET) // Button Press
			{
				ButtonMatrixState |= (uint16_t) 1 << (i + ButtonMatrixRow * 4);
			}
			else
			{
				ButtonMatrixState &= ~((uint16_t) 1 << (i + ButtonMatrixRow * 4));
			}
		}
		uint8_t NowOutputPin = ButtonMatrixRow + 4;
		//SET Rn
		HAL_GPIO_WritePin(ButtonMatrixPort[NowOutputPin],
				ButtonMatrixPin[NowOutputPin], GPIO_PIN_SET);
		// update New Row
		ButtonMatrixRow = (ButtonMatrixRow + 1) % 4;

		uint8_t NextOutputPin = ButtonMatrixRow + 4;
		//Reset Rn+1
		HAL_GPIO_WritePin(ButtonMatrixPort[NextOutputPin],
				ButtonMatrixPin[NextOutputPin], GPIO_PIN_RESET);

	}
}
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

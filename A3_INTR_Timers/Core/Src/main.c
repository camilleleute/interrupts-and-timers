/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
void SystemClock_Config(void);
// TIM2 configuration
void TIM2_Init(void){
	// enable TIM2 clock
	RCC->APB1ENR1 |= (RCC_APB1ENR1_TIM2EN);
	// set TIM2 as an up counter
	TIM2->CR1 &= ~TIM_CR1_DIR;
	// 4MHz clock for 5kHz
	TIM2->ARR =  0xFFFFFFFF;
	// set count for CCR1
	TIM2->CCR1 = 399;
	// enable update event interrupt in TIM2 and CCR1 interrupts
	TIM2->DIER |=(TIM_DIER_UIE | TIM_DIER_CC1IE);
	// clear interrupt status register for update event and CCR1
	TIM2->SR&= ~(TIM_SR_UIF | TIM_SR_CC1IF);
	// start timer
	TIM2->CR1 |= TIM_CR1_CEN;
	// disable preload
	//TIM2->CCMR1	&= ~TIM_CCMR1_OC1PE;
	// enable TIM2 interrupts
	NVIC->ISER[0] = (1 <<(TIM2_IRQn & 0x1F));
}
// configure
void Pin_Init(void){
	// turn clock on for GPIOC
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
	// configure other registers to correct mode
	GPIOC->MODER &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE1);
	GPIOC->MODER |= (1 << GPIO_MODER_MODE0_Pos | 1 << GPIO_MODER_MODE1_Pos);
	GPIOC->OTYPER &= ~(GPIO_OTYPER_OT0 | GPIO_OTYPER_OT1);
	GPIOC->OSPEEDR |= (GPIO_OSPEEDR_OSPEED0 | GPIO_OSPEEDR_OSPEED1);
	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD0 | GPIO_PUPDR_PUPD1);
}
void clock_output(void){
	// Enable MCO, select MSI (4 MHz source)
	RCC->CFGR = ((RCC->CFGR & ~(RCC_CFGR_MCOSEL)) | (RCC_CFGR_MCOSEL_0));
	// Configure MCO output on PA8
	RCC->AHB2ENR   |=  (RCC_AHB2ENR_GPIOAEN);
	GPIOA->MODER   &= ~(GPIO_MODER_MODE8);		// alternate function mode
	GPIOA->MODER   |=  (2 << GPIO_MODER_MODE8_Pos);
	GPIOA->OTYPER  &= ~(GPIO_OTYPER_OT8);		// Push-pull output
	GPIOA->PUPDR   &= ~(GPIO_PUPDR_PUPD8);		// no resistor
	GPIOA->OSPEEDR |=  (GPIO_OSPEEDR_OSPEED8);		// high speed
	GPIOA->AFR[1]  &= ~(GPIO_AFRH_AFSEL8);		// select MCO function
}
// TIM2 ISR
void TIM2_IRQHandler(void){
	// ISR tracking pin HIGH (entering)
	GPIOC->ODR |= GPIO_PIN_1;
	// ARR max reached
//	if (TIM2->SR & TIM_SR_UIF) {
//		// turn pin high
//		GPIOC->ODR |= GPIO_PIN_0;
//		// clear update event interrupt flag
//		TIM2->SR &= ~(TIM_SR_UIF);
//		// reset CCR1
//		TIM2->CCR1 = 399;
//	}
	// CCR1 reached
	if (TIM2->SR & TIM_SR_CC1IF) {
		// turn pin low
		GPIOC->ODR ^= GPIO_PIN_0;
		// clear update event interrupt flag
		TIM2->SR &= ~(TIM_SR_CC1IF);
		// incrementing CCR for 50% 5kHz square wave
		TIM2->CCR1 += 399;
	}
	// ISR tracking pin low (exiting)
	GPIOC->ODR &= ~GPIO_PIN_1;
}
int main(void)
{
	// initializations
	HAL_Init();
	clock_output();
	SystemClock_Config();
	Pin_Init();
	__enable_irq();
	TIM2_Init();
 while (1)
 {
 }
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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
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

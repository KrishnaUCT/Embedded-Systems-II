/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include <stdint.h>
#include "stm32f0xx.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define MAX_ITER 100
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//TODO: Define and initialise the global varibales required
int image_widths[] = {128, 160, 192, 224, 256};
int image_heights[] = {128, 160, 192, 224, 256};
int MAX_ITERS = 100;

#define SYSTICK_LOAD_VAL 0x00FFFFFF

typedef struct {
	int image_size;
	uint64_t wall_clock_time_ms;
	uint64_t checksum;
} benchmark_results;


int completed_executions = 0;
uint32_t start_time = 0;
uint32_t end_time = 0;
uint32_t start_cycles = 0;
uint32_t end_cycles = 0;
benchmark_results results[5];
volatile int currentIteration = 0;
/*
  start_time
  end_time
  execution_time
  checksum: should be uint64_t
  initial width and height maybe or you might opt for an array??
*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
uint64_t calculate_mandelbrot_fixed_point_arithmetic(int width, int height, int max_iterations);
uint64_t calculate_mandelbrot_fixed_point_arithmetic_split(int width, int height, int max_iterations);
uint64_t calculate_mandelbrot_double(int width, int height, int max_iterations);

uint32_t get_cycle_count();
float calculate_throughput(int width, int height, uint32_t time_ms);
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
  /* USER CODE BEGIN 2 */
  //TODO: Turn on LED 0 to signify the start of the operation
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);

  for (int i = 0; i < 5; i++){

	  int width = image_widths[i];
	  int height = image_heights[i];

	  // memory check
	  uint32_t required_memory = width * height * sizeof(int32_t) * 2;

	  //TODO: Record the start time
	  start_time = HAL_GetTick();
	  start_cycles = get_cycle_count();


	  //TODO: Call the Mandelbrot Function and store the output in the checksum variable defined initially
	  //uint64_t checksum = calculate_mandelbrot_double(width, height, MAX_ITER);
	  uint64_t checksum  = calculate_mandelbrot_fixed_point_arithmetic(width, height, MAX_ITERS);

	  //TODO: Record the end time
	  end_cycles = get_cycle_count();
	  end_time = HAL_GetTick();

	  uint32_t wall_clock_time = (uint32_t)(end_time - start_time);

	  results[i].image_size = image_widths[i];
	  results[i].wall_clock_time_ms = wall_clock_time;
	  results[i].checksum = checksum;


	  //TODO: Calculate the execution time


	  //TODO: Turn on LED 1 to signify the end of the operation
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);

	  //TODO: Hold the LEDs on for a 1s delay
	  HAL_Delay(1000);

	  //TODO: Turn off the LEDs
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);

  }

  //TODO: Turn off all LEDs
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    /* USER CODE BEGIN 3 */
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
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
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB0 PB1 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */


uint32_t get_cycle_count(void){
	return (SYSTICK_LOAD_VAL - SysTick->VAL) + (HAL_GetTick()*SYSTICK_LOAD_VAL);
}

float calculate_throughput(int width, int height, uint32_t time_ms){
	if (time_ms == 0){
		// division by zero
		return 0.0f;
	}
	return ((float)(width*height))/((float)(time_ms)/1000.0f);
}

//TODO: Mandelbroat using variable type integers and fixed point arithmetic
uint64_t calculate_mandelbrot_fixed_point_arithmetic(int width, int height, int max_iterations){
    uint64_t mandelbrot_sum = 0;

    //TODO: Complete the function implementation
    const int32_t SCALE = 1000000;
    const int32_t ESCAPE = 4 * SCALE;

    for (int y = 0; y < height; y++){
    	for (int x = 0; x < width; x++){
    		int32_t x0 = (int32_t)(((int64_t)x * (int64_t)(3.5f * SCALE)) / width - (int64_t)(2.5f * SCALE));
    		int32_t y0 = (int32_t)(((int64_t)y * (int64_t)(2.0f * SCALE)) / width - (int64_t)(1.0f * SCALE));

    		int32_t xi = 0;
    		int32_t yi = 0;
    		int iteration = 0;

    		while (iteration < max_iterations) {
    			int32_t xi2 = (int32_t) (((int64_t)xi*(int64_t)xi) >> 16);
    			int32_t yi2 = (int32_t) (((int64_t)yi*(int64_t)yi) >> 16);

    			if (xi2 + yi2 > ESCAPE){
    				break;
    			}

    			int32_t tmp = xi2 - yi2 + x0;
    			yi = (int32_t)((((int64_t)xi*yi) << 1) >> 16) + y0;
    			xi = tmp;

    			iteration++;
    		}
    		mandelbrot_sum += iteration;

    	}
    }

    return mandelbrot_sum;

}

uint64_t calculate_mandelbrot_fixed_point_arithmetic_split(int width, int height, int max_iterations){
    uint64_t mandelbrot_sum = 0;

    int split_width = 128;
    int split_height = 128;

    //TODO: Complete the function implementation
    const int32_t SCALE = 1 << 16;
    const int32_t ESCAPE = 4 * SCALE;

    for (int tile_y = 0; tile_y < height; tile_y += split_width){
    	for (int tile_x = 0; tile_x < width; tile_x += split_height){

    		int actual_tile_width = (tile_x + split_width > width) ? width - tile_x : split_width;
    		int actual_tile_height = (tile_y + split_height > height) ? height - tile_y : split_height;

			for (int y = 0; y < actual_tile_height; y++){
				for (int x = 0; x < actual_tile_width; x++){

					int actualX = tile_x + x;
					int actualY = tile_y + y;

					int32_t x0 = (int32_t)(((int64_t)actualX * (int64_t)(3.5f * SCALE)) / width - (int64_t)(2.5f * SCALE));
					int32_t y0 = (int32_t)(((int64_t)actualY * (int64_t)(2.0f * SCALE)) / height - (int64_t)(1.0f * SCALE));

					int32_t xi = 0;
					int32_t yi = 0;
					int iteration = 0;

					while (iteration < max_iterations) {
						int32_t xi2 = (int32_t) (((int64_t)xi*(int64_t)xi) >> 16);
						int32_t yi2 = (int32_t) (((int64_t)yi*(int64_t)yi) >> 16);

						if (xi2 + yi2 > ESCAPE){
							break;
						}

						int32_t tmp = xi2 - yi2 + x0;
						yi = (int32_t)((((int64_t)xi*yi) << 1) >> 16) + y0;
						xi = tmp;

						iteration++;
					}
					mandelbrot_sum += iteration;

				}
			}
    	}
    }

    return mandelbrot_sum;

}

//TODO: Mandelbroat using variable type double
uint64_t calculate_mandelbrot_double(int width, int height, int max_iterations){
    uint64_t mandelbrot_sum = 0;
    //TODO: Complete the function implementation

	for (int y = 0; y < height; y++){

		for (int x = 0; x < width; x++){

			double x0 = (((double)x)/(width)) * 3.5 - 2.5;
			double y0 = (((double)y)/(height)) * 2.0 - 1.0;


			double xi = 0.0;
			double yi = 0.0;
			int iteration = 0;

			while ( (iteration < max_iterations) && ((xi*xi + yi*yi) <= 4.0) ) {
				double temp = (xi*xi) - (yi*yi) + x0;
				yi = (2 * xi * yi) + y0;
				xi = temp;
				iteration += 1;
			}

			mandelbrot_sum += iteration;

		}
	}

	return mandelbrot_sum;
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

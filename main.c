/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oled.h"
#include "max7219.h"
#include <stdio.h>
#include <string.h>

#include"dog_animal.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TEMP_THRESHOLD   280
#define LIGHT_THRESHOLD  2000
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
I2C_HandleTypeDef hi2c1;
SPI_HandleTypeDef hspi1;
UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
uint8_t rx_data;
uint8_t page = 1;
uint8_t page_dirty = 1;

uint8_t single_light_mode;
uint8_t temp_high;
uint8_t light_high;

uint8_t matrix_animating = 0;
uint32_t matrix_anim_tick = 0;

uint32_t adc_light;
uint32_t adc_temp;

uint32_t last_light = 9999;
uint32_t last_temp = 9999;
uint32_t temp_c;

uint8_t alarm_enable = 1;


uint8_t gif_frame = 0;
uint32_t gif_tick = 0;


int16_t scroll_x = 128;


int16_t cheer_x = 128;
uint32_t cheer_tick = 0;



uint16_t year = 2026;
uint8_t month = 5;
uint8_t day = 3;


uint8_t hour = 0;
uint8_t minute = 0;
uint8_t second = 0;
uint8_t time_valid = 0;

uint32_t last_time_tick = 0;

char uart_buf[20];
uint8_t uart_idx = 0;

char uart_line[20];
     


char buf[20];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_ADC1_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */
uint32_t Read_ADC_Channel(uint32_t channel);

void Parse_Time_Command(char *cmd);
void Update_Time_Form_Tick(void);

void Parse_uart_Command(char*cmd);
void BT_SendString(char*str);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/*void Parse_Time_Command(char *cmd)
{
    unsigned int h, m, s;

    if (sscanf(cmd, "T%u:%u:%u", &h, &m, &s) == 3)
    {
        if (h < 24 && m < 60 && s < 60)
        {
            hour = h;
            minute = m;
            second = s;
            time_valid = 1;
            last_time_tick = HAL_GetTick();
        }
    }
}*/


uint32_t Read_ADC_Channel(uint32_t channel)
{
    ADC_ChannelConfTypeDef sConfig = {0};
    uint32_t value = 0;
    uint8_t i;

    sConfig.Channel = channel;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_71CYCLES_5;
		
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);

    for (i = 0; i < 3; i++)
    {
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1, 10);
        value += HAL_ADC_GetValue(&hadc1);
        HAL_ADC_Stop(&hadc1);
    }

    return value / 3;
}

void Parse_Uart_Command(char *cmd)
{
    unsigned int h, m, s;

    if (strcmp(cmd, "1") == 0)
    {
        page = 1;
        page_dirty = 1;
        matrix_animating = 1;
        matrix_anim_tick = HAL_GetTick();
        MAX7219_ShowBlink();
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    }
    else if (strcmp(cmd, "2") == 0)
    {
        page = 2;
        page_dirty = 1;
        matrix_animating = 1;
        matrix_anim_tick = HAL_GetTick();
        MAX7219_ShowBlink();
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    }
    else if (strcmp(cmd, "3") == 0)
    {
        page = 3;
        page_dirty = 1;
        matrix_animating = 1;
        matrix_anim_tick = HAL_GetTick();
        MAX7219_ShowBlink();
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    }
		
		else if (strcmp(cmd, "4") == 0)
		{
				page = 4;
				page_dirty = 1;
				matrix_animating = 1;
				matrix_anim_tick = HAL_GetTick();
				MAX7219_ShowBlink();
				HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		}

		else if (strcmp(cmd, "D") == 0)
    {
        sprintf(buf, "Temp:%2u.%1uC\r\n", (unsigned int)(temp_c / 10), (unsigned int)(temp_c % 10));
        BT_SendString(buf);
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    }
    else if (strcmp(cmd, "L") == 0)
    {
        sprintf(buf, "Light:%4u\r\n", (unsigned int)adc_light);
        BT_SendString(buf);
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    }
    else if (strcmp(cmd, "A") == 0)
    {
        alarm_enable = 1;
        BT_SendString("Alarm ON\r\n");
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    }
    else if (strcmp(cmd, "B") == 0)
    {
        alarm_enable = 0;
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);   // ????????,SET=??
        BT_SendString("Alarm OFF\r\n");
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    }
		else if (strcmp(cmd, "GJQ200504") == 0)
		{
				page = 5;
				page_dirty = 1;
				gif_frame = 0;
				gif_tick = HAL_GetTick();
			
				cheer_x = 128;
				cheer_tick = HAL_GetTick();/*gundongxianshi*/

				HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		}


    else if (sscanf(cmd, "T%u:%u:%u", &h, &m, &s) == 3)
    {
        if (h < 24 && m < 60 && s < 60)
        {
            hour = h;
            minute = m;
            second = s;
            time_valid = 1;
            last_time_tick = HAL_GetTick();
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        }
    }
}

void Update_Time_From_Tick(void)
{
    if (time_valid)
    {
        while (HAL_GetTick() - last_time_tick >= 1000)
        {
            last_time_tick += 1000;
            second++;

            if (second >= 60)
            {
                second = 0;
                minute++;
            }
            if (minute >= 60)
            {
                minute = 0;
                hour++;
            }
            if (hour >= 24)
            {
                hour = 0;
            }
        }
    }
}


/*
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

        if (rx_data == '1')
        {
            page = 1;
            page_dirty = 1;
            matrix_animating = 1;
            matrix_anim_tick = HAL_GetTick();
            MAX7219_ShowBlink();
        }
        else if (rx_data == '2')
        {
            page = 2;
            page_dirty = 1;
            matrix_animating = 1;
            matrix_anim_tick = HAL_GetTick();
            MAX7219_ShowBlink();
        }
        else if (rx_data == '3')
        {
            page = 3;
            page_dirty = 1;
            matrix_animating = 1;
            matrix_anim_tick = HAL_GetTick();
            MAX7219_ShowBlink();
        }

        HAL_UART_Receive_IT(&huart1, &rx_data, 1);
    }
}*/


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        if ((rx_data == '1' || rx_data == '2' || rx_data == '3' ||
             rx_data == 'D' || rx_data == 'L' || rx_data == 'A' || rx_data == 'B') && uart_idx == 0)
        {
            uart_line[0] = rx_data;
            uart_line[1] = '\0';
            Parse_Uart_Command(uart_line);
        }
        else if (rx_data == '\r' || rx_data == '\n')
        {
            if (uart_idx > 0)
            {
                uart_line[uart_idx] = '\0';
                Parse_Uart_Command(uart_line);
                uart_idx = 0;
            }
        }
        else
        {
            if (uart_idx < sizeof(uart_line) - 1)
            {
                uart_line[uart_idx++] = rx_data;
            }
            else
            {
                uart_idx = 0;
            }
        }

        HAL_UART_Receive_IT(&huart1, &rx_data, 1);
    }
}


void BT_SendString(char *str)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)str, strlen(str), 100);
}



/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  MX_ADC1_Init();
  MX_SPI1_Init();

  /* USER CODE BEGIN 2 */
  HAL_ADCEx_Calibration_Start(&hadc1);
  HAL_UART_Receive_IT(&huart1, &rx_data, 1);

  MAX7219_Init();
  MAX7219_ShowSmile();

  OLED_Init();
  OLED_Clear();
  OLED_ShowString(1, 1, "Hello");
  OLED_ShowString(1, 2, "Welcome");
  OLED_ShowString(1, 3, "Hello,Welcome");
  HAL_Delay(500);
  OLED_Clear();
  /* USER CODE END 2 */

  while (1)
  {
    /* USER CODE BEGIN 3 */
		
		if (time_valid && (HAL_GetTick() - last_time_tick >= 1000))
		{
				last_time_tick += 1000;

				second++;
				if (second >= 60)
				{
						second = 0;
						minute++;
				}
				if (minute >= 60)
				{
						minute = 0;
						hour++;
				}
				if (hour >= 24)
				{
						hour = 0;
				}
		}

		
		
    adc_temp  = Read_ADC_Channel(ADC_CHANNEL_2);
    adc_light = Read_ADC_Channel(ADC_CHANNEL_1);
		
		Update_Time_From_Tick();


    temp_c = (adc_temp * 3300UL) / 4095UL + 10;
		
		
		


    temp_high = (temp_c >= TEMP_THRESHOLD);
    light_high = (adc_light <= LIGHT_THRESHOLD);
    single_light_mode = !light_high;
		
		
		/*adc_temp  = Read_ADC_Channel(ADC_CHANNEL_2);
		adc_light = Read_ADC_Channel(ADC_CHANNEL_1);

		
		temp_x10 = (adc_temp * 3300UL) / 4095UL + 20;

		
		if (temp_filtered_x10 == 0)
		{
				temp_filtered_x10 = temp_x10;
		}
		else
		{
				temp_filtered_x10 = (temp_filtered_x10 * 3 + temp_x10) / 4;
		}

		
		if (temp_display_x10 == 0)
		{
				temp_display_x10 = temp_filtered_x10;
		}
		else if (temp_display_x10 < temp_filtered_x10)
		{
				temp_display_x10++;
		}
		else if (temp_display_x10 > temp_filtered_x10)
		{
				temp_display_x10--;
		}

		
		temp_int = (temp_display_x10 + 5) / 10;

		temp_high = (temp_int >= TEMP_THRESHOLD);
		light_high = (adc_light <= LIGHT_THRESHOLD);
		single_light_mode = !light_high;*/
		
		

		

    if (alarm_enable && (!single_light_mode) && temp_high)
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);
    }
    else
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
    }
		

	
		

    if (matrix_animating)
    {
        if (HAL_GetTick() - matrix_anim_tick >= 500)
        {
            matrix_animating = 0;
        }
        else
        {
            MAX7219_ShowBlink();
        }
    }

    if (!matrix_animating)
    {
        if (alarm_enable && (!single_light_mode) && temp_high)
        {
            MAX7219_ShowAlarm();
        }
        else if (single_light_mode)
        {
            MAX7219_ShowSmile();
        }
        else
        {
            MAX7219_ShowNeutral();
        }
    }

    if (page == 1)
    {
        if (page_dirty)
        {
           /* OLED_Clear();
            OLED_ShowString(1, 1, "Hello");
            OLED_ShowString(1, 2, "Welcome");
            OLED_ShowString(1, 3, "Hello,Welcome");*/
					
						OLED_Clear();
						OLED_ShowStringCenter(1, "Hello,Welcome");
						OLED_ShowChineseCenter16(3, 0, 8);
            page_dirty = 0;
        }
				
				OLED_ShowStringCenter(6, "2026/05/04");
				
				if (time_valid)
				{
						sprintf(buf, "%02d:%02d:%02d", hour, minute, second);
						OLED_ShowStringCenter(7, buf);
				}
				else
				{
					OLED_ShowStringCenter(7,"--:--:--");
				}
						
    }
    else if (page == 2)
    {
        if (page_dirty)
        {
            OLED_Clear();
            last_temp = 9999;
            page_dirty = 0;
        }

       if (temp_c != last_temp)
        {
            last_temp = temp_c;
            sprintf(buf, "Temp:%2lu,%1luC", temp_c / 10, temp_c % 10);


            OLED_ShowString(1, 1, buf);
        }
				/*if (temp_int != last_temp)
				{
						last_temp = temp_int;
						sprintf(buf, "Temp:%2luC", temp_int);
						OLED_ShowString(1, 1, buf);
				}*/

				
				
    }
    else if (page == 3)
    {
        if (page_dirty)
        {
            OLED_Clear();
            last_light = 9999;
            page_dirty = 0;
        }

        if (adc_light != last_light)
        {
            last_light = adc_light;
            sprintf(buf, "Light:%4lu", adc_light);
            OLED_ShowString(1, 1, buf);
        }
    }
		
		else if (page == 4)
		{
				if (page_dirty)
				{
						OLED_Clear();
						OLED_ShowChineseCenter16(3, 8, 4);   
						page_dirty = 0;
				}
		}
		
		else if (page == 5)
		{
				if (page_dirty)
				{
						OLED_Clear();
						gif_frame = 0;
						gif_tick = HAL_GetTick();
						page_dirty = 0;
				}

				if (HAL_GetTick() - gif_tick >= 100)
				{
						gif_tick = HAL_GetTick();
						gif_frame++;
						if (gif_frame >= DOG_FRAME_COUNT)
						{
								gif_frame = 0;
						}
				}
			/*	if (HAL_GetTick() - cheer_tick >= 60)
				{
						cheer_tick = HAL_GetTick();
						cheer_x--;

						if (cheer_x < -144)
						{
								cheer_x = 128;
						}
				}*/

				
				if (HAL_GetTick() - cheer_tick >= 10)
				{
						cheer_tick = HAL_GetTick();
						cheer_x -= 4;

						if (cheer_x < -144)
						{
								cheer_x = 128;
						}

						OLED_ClearArea(1, 7, 128, 16);

						OLED_ShowChinese16Clip(cheer_x +   0, 7, 12);
						OLED_ShowChinese16Clip(cheer_x +  16, 7, 13);
						OLED_ShowChinese16Clip(cheer_x +  32, 7, 14);
						OLED_ShowChinese16Clip(cheer_x +  48, 7, 15);
						OLED_ShowChinese16Clip(cheer_x +  64, 7, 16);
						OLED_ShowChinese16Clip(cheer_x +  80, 7, 17);
						OLED_ShowChinese16Clip(cheer_x +  96, 7, 18);
						OLED_ShowChinese16Clip(cheer_x + 112, 7, 19);
						OLED_ShowChinese16Clip(cheer_x + 128, 7, 20);
				}/*wenzigundong*/



				/*OLED_ClearArea(33, 1, DOG_BMP_W, DOG_BMP_H);*/
				OLED_ShowBMP(33, 1, DOG_BMP_W, DOG_BMP_H, dog_frames[gif_frame]);
		}



    HAL_Delay(50);
    /* USER CODE END 3 */
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                              | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{
  ADC_ChannelConfTypeDef sConfig = {0};

  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
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

  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);

  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdbool.h"
#include "stdlib.h"
#include "analog.h"
#include "rgb.h"
#include "keyboard.h"
#include "keyboard_tree.h"
#include "lefl.h"
#include "flash_address.h"
#include "MB85RC16.h"
#include "communication.h"
#include "usbd_custom_hid_if.h"
#include "keyboard_tree.h"

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

/* USER CODE BEGIN PV */

uint8_t count=0;
uint8_t cmd_buffer;
bool sendreport=true;
bool sendreport_ready=false;
uint8_t spi_tx[8]="hello";
uint8_t spi_rx[8];
uint8_t spi_tx1[8]="world";
uint8_t spi_rx1[8];
uint8_t spi_flag;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

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
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_TIM7_Init();
  MX_TIM8_Init();
  MX_USART1_UART_Init();
  MX_USB_DEVICE_Init();
  MX_TIM6_Init();
  MX_TIM3_Init();
  MX_SPI2_Init();
  MX_I2C1_Init();
  MX_SPI3_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */

  lefl_bit_array_init(&Keyboard_KeyArray, (size_t*)(Keyboard_ReportBuffer+2), 168);
  Keyboard_Init();
  Keyboard_Tree_Init();
  RGB_Init();
  Analog_Recovery();
  RGB_Recovery();

  HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
  RGB_Start();
  //HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);

  //Analog_Init();
  HAL_GPIO_WritePin(SPI2_CS1_GPIO_Port, SPI2_CS1_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(SPI2_CS2_GPIO_Port, SPI2_CS2_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(SPI2_CS3_GPIO_Port, SPI2_CS3_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(SPI2_CS4_GPIO_Port, SPI2_CS4_Pin, GPIO_PIN_SET);
  RGB_Flash();
  RGB_TurnOff();
  HAL_TIM_Base_Start_IT(&htim6);
  HAL_TIM_Base_Start_IT(&htim7);
  HAL_TIM_Encoder_Start_IT(&htim3,TIM_CHANNEL_1);
  HAL_TIM_Encoder_Start_IT(&htim8,TIM_CHANNEL_1);
  MX_USART1_UART_Init();
  Communication_Enable(&huart1,USART1_RX_Buffer,BUFFER_LENGTH);

  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)ADC_Buffer, ADVANCED_KEY_NUM);
  //HAL_SPI_TransmitReceive_IT(&hspi3, spi_tx1, spi_rx1, 1);
  //HAL_SPI_Receive_IT(&hspi2, spi_rx, 8);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      HAL_Delay(1);

      switch (cmd_buffer)
      {
          case CMD_CALIBRATION_START:
              memset(Keyboard_ReportBuffer,0,sizeof(Keyboard_ReportBuffer)/sizeof(uint8_t));
              HAL_TIM_Base_Stop_IT(&htim7);
              HAL_ADC_Stop_DMA(&hadc1);
              Analog_Init();
              HAL_TIM_Base_Start_IT(&htim7);
              HAL_ADC_Start_DMA(&hadc1, (uint32_t*)ADC_Buffer, ADVANCED_KEY_NUM);
              break;
          case CMD_ANALOG_READ:
              Analog_Recovery();
              break;
          case CMD_ID_READ:
              Keyboard_ID_Recovery();
              break;
          case CMD_RGB_READ:
              RGB_Recovery();
              break;
          case CMD_KEYBOARD_TREE_SCAN:
              Keyboard_Tree_BaseStatus=KEYBOARD_TNODE_DETECT;
              break;
          default:
              cmd_buffer=CMD_NULL;
              break;
      }

      switch(Keyboard_Tree_BaseStatus)
      {
          case KEYBOARD_TNODE_CONNECTED:
              break;
          case KEYBOARD_TNODE_DETECT:
              Keyboard_Tree_Detect();
              break;
          case KEYBOARD_TNODE_READY:
              break;
          case KEYBOARD_TNODE_ACTIVE:
          default:
              break;
      }
    /* USER CODE END WHILE */

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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB|RCC_PERIPHCLK_USART1
                              |RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_TIM8
                              |RCC_PERIPHCLK_ADC12;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Adc12ClockSelection = RCC_ADC12PLLCLK_DIV1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_SYSCLK;
  PeriphClkInit.USBClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  PeriphClkInit.Tim8ClockSelection = RCC_TIM8CLK_HCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance==TIM7)
  {

    Keyboard_Scan();
    switch(Keyboard_Tree_BaseStatus)
    {
        case KEYBOARD_TNODE_CONNECTED:
            break;
        case KEYBOARD_TNODE_DETECT:
            break;
        case KEYBOARD_TNODE_READY:
            break;
        case KEYBOARD_TNODE_ACTIVE:
            Keyboard_Tree_Scan();
            Keyboard_Tree_Report();
            break;
        default:
            break;
    }
    Analog_Average();
    Analog_Check();
    if(!HAL_GPIO_ReadPin(MENU_GPIO_Port, MENU_Pin))
        Keyboard_SendReport();
    Communication_Pack();
    if(eeprom_buzy)
    {
        Communication_Add8(USART1,PROTOCOL_CMD,CMD_EEPROM_DISABLE);
    }
    else
    {
        Communication_Add8(USART1,PROTOCOL_CMD,CMD_EEPROM_ENABLE);
    }
    Communication_USART1_Transmit();

    RGB_Update();
    Analog_Clean();

  }

  if (htim->Instance==TIM6)
  {

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

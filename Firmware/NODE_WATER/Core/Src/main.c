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
#include "i2c-lcd.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "DS3231.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

char a[6], b[6], c[6];// a cho flowrate, b cho today, c cho thismonth: Mảng này được sử dụng để chuyển từ float sang char để hiển thị lên LCD
char Rx_data[15], Tx_data[30]; // Hai mảng này dùng để lưu giá trị nhận được từ module Zigbee


volatile int  Pulse_Count; // Khai báo volatile do biến này sẽ thay đổi liên tục trong quá trình hệ thống hoạt động
float  Liter_per_minute = 0, flowrate = 0;
unsigned long Current_Time, Loop_Time;
float calibrationFactor = 6.5;
float today = 0, thismonth = 0;
int m = 1, i =0; // m = month để coi cosumed từng tháng. i là biến trạng thái ban đầu cho chức năng coi cosumed từng tháng
int state = 0;
int gtlen;
int gtmenu;
int gtxuong;
int demtong = 0;


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
I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;

TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_I2C2_Init(void);
void resetTimerCounter(TIM_HandleTypeDef *htim);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void resetTimerCounter(TIM_HandleTypeDef *htim)
{
    htim->Instance->CNT = 0; //
}

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
  MX_I2C1_Init();
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  MX_I2C2_Init();

  	lcd_init();
    state = EEPROM_ReadFloat(0x0010);
    if(state == 1){
    Set_Time(00, 25, 21, 1, 2, 6, 24); // Cài đặt thời gian ban đầu second, minute, hour, date, day, month,year
    state = 0;
    EEPROM_WriteFloat(0x0010,state);
    }

	for(int n = 1; n<13; n ++){
	   addr = 0x0020 + (n-1)*4;
	   EEPROM_WriteFloat(addr,n*2);
	 }

    lcd_goto_XY(2,0);
    lcd_send_string("RATE= ");
    lcd_goto_XY(3,0);
    lcd_send_string("TODAY= ");
    lcd_goto_XY(4,0);
    lcd_send_string("MONTH= ");
    lcd_goto_XY(1,12);
    lcd_send_string("T= ");
    Get_Time(); // Lấy giá trị thời gian ngày, tháng, năm từ DS3231
    Time_LCD(); // Hàm hiển thị thời gian lên LCD
    HAL_UART_Receive_IT(&huart1, (uint8_t*)Rx_data, 1); // Kích hoạt ngắt nhận UART khi nhận dc 1 byte data
    HAL_TIM_Base_Start(&htim1); // Khởi động Timer 1 bắt đầu đếm
    Current_Time = HAL_GetTick(); // Lấy giá trị thời gian hiện tại
    Loop_Time = Current_Time;

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1){

      if(demtong == 0){

          gtxuong = HAL_GPIO_ReadPin(GPIOA, XUONG_Pin);
          if(gtxuong == 0){
          while(!gtxuong == 0);
    	  EEPROM_WriteFloat(0x0001,0);
    	  EEPROM_WriteFloat(0x0005,0);
          }

          gtmenu = HAL_GPIO_ReadPin(GPIOA, CHON_Pin);
          if(gtmenu == 0){
          while(!gtmenu == 0);
          	 demtong = 1;
          	 HAL_Delay(200);
          }
      /* USER CODE END WHILE */
  	  Get_Time(); // Đọc giá trị ngày, tháng, năm từ DS3231
  	  if(dayofmonth_t != dayofmonth || month_t != month || year_t != year){
  		  if(month != month){// Nếu qua tháng mới thì lưu giá trị và reset thismonth
  			  Save_Month(thismonth);
  			  thismonth = 0;
  		  }
  		  if(dayofmonth_t != dayofmonth){// Nếu qua ngày mới thì reset today
  			  today = 0;
  		  }
  		  if(year_t != year){// Nếu qua năm mới thì xóa tất cả giá trị lưu trữ trong từng tháng
  		    for(int n = 1; n<13; n ++){
  		    	addr = 0x0020 + (n-1)*4;
  		    	EEPROM_WriteFloat(addr,0);
  		    }
  		  }
  	  	Time_LCD();
  	  }

  	  force_temp_conv();
  	  TEMP = Get_Temp(); //Đọc giá trị nhiệt độ
  	  sprintf (buffer, "%.2f", TEMP);
  	  lcd_goto_XY(1, 15);
  	  lcd_send_string(buffer);

  	  today = EEPROM_ReadFloat(0x0001);
  	  thismonth = EEPROM_ReadFloat(0x0005);

  	  Current_Time = HAL_GetTick();
  	  Pulse_Count = __HAL_TIM_GET_COUNTER(&htim1); // Lấy giá trị đếm được từ thanh ghi CNT
  	  if(Current_Time >= (Loop_Time + 1000))
  	  {
  	  	flowrate = ((1000.0 / (HAL_GetTick() - Loop_Time)) * Pulse_Count) / calibrationFactor;
  	  	Loop_Time = Current_Time;
  	  	Liter_per_minute = (flowrate / 60);

  	  	today += Liter_per_minute;
  	  	thismonth = today;

  	  	sprintf (Tx_data, "P2P 0000 R%.2fV%.2fM%.2fS", flowrate, today, thismonth);// Đóng gói dữ liệu để truyền qua Zigbee

  	  	EEPROM_WriteFloat(0x0001,today);
  	  	EEPROM_WriteFloat(0x0005,thismonth);

  	  	sprintf(a,"%.2f",flowrate);
  	    sprintf(b,"%.2f",EEPROM_ReadFloat(0x0001));
  	  	sprintf(c,"%.2f",EEPROM_ReadFloat(0x0005));

  	  	lcd_goto_XY(2,8);
  	  	lcd_send_string("          ");
  	  	lcd_goto_XY(2,8);
  	  	lcd_send_string(a);
  	  	lcd_send_string(" L/M");

  	  	lcd_goto_XY(3,8);
  	  	lcd_send_string("           ");
  	  	lcd_goto_XY(3,8);
  	  	lcd_send_string(b);
  	  	lcd_send_string(" L");

  	  	lcd_goto_XY(4,7);
  	  	lcd_send_string("          ");
  	  	lcd_goto_XY(4,8);
  	  	lcd_send_string(c);
  	  	lcd_send_string(" L");

  	  	resetTimerCounter(&htim1);

  	  }}
      else if(demtong == 1){ // Chức năng xem thể tích nước đã sử dụng ở tưng tháng cụ thể
    	  if(i == 0){ // Mới đầu vô sẽ clear LCD và cài đặt các chế độ ban đầu
		  lcd_clear_display();
		  HAL_Delay(10);
		  lcd_goto_XY(1,0);
		  lcd_send_string("MONTH: ");
		  lcd_goto_XY(2,0);
		  lcd_send_string("CONSUMED: ");
		  addr = 0x0020 + (m-1)*4;
		  float consumed = EEPROM_ReadFloat(addr);
		  sprintf(a,"%d",m);
		  sprintf(b,"%.2f",consumed);
		  lcd_goto_XY(1,7);
	  	  lcd_send_string(a);
		  lcd_goto_XY(2,10);
	  	  lcd_send_string(b);
	  	  i = 1;
	  	  HAL_Delay(100);
    	  }
          gtlen = HAL_GPIO_ReadPin(GPIOA, LEN_Pin);
          gtxuong = HAL_GPIO_ReadPin(GPIOA, XUONG_Pin);
          gtmenu = HAL_GPIO_ReadPin(GPIOA, CHON_Pin);
          if(gtlen == 0)
          {
        	  if( m == 12 ){m = 1;}
        	  else m = m + 1;
          }
		  if(gtxuong == 0)
		  {
			  if(m == 1) {m = 12;}
			  else m = m - 1;
		  }

		  addr = 0x0020 + (m-1)*4;
		  float consumed = EEPROM_ReadFloat(addr);
		  sprintf(b,"%.2f",consumed);
	  	  sprintf(a,"%d",m);
		  lcd_goto_XY(1,7);
		  lcd_send_string("        ");
		  lcd_goto_XY(2,10);
		  lcd_send_string("        ");
	  	  lcd_goto_XY(1,7);
	  	  lcd_send_string(a);
		  lcd_goto_XY(2,10);
		  lcd_send_string(b);
		  lcd_send_string(" L");

          if(gtmenu == 0)
          {
        	  demtong = 0;
        	  i = 0;
        	  m = 1;
        	  lcd_clear_display();
        	  HAL_Delay(10);
        	  lcd_goto_XY(2,0);
        	  lcd_send_string("RATE= ");
        	  lcd_goto_XY(3,0);
        	  lcd_send_string("TODAY= ");
        	  lcd_goto_XY(4,0);
        	  lcd_send_string("MONTH= ");
        	  lcd_goto_XY(1,12);
        	  lcd_send_string("T= ");
        	  Get_Time();
        	  Time_LCD();
        	  Current_Time = HAL_GetTick();
        	  Loop_Time = Current_Time;
          }
          HAL_Delay(120);
      }
    }
  /* USER CODE END 3 */
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){ // hàm ngắt UART để nhận dữ liệu (Khi nhận dc dữ liệu từ chân RX thì thoát ra khỏi vòng while và nhảy vào hàm này)
	/*if(Rx_data[0] =='0'&& Rx_data[1] == '0' && Rx_data[2] == '1'){
		//HAL_UART_Transmit(&huart1, (uint8_t*) b, sizeof(b),1000);
		HAL_UART_Transmit(&huart1, (uint8_t*) Tx_data, sizeof(Tx_data),10);
		//memset(Rx_data, 0, sizeof(Rx_data));
	}
	HAL_UART_Receive_IT(&huart1, (uint8_t*)Rx_data, 3);*/
	if(Rx_data[0] =='R'){
		HAL_UART_Transmit(&huart1, (uint8_t*) Tx_data, sizeof(Tx_data),10);
		//memset(Rx_data, 0, sizeof(Rx_data));
	}
	else if(Rx_data[0] =='A'){ // Gửi giá trị thể tích nước đã tiêu thụ trong tháng 10
		addr = 0x0020 + (10-1)*4;
		float consumed = EEPROM_ReadFloat(addr);
		sprintf (Tx_data, "P2P 0000 L%.2fS", consumed);// Đóng gói dữ liệu để truyền qua Zigbee
		HAL_UART_Transmit(&huart1, (uint8_t*) Tx_data, sizeof(Tx_data),10);
		//memset(Rx_data, 0, sizeof(Rx_data));
	}
	else if(Rx_data[0] =='B'){ // Gửi giá trị thể tích nước đã tiêu thụ trong tháng 11
		addr = 0x0020 + (11-1)*4;
		float consumed = EEPROM_ReadFloat(addr);
		sprintf (Tx_data, "P2P 0000 L%.2fS", consumed);// Đóng gói dữ liệu để truyền qua Zigbee
		HAL_UART_Transmit(&huart1, (uint8_t*) Tx_data, sizeof(Tx_data),10);
		//memset(Rx_data, 0, sizeof(Rx_data));
	} else if(Rx_data[0] =='C'){ // Gửi giá trị thể tích nước đã tiêu thụ trong tháng 12
		addr = 0x0020 + (12-1)*4;
		float consumed = EEPROM_ReadFloat(addr);
		sprintf (Tx_data, "P2P 0000 L%.2fS", consumed);// Đóng gói dữ liệu để truyền qua Zigbee
		HAL_UART_Transmit(&huart1, (uint8_t*) Tx_data, sizeof(Tx_data),10);
		//memset(Rx_data, 0, sizeof(Rx_data));
	} else { // Gửi giá trị thể tích nước đã tiêu thụ từ tháng 1 - 9
		int value = Rx_data[0] - '0';
		addr = 0x0020 + (value-1)*4;
		float consumed = EEPROM_ReadFloat(addr);
		sprintf (Tx_data, "P2P 0000 L%.2fS", consumed);// Đóng gói dữ liệu để truyền qua Zigbee
		HAL_UART_Transmit(&huart1, (uint8_t*) Tx_data, sizeof(Tx_data),10);
	}
	HAL_UART_Receive_IT(&huart1, (uint8_t*)Rx_data, 1);
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
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

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
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
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_ETRMODE2;
  sClockSourceConfig.ClockPolarity = TIM_CLOCKPOLARITY_NONINVERTED;
  sClockSourceConfig.ClockPrescaler = TIM_CLOCKPRESCALER_DIV1;
  sClockSourceConfig.ClockFilter = 0;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 38400;
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
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pins : LEN_Pin CHON_Pin XUONG_Pin */
  GPIO_InitStruct.Pin = LEN_Pin|CHON_Pin|XUONG_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
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

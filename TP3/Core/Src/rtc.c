/*
 * rtc.c
 *
 *  Created on: Mar 17, 2023
 *      Author: RUFF CHIROSSEL FAURE
 */

#include "rtc.h"
/*This structure will allow us to modify the RTC register*/
RTC_HandleTypeDef hrtc;


/**
  * @brief RTC Initialization Function generated from the graphic interface
  * @param None
  * @retval None
  */
void RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */
//structure definition where time and date are stocked
  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */

  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    RTC_Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */

  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    RTC_Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_FRIDAY;
  sDate.Month = RTC_MONTH_MARCH;
  sDate.Date = 0x31;
  sDate.Year = 0x23;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    RTC_Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
void GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

void Read_Time_And_Date(RTC_DateTypeDef *sDate,RTC_TimeTypeDef *sTime, uint32_t Format){
	//Get time and set a error message if the time hasn't been retrieved correctly
	if (HAL_RTC_GetTime(&hrtc,sTime,Format)!= HAL_OK){
		RTC_Error_Handler();
	}
	if (HAL_RTC_GetDate(&hrtc,sDate,Format) != HAL_OK){
		RTC_Error_Handler();
	}
}

void Print_Time_And_Date_Usart(){
	//Defining a Time and Date struct to read using HAL function
	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};

	//Reading
	Read_Time_And_Date(&sDate,&sTime,RTC_FORMAT_BIN);

	//Converting the result in the structs to readable string
	//the input is a pointer to sTime and the result is Time_buffer defined by the funciton sTime_To_String
	char * time = sTime_To_String(&sTime);
	char * date = sDate_To_String(&sDate);
	//Writing to Usart
	serial_puts("The date time is ");
	serial_puts(date);
	serial_puts("The current time is ");
	serial_puts(time);

	//Freeing the buffer, buffer of the time and the date are defined with malloc that have to be freed
	free(date);
	free(time);


}

char* sTime_To_String(RTC_TimeTypeDef *sTime){
	char* hour_buffer = (char*) malloc(sizeof(char)*80);
	sprintf(hour_buffer, "%02d:%02d:%02d\r\n", sTime->Hours, sTime->Minutes, sTime->Seconds);  // format time string
	return hour_buffer;
}

char* sDate_To_String(RTC_DateTypeDef *sDate){
	char* date_buffer = (char*) malloc(sizeof(char)*80);
//day is a string of 3 characters + 1 null character for the end of the string
	char day[4];
	switch (sDate->WeekDay){
		case RTC_WEEKDAY_MONDAY :
			strcpy(day,"Mon");
			break;
		case RTC_WEEKDAY_TUESDAY :
			strcpy(day,"Tue");
			break;
		case RTC_WEEKDAY_WEDNESDAY :
			strcpy(day,"Wed");
			break;
		case RTC_WEEKDAY_THURSDAY :
			strcpy(day,"Thu");
			break;
		case RTC_WEEKDAY_FRIDAY :
			strcpy(day,"Fri");
			break;
		case RTC_WEEKDAY_SATURDAY :
			strcpy(day,"Sat");
			break;
		case RTC_WEEKDAY_SUNDAY :
			strcpy(day,"Sun");
			break;
		default :
			strcpy(day,"Mon");
	}
//%s for the format remplaced by the variable day, %02d remplace by date and month with a 0 is the date is 1 number
//20%02d is the year (2 numbers) preceded with "20"
	sprintf(date_buffer, "%s %02d-%02d-20%02d\r\n", day, sDate->Date, sDate->Month, sDate->Year);  // format date string
	return date_buffer;
}


void Set_RTC_All(RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime, uint32_t Format){
	if (HAL_RTC_SetTime(&hrtc,sTime,Format)!= HAL_OK){
		RTC_Error_Handler();
	}
	if (HAL_RTC_SetDate(&hrtc,sDate,Format) != HAL_OK){
		RTC_Error_Handler();
	}
}


void RTC_Error_Handler()
{
  __disable_irq();
  while (1)
  {
  }
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if (GPIO_Pin == GPIO_PIN_0)
	  {
		Print_Time_And_Date_Usart();
	  }
}


void init_calendar(uint8_t hour, uint8_t minute, uint8_t second, uint8_t weekday, uint8_t day, uint8_t month, uint8_t year )
{
	int temps=0;
	int datecomplete=0;
	int jour=0;
	int mois=0;


	temps = hour;
	temps=temps << 8;
	temps |= minute;
	temps=temps << 8;
	temps |= second;

	datecomplete = year << 16;
	jour = weekday << 12;
	datecomplete |= jour;
	mois = month << 8;
	datecomplete |= mois;
	datecomplete |= day;


	RCC->BDCR |= 0x1000; //Start the RTC
	RTC->WPR =0xCAU; //Disable the RTC registers write protection
	RTC->WPR =0x53U; //Disable the RTC registers write protection
	RTC->ISR =0x80; //Enter Initialization mode
	while(!(RTC->ISR & 0xBF)); //Wait for the confirmation of initialization mode (clock synchronization)
	RTC->PRER =0xFF; //Program the prescaler values if needed
	HAL_Delay(100); //permet de remettre la valeur de la clock à la valeur demandée
	RTC->PRER |=0x7F0000; //Program the prescaler values if needed
	RTC->TR =temps; //Load time values in the shadow registers
	RTC->DR =datecomplete; //Load date values in the shadow registers
	RTC->CR &=0xBF; //Configure the time format 24h
	RTC->ISR &=0xFFFFFF7F; //Exit Initialization mode
	RTC->WPR =0xFFU; //Enable the RTC Registers Write Protection
}

/*
 * rtc.c
 *
 *  Created on: Mar 17, 2023
 *      Author: hender
 */


/*
 * THis function intialise the RTC register
 */

#include "rtc.h"

RTC_HandleTypeDef hrtc;

void Init_RTC();


/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

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
    Error_Handler();
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
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_FRIDAY;
  sDate.Month = RTC_MONTH_MARCH;
  sDate.Date = 0x31;
  sDate.Year = 0x23;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

void Get_RTC_All(RTC_DateTypeDef *sDate,RTC_TimeTypeDef *sTime, uint32_t Format){
	if (HAL_RTC_GetTime(&hrtc,sTime,Format)!= HAL_OK){
			Error_Handler();
	}
	if (HAL_RTC_GetDate(&hrtc,sDate,Format) != HAL_OK){
		Error_Handler();
	}
}

void Print_RTC_To_UART(){
	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};
	Get_RTC_All(&sDate,&sTime,RTC_FORMAT_BIN);
	char * msg = RTC_To_Char(&sDate,&sTime);
	serial_puts(msg);


}

char* RTC_To_Char(RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime){
	char* hour_buffer = (char*) malloc(sizeof(char)*80);
	sprintf(hour_buffer, "%02d:%02d:%02d\r\n", sTime->Hours, sTime->Minutes, sTime->Seconds);  // format time string
	return hour_buffer;
}


void Set_RTC_All(RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime, uint32_t Format){
	if (HAL_RTC_SetTime(&hrtc,sTime,Format)!= 0){
		Error_Handler();
	}
	if (HAL_RTC_SetDate(&hrtc,sDate,Format) != 0){
		Error_Handler();
	}

}



/*
 * rtc.c
 *
 *  Created on: Mar 17, 2023
 *      Author: hender
 */

#include "rtc.h"
/*This structure will allow us to modify the RTC register*/
RTC_HandleTypeDef hrtc;


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

void Read_Time_And_Date(RTC_DateTypeDef *sDate,RTC_TimeTypeDef *sTime, uint32_t Format){
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
	char * time = sTime_To_String(&sTime);
	char * date = sDate_To_String(&sDate);
	//Writing tu Usart
	serial_puts("The date time is ");
	serial_puts(date);
	serial_puts("The current time is ");
	serial_puts(time);

	//Freeing the buffer
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


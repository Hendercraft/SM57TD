/*
 * rtc.h
 *
 *  Created on: Mar 17, 2023
 *      Authors: RUFF CHIROSSEL FAURE
 *
 */

#ifndef __RTC_H
#define __RTC_H

//bibliothèque pour le sprintf
#include "stdio.h"
//bibliothèque pour les espaces mémoires
#include "stdlib.h"
//bibliothèque pour passer en ascii
#include "usart2.h"
#include "stm32f4xx_hal.h"
//bibliothèque string copie
#include "string.h"
/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
void MX_RTC_Init(void);


/**
  * @brief Read the time and date from RTC and put the data in sTime and sDate
  * @param  sDate Pointer to Date structure
  * @param  sTime Pointer to Time structure
  * @param  Format Specifies the format of the entered parameters.
  *         This parameter can be one of the following values:
  *           @arg RTC_FORMAT_BIN: Binary data format
  *           @arg RTC_FORMAT_BCD: BCD data format
  * @retval None
  */
void Read_Time_And_Date(RTC_DateTypeDef *sDate,RTC_TimeTypeDef *sTime, uint32_t Format);


/**
  * @brief Read the actual time and date form the RTC and print it to USART
  * @param None
  * @retval None
  */
void Print_Time_And_Date_Usart();


/**
  * @brief Transform a RTC_TimeTypeDef to a printable string
  * @param sTime, a pointer to a structure containing time information (see stm32f4xx_hal_rtc.h)
  * @retval A string containing time info ready to be pushed to usart
  */
char* sTime_To_String(RTC_TimeTypeDef *sTime);

/**
  * @brief Transform a RTC_DateTypeDef to a printable string
  * @param sDate, a pointer to a structure containing date information (see stm32f4xx_hal_rtc.h)
  * @retval A string containing date info ready to be pushed to usart
  */
char* sDate_To_String(RTC_DateTypeDef *sDate);



/**
  * @brief  Sets RTC current time and date.
  * @param  sDate Pointer to Date structure
  * @param  sTime Pointer to Time structure
  * @param  Format Specifies the format of the entered parameters.
  *          This parameter can be one of the following values:
  *            @arg RTC_FORMAT_BIN: Binary data format
  *            @arg RTC_FORMAT_BCD: BCD data format
  * @note this function will require a error handler
  */
void Set_Time_And_Date(RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime, uint32_t Format);

/**
  * @brief  This ask the user to set the time and date
  * @retval None
  */
void Write_Time_Usart();

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void RTC_Error_Handler();



void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

#endif

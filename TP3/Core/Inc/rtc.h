/*
 * rtc.h
 *
 *  Created on: Mar 17, 2023
 *      Author: hender
 */

#ifndef __RTC_H
#define __RTC_H


#include "stdio.h"
#include "stdlib.h"
#include "main.h"

/*This structure will allow us to modify the RTC register*/

void MX_RTC_Init(void);


/*
 * This function convert the time and date given by the RTC struct into a string
 */
char* RTC_To_Char(RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime);



/*
 * This function fetch the time from the RTC clock and convert it to a string.
 * Then this string is sent by UART
 */
void Print_RTC_To_UART();

/*
 * THis function intialise the RTC register 
 */
void Init_RTC();

/*
 * This function fetch the time from the RTC register and store them into the structs given in argument
 */
void Get_RTC_All(RTC_DateTypeDef *sDate,RTC_TimeTypeDef *sTime, uint32_t Format);

/*
 * This function set the rtc clock using *sTime and *sDate
 *
 */
void Set_RTC(RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime, uint32_t Format);

/*
 * This function read the shadow register of RTC and return the current value
 *
 */
uint32_t Read_RTC();

#endif


/*
 * can.h
 *
 *  Created on: Apr 14, 2023
 *      Author: hender
 */

#ifndef INC_CAN_H_
#define INC_CAN_H_

#include "stm32f407xx.h"


typedef struct{
	uint16_t STDID;
	uint8_t RTR;
	uint8_t DLC;
	uint8_t data[8];
}CAN_frame;

void CAN_GPIO_Init();

void CAN_Config();

uint8_t CAN_sendFrame(CAN_frame CAN_mess);

#endif /* INC_CAN_H_ */

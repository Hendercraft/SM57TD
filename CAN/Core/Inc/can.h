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
	uint32_t ID;
	uint8_t IDE;
	uint8_t RTR;
	uint8_t DLC;
	uint8_t data[8];
}CAN_frame;

void CAN_GPIO_Init();

void CAN_config(uint8_t IDE, uint8_t FBM, uint16_t Filter_ID_high, uint16_t Filter_ID_low, uint16_t Filter_Mask_high, uint16_t Filter_Mask_low);

uint8_t CAN_sendFrame(CAN_frame CAN_mess);


void CAN_receiveCallback(CAN_frame CAN_mess);

#endif /* INC_CAN_H_ */

/*
 * can.c
 *
 *  Created on: Apr 14, 2023
 *      Author: hender
 */

#include "can.h"

void CAN_GPIO_Init(){

	RCC->AHB1ENR |= 0x2; //gpio clock activation
	RCC->APB1ENR |= (0x1<<25); // can clock avtivation

	PB->MODER &=  ~(GPIO_MODER_MODER8_Msk |GPIO_MODER_MODER9_Msk); // set the Moder of pin 8 & 9 to 00
	PB->MODER |=  (GPIO_MODER_MODER8_1 |GPIO_MODER_MODER9_1); // set the Moder of pin 8 & 9 to 10 aka Alternate fonction

	PB->PUPDR &= ~(GPIO_PUPDR_PUPD8_Msk | GPIO_PUPDR_PUPD9_Msk);
	PB->OSPEEDR |= 0xF << 16; //Set the pin 8 and 9 to 11 aka hight speed

	PB->AFR[1] &= ~0xFF;
	PB->AFR[1] |= 0x99;

}

void CAN_Config(){
	//clear sleep bit
	CAN1->MCR &= ~(0x2);
	//wait for the can to wakeup
	while(CAN1->MSR & (0x2));
	//Set to config mode
	CAN1->MCR |= 0x1;

	while(!(CAN1->MSR & 0x1));

	//Set all bit but config to 0 (maybe check it later for sleep mode ?)
	CAN1->MCR &= 0x1;
	//TODO change it for a modifed value
	CAN1->BTR = 0x00250042;

	//Realase mail box (? Is it necessary (box empty ?))
	CAN1->RF0R | CAN_RF0R_RFOM0;

	CAN1->IER |= 0x3;


}

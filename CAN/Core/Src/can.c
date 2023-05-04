/*
 * can.c
 *
 *  Created on: Apr 14, 2023
 *      Author: hender
 */

#include "can.h"

static GPIO_TypeDef * PB = GPIOB;


void CAN_GPIO_Init(){

	RCC->AHB1ENR |= 0x2; //gpio clock activation
	RCC->APB1ENR |= (0x1<<25); // can clock activation

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

	//TODO change it for a modifed value rn it's 250kbaus, normal mode, prescler 66 Bsi 6 TQ / BS2 - 3TQ
	CAN1->BTR = 0x0003001B;

	//Realase mail box (? Is it necessary (box empty ?))
	CAN1->RF0R |= CAN_RF0R_RFOM0;

	CAN1->IER |= 0x3;

	NVIC_SetPriority(CAN1_RX0_IRQn,0);
	NVIC_SetPriority(CAN1_TX_IRQn,0);
	NVIC_EnableIRQ(CAN1_RX0_IRQn);
	NVIC_EnableIRQ(CAN1_TX_IRQn);

	//set to normal mod
	CAN1->MCR &= ~(0x1);
	//wait for normal mod
	while((CAN1->MSR & 0x1));

	//Filters in 32 bits mods

	CAN1->FS1R |= 0x3; //activating filter 0 and 1 in 32bits
	CAN1->FM1R |= 0x2; //filter 1 in list
	CAN1->FM1R &= ~ (0x1); //0 in mask mod
	CAN1->FFA1R &= ~(0x3);

	CAN1->sFilterRegister[0].FR1 = (0x100 << 21); //ID
	CAN1->sFilterRegister[0].FR2 = (0x7F0 << 21); //Mask

	CAN1->sFilterRegister[1].FR1 = (0x200 << 21); //ID
	CAN1->sFilterRegister[1].FR2 = (0x205 << 21); //ID
	CAN1->sFilterRegister[1].FR2 |= 0x2; //ID with RTR	= 1

	//Activate Filter 0 and 1
	CAN1->FA1R |= 0x3;
	CAN1->FMR &= 0x0;

}


uint8_t CAN_sendFrame(CAN_frame CAN_mess){
	//Check if TxMailbox0 is empty
	if (CAN1->TSR & 0x04000000){
		//fill the mailbox
		//Writing the the id and the RTR
		CAN1->sTxMailBox[0].TIR = (CAN_mess.STDID << 21) | (CAN_mess.RTR <<1);
		CAN1->sTxMailBox[0].TDTR = CAN_mess.DLC;
		CAN1->sTxMailBox[0].TDLR = CAN_mess.data[3] << 24 | CAN_mess.data[2] << 16 | CAN_mess.data[1] << 8 | CAN_mess.data[0];
		CAN1->sTxMailBox[0].TDHR = CAN_mess.data[7] << 24 | CAN_mess.data[6] << 16 | CAN_mess.data[5] << 8 | CAN_mess.data[4];

		CAN1->sTxMailBox[0].TIR |= 1; //Send the message
		return(1);
	}
	else return(0);
}

/*
 * can.c
 *
 *  Created on: Apr 14, 2023
 *      Author: hender
 */

#include "can.h"

static GPIO_TypeDef * PB = GPIOB;


void CAN_GPIO_Init(){

	RCC->AHB1ENR |= 0x2; //gpio clock activation for can gpio B
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; //for the led gpio D
	RCC->APB1ENR |= (0x1<<25); // can clock activation

	PB->MODER &=  ~(GPIO_MODER_MODER8_Msk |GPIO_MODER_MODER9_Msk); // set the Moder of pin 8 & 9 to 00
	PB->MODER |=  (GPIO_MODER_MODER8_1 |GPIO_MODER_MODER9_1); // set the Moder of pin 8 & 9 to 10 aka Alternate fonction

	PB->PUPDR &= ~(GPIO_PUPDR_PUPD8_Msk | GPIO_PUPDR_PUPD9_Msk);
	PB->OSPEEDR |= 0xF << 16; //Set the pin 8 and 9 to 11 aka hight speed

	PB->AFR[1] &= ~0xFF;
	PB->AFR[1] |= 0x99;

	//Config the pin as general output for the led
	GPIOD->MODER |= GPIO_MODER_MODER12_0;

}

void CAN_config(uint8_t IDE, uint8_t FBM, uint16_t Filter_ID_high, uint16_t Filter_ID_low, uint16_t Filter_Mask_high, uint16_t Filter_Mask_low){
	//clear sleep bit
	CAN1->MCR &= ~(0x2);
	//wait for the can to wakeup
	while(CAN1->MSR & (0x2));
	//Set to config mode
	CAN1->MCR |= 0x1;

	while(!(CAN1->MSR & 0x1));

	//Set all bit but config to 0 (maybe check it later for sleep mode ?)
	CAN1->MCR &= 0x1;

	// 250kbaus, loopback mode, BRP 16 10TQ
	//CAN1->BTR = 0x0403001B;
	CAN1->BTR = 0x40250010;

	//Realase mail box (? Is it necessary (box empty ?))
	CAN1->RF0R |= CAN_RF0R_RFOM0;

	//Enable interrupt on mailbox 0
	CAN1->IER |= 0x3;

	NVIC_SetPriority(CAN1_RX0_IRQn,0);
	NVIC_SetPriority(CAN1_TX_IRQn,0);
	NVIC_EnableIRQ(CAN1_RX0_IRQn);
	NVIC_EnableIRQ(CAN1_TX_IRQn);

	//set to normal mod
	CAN1->MCR &= ~(0x1);
	//wait for normal mod
	while((CAN1->MSR & 0x1));

	//set filter in init mode
	CAN1->FMR |= (0x1); //Mask mode

	CAN1->FA1R |= 0x1;

	//Assign the message from filter 0 to FIFO0
	CAN1->FFA1R &= ~(0x1);

	if (FBM == 0){
		CAN1->FM1R &= ~(0x1); //Set filter 1 in mask mod
	}else{
		CAN1->FM1R |= 0x1; //Set filter 1 in list mod
	}

	// configure filter mode based on IDE
	if (IDE == 0){ // standard mode
		CAN1->FS1R &= ~0x1; //activating filter 0 in 16bits
		CAN1->sFilterRegister[0].FR1 = (Filter_Mask_low << 16 | Filter_ID_low);
		CAN1->sFilterRegister[0].FR2 = (Filter_Mask_high << 16 | Filter_ID_high);

	}else{ // extended mode
		CAN1->FS1R |= 0x1; //activating filter 0 32bits
		CAN1->sFilterRegister[0].FR1 = (Filter_ID_high << 16 | Filter_ID_low);
		CAN1->sFilterRegister[0].FR2 = (Filter_Mask_high << 16 | Filter_Mask_low);
	}

	//Activate Filter 0
	CAN1->FMR &= ~(0x1);

}


uint8_t CAN_sendFrame(CAN_frame CAN_mess){
    //Check if TxMailbox0 is empty
    if (CAN1->TSR & 0x04000000){
        //fill the mailbox
        //Writing the id and the RTR
        uint32_t id = 0;
        if (CAN_mess.IDE == 0) { // standard mode
            id = (CAN_mess.ID << 21) | (CAN_mess.RTR << 1);
        }
        else { // extended mode
            id = (CAN_mess.ID << 3) | (CAN_mess.RTR << 1) | (1 << 2);
        }
        CAN1->sTxMailBox[0].TIR = id;
        CAN1->sTxMailBox[0].TDTR = CAN_mess.DLC;
        CAN1->sTxMailBox[0].TDLR = CAN_mess.data[3] << 24 | CAN_mess.data[2] << 16 | CAN_mess.data[1] << 8 | CAN_mess.data[0];
        CAN1->sTxMailBox[0].TDHR = CAN_mess.data[7] << 24 | CAN_mess.data[6] << 16 | CAN_mess.data[5] << 8 | CAN_mess.data[4];

        CAN1->sTxMailBox[0].TIR |= 1; //Send the message
        return(1);
    }
    else return(0);
}



// Interrupt handler for CAN1 RX0
void CAN1_RX0_IRQHandler(void)
{
    CAN_frame CAN_mess;

    if (CAN1->RF0R & CAN_RF0R_FMP0) // check if there is a message in the FIFO
    {
        // read the message from the FIFO
        CAN_mess.IDE = (CAN1->sFIFOMailBox[0].RIR & CAN_RI0R_IDE) ? 1 : 0; // check if extended identifier
        if (CAN_mess.IDE == 0) { // standard identifier
            CAN_mess.ID = (CAN1->sFIFOMailBox[0].RIR >> 21) & 0x7FF; //Fetch the 11bits ID
        }
        else { // extended identifier
            CAN_mess.ID = (CAN1->sFIFOMailBox[0].RIR >> 3) & 0x1FFFFFFF; //Fetch the 29 bit ID
        }
        CAN_mess.RTR = (CAN1->sFIFOMailBox[0].RIR & CAN_RI0R_RTR) ? 1 : 0; //Is it a reception trame?
        CAN_mess.DLC = CAN1->sFIFOMailBox[0].RDTR & 0x0F;

        for (int i = 0; i < CAN_mess.DLC; i++){
        	CAN_mess.data[i] = (CAN1->sFIFOMailBox[0].RDLR >> (i*8)) & 0xFF;
        }
        // call the receive callback function
        CAN_receiveCallback(CAN_mess);
        // clear the message from the FIFO
        CAN1->RF0R |= CAN_RF0R_RFOM0;
    }
}

void CAN1_TX_IRQHandler(void){

    // Check if transmission mailbox 0 is empty
    if ((CAN1->TSR & CAN_TSR_TME0) == CAN_TSR_TME0)
    {
        // Send the next message in the transmission queue
        // ...

        // Clear the interrupt flag
        CAN1->TSR |= CAN_TSR_TXOK0;
    }
}

// Receive callback function to be implemented by user
void CAN_receiveCallback(CAN_frame CAN_mess)
{
	GPIOD->BSRR |= GPIO_BSRR_BS12;
}



/*
 * int.c
 *
 *  Created on: Jun 19, 2023
 *      Author: hender
 */

#include "int.h"

// Interrupt handler for CAN1 RX0
void CAN1_RX0_IRQHandler(void)
{
    if (CAN1->RF0R & CAN_RF0R_FMP0) // check if there is a message in the FIFO
    {
    	 // clear the message from the FIFO
    	CAN1->RF0R |= CAN_RF0R_RFOM0;

        // send a message to task
        osSignalSet(osThreadGetId(), 0x01);

    }
}

void USART3_IRQHandler(void)
{
		if(USART3->SR & USART_SR_RXNE || USART3->SR & USART_SR_LBD_Msk){
			slave_response();
		}

}

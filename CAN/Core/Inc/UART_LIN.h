/* 
 * File:   UART_LIN.h
 * Author: aravey
 *
 * Created on 12 april 2020, 13:42
 */

#ifndef UART_LIN_H
#define	UART__LIN_H

#include "stm32f4xx.h"

//Declaration to Link External Functions & Variables:

/*--- Lin message structures and flag ---*/

typedef struct
  {
  uint8_t ID;
  uint8_t length;
  uint8_t data[10];
  uint8_t checksum;
  }LINMSG;

//Variables with Global Scope  
extern  unsigned char DisplayData[];
extern  LINMSG Tx_Msg;
extern  LINMSG Rx_Msg;
extern int new_request;

//Functions :
void USART3_IRQHandler(void);
void UART_Init (void);
void SendMessage(LINMSG *msg);
void SendHeader(uint8_t ID);
void SendResponse();
void SendRequest(LINMSG *msg);
int slave_response(void);
void UART_PutChar(uint8_t data);
void sync_break(void);
uint8_t checksum(uint8_t length, uint8_t *data);
static void GPIO_Init(void);



#endif	/* UART_LIN_H */


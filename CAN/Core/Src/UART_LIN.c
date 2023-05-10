/* 
 * File:   UART_LIN.c
 * Author: aravey
 *
 * Created on 12 april 2020, 13:42
 */

#include "UART_LIN.h"

//Functions

//UART_Init() sets up the UART for a 8-bit data, No Parity, 1 Stop bit
//at 9600 baud with transmitter interrupts enabled
void UART_Init(void)
{
    // Enable the USART3 clock
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN;

    // Configure the GPIO pins for USART3
    // ...
    GPIO_Init();

    // Disable USART3
    USART3->CR1 &= ~USART_CR1_UE;

    // Configure USART3 for 8-bit data, no parity, and 1 stop bit
    USART3->CR1 &= ~(USART_CR1_M | USART_CR1_PCE | USART_CR1_OVER8);
    USART3->CR2 &= ~USART_CR2_STOP;
    USART3->BRR = SystemCoreClock / 9600;

    // Enable transmitter interrupts
    USART3->CR1 |= USART_CR1_TXEIE;

    // Enable USART3
    USART3->CR1 |= USART_CR1_UE;
}

static void GPIO_Init(void)
{
    // Enable GPIOB clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    // Configure PB10 (USART3 TX) as AF mode, high speed, and push-pull output
    GPIOB->MODER &= ~GPIO_MODER_MODE10;
    GPIOB->MODER |= GPIO_MODER_MODE10_1;
    GPIOB->OTYPER &= ~GPIO_OTYPER_OT10;
    GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED10;
    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD10;
    GPIOB->AFR[1] &= ~GPIO_AFRH_AFSEL10;
    GPIOB->AFR[1] |= (7U << GPIO_AFRH_AFSEL10_Pos);

    // Configure PB11 (USART3 RX) as AF mode, high speed, and pull-up input
    GPIOB->MODER &= ~GPIO_MODER_MODE11;
    GPIOB->MODER |= GPIO_MODER_MODE11_1;
    GPIOB->OTYPER &= ~GPIO_OTYPER_OT11;
    GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED11;
    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD11;
    GPIOB->PUPDR |= GPIO_PUPDR_PUPD11_0;
    GPIOB->AFR[1] &= ~GPIO_AFRH_AFSEL11;
    GPIOB->AFR[1] |= (7U << GPIO_AFRH_AFSEL11_Pos);
}


/*--- Transmit LIN Message ---*/

void SendMessage(LINMSG *msg)
  {

  }

/*--- Transmit LIN Request ---*/

void SendRequest(LINMSG *msg)
  {

  }


/*--- Send sync field and break ---*/

void sync_break(void)
  {

  }

/*--- Transmit char ---*/
 
void UART_PutChar(uint8_t data)
  {
	//Load data to register
	USART3->DR = data;
	while(!(USART3->SR & 0x00000080));
	while(!(USART3->SR & 0x00000040));
  }

/*--- Calculate lin checksum ---*/

uint8_t checksum(uint8_t length, uint8_t *data)
  {
  uint8_t ix;
  uint16_t check_sum = 0;

  for(ix = 0; ix < length-1; ix++)
    {
    check_sum += data[ix];
    if(check_sum >= 256){
      check_sum -= 255;
      }
    }

  return (uint8_t)(0xff - check_sum);
  }
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

    // Configure the GPIO pins for USART3
    // ...
    GPIO_Init();
    // Enable the USART3 clock
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
    // Disable USART3
    USART3->CR1 &= ~USART_CR1_UE;

    // Configure USART3 for 8-bit data, no parity, and 1 stop bit oversampling 16
    USART3->CR1 &= ~(USART_CR1_M | USART_CR1_PCE | USART_CR1_OVER8);
    USART3->CR1 |= (USART_CR1_TE | USART_CR1_RE);
    USART3->CR2 &= ~USART_CR2_STOP;
    USART3->CR3 &= 0x00000000;
    // Sstem clock (42Mhz) / Baud rate (9600) * 8* (2-CR->Over) = 273.4375 -> Mantissa = 273d=0x111 , Fraction = 0.4375*16 = 7d = 0x7
    USART3->BRR = 0x00001117;

    // Enable transmitter interrupts
    //USART3->CR1 |= USART_CR1_TXEIE;

    // Enable USART3
    USART3->CR1 |= USART_CR1_UE;

    // Perform the delay
    for (uint32_t i = 0; i < 5000000; i++) {
    	__NOP(); // No Operation, consumes one cycle
    }

}


void UART_Init_test (void)
{
    //Initialize PB10 as USART3_TX and PB11 as USART3_RX
        //Activate B port
        SET_BIT(RCC->AHB1ENR,RCC_AHB1ENR_GPIOBEN);
        /* Setup PA2 and PA3 as Alternate Function */
        //MODIFY_REG(GPIOB->MODER, GPIO_MODER_MODER2, 0b01);
        //MODIFY_REG(GPIOB->MODER, GPIO_MODER_MODER3, 0b01);
        GPIOB->MODER |= 0x00A00000;
        GPIOB->MODER &= 0xFFAFFFFF;
        /* Setup Alternate function as USART3 */
        GPIOB->AFR[1] &= 0xFFFF77FF;
        GPIOB->AFR[1] |= 0x00007700;
        /* Push pull output */
        GPIOB->OTYPER &= 0xFFFFF3FF;
        /* Pull up resistor on */
        GPIOB->PUPDR &= 0xFF5FFFFF;
        GPIOB->PUPDR |= 0x00500000;
        /* Output speed set to VeryHigh */
        GPIOB->OSPEEDR |= 0x00F00000;


        //Activate USART3 Clock
        SET_BIT(RCC->APB1ENR,RCC_APB1ENR_USART3EN);
        //Enable USART, no TE no RE yet, Oversampling = 8, 8bit mode, no parity
        //Enable Tx and Rx
        USART3->CR1 = 0x0000002C; //0x0000002C lorsqu'on veut interruptions;
        // LIN mode(4), No clock output (synchronous mode)
        USART3->CR2 = 0x00004000; //4040 SI INTERRUPT LIN BREAK
        // No control mode, 3 sample point,
        USART3->CR3 = 0x00000000;
        // 9600bauds -> USARTDIV = 546.875 -> Mantissa = 546d=0x222 , Fraction = 0.87516 = 14d = 0xE
        USART3->BRR = 0x00001117; //0x0000222E;
        //Enable UART
        USART3->CR1 |= 0x0000200C; //0x0000A02C lorsqu'on veut interruptions;
        //SET_BIT(USART3->CR1, USART_CR1_UE );

        HAL_Delay(1);

        /* USART3 interrupt Init */
        NVIC_SetPriority(USART3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 1));
        NVIC_EnableIRQ(USART3_IRQn);

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
	// Send 10 break bits
	USART3->CR1 |= USART_CR1_SBK;  // Set SBK bit to send break bits
	while (USART3->SR & USART_CR1_SBK);
	while(!(USART3->SR & 0x00000040));

	USART3->DR = 0x55;  // Send sync field with value 0x55
	while(!(USART3->SR & 0x00000080));
	while(!(USART3->SR & 0x00000040));
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

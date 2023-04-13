/*
 * i2c.c
 *
 *  Created on: 12 f�vr. 2020
 *      Author: aravey
 */

#include <DAC.h>
#include "main.h"
#include "stm32f4xx_hal.h"
#include "i2c.h"

uint8_t I2C_buffer[2];					//I2C data buffer array
uint16_t I2S_buffer[1];					//I2S data buffer array


/*_______________________________________________________________________________________
 |------------------------------------ DAC_GPIO_Init ------------------------------------|
 |			This function initializes the GPIOs for the use of the library.			   	 |
 |_______________________________________________________________________________________|
*/
void DAC_GPIO_Init(void)
{
  /* GPIO Ports Clock Enable */
  RCC->AHB1ENR |= 0x8F;

  /*Configure GPIO pin Output Level */
  GPIOD->BSRR &= ~(1<<4);

  /*Configure GPIO pin : RESET_DAC_Pin */
  GPIOD->MODER |= 0x100;
  GPIOD->OTYPER &= ~(1<<4);
  GPIOD->PUPDR &= ~(3<<8);
  GPIOD->OSPEEDR &= ~(3<<8);
}

/*_______________________________________________________________________________________
 |----------------------------------- DAC_Power_Up --------------------------------------|
 |	 In this function, the DAC is powered up and initialized. The Reset Pin PD4 is		 |
 |	 	 	 	maintained high. I2C bus is used to communicate with the DAC.			 |
 |_______________________________________________________________________________________|
*/
void DAC_Power_Up(){

	GPIOD->BSRR = 0x00000010;				//Pin PD4 high (reset pin of the DAC)

//Initializing the DAC as we want
	I2C_buffer[0] = POWER_CTRL_2;
	I2C_buffer[1] = 0xA0;					//Turning headphone channel and speaker channel ON
	I2C_Master_Transmit(DAC_ADDR_W, I2C_buffer, 2);

	I2C_buffer[0] = ITRF_CTRL_1;
	I2C_buffer[1] = 0x87;					//Enable I2S clocking control, Master only
	I2C_Master_Transmit(DAC_ADDR_W, I2C_buffer, 2);


//Initializing the DAC, datasheet's procedure
	I2C_buffer[0] = 0x00;
	I2C_buffer[1] = 0x99;
	I2C_Master_Transmit(DAC_ADDR_W, I2C_buffer, 2);

	I2C_buffer[0] = 0x47;
	I2C_buffer[1] = 0x80;
	I2C_Master_Transmit(DAC_ADDR_W, I2C_buffer, 2);

	I2C_buffer[0] = 0x32;
	I2C_buffer[1] = 0x80;
	I2C_Master_Transmit(DAC_ADDR_W, I2C_buffer, 2);

	I2C_buffer[0] = 0x00;
	I2C_buffer[1] = 0x00;
	I2C_Master_Transmit(DAC_ADDR_W, I2C_buffer, 2);

	I2C_buffer[0] = 0x02;						//Power Control 1 register
	I2C_buffer[1] = 0x9E;						//Power-Up the DAC
	I2C_Master_Transmit(DAC_ADDR_W, I2C_buffer, 2);

}




/*_______________________________________________________________________________________
 |---------------------------------- DAC_Power_Down -------------------------------------|
 | In this function, the DAC is powered down. The beep and speaker channel are set OFF.	 |
 |	 	 	 				The reset pin is maintained low.							 |
 |_______________________________________________________________________________________|
*/
void DAC_Power_Down(){


	I2C_buffer[0] = BEEP_CONFIG_ADDR;		//DAC register of address 0x1E
	I2C_buffer[1] = 0x00;					//Disable beep
	I2C_Master_Transmit(DAC_ADDR_W, I2C_buffer, 2);


	I2C_buffer[0] = POWER_CTRL_2;
	I2C_buffer[1] = 0xF0;					//Turning headphone channel and speaker channel OFF
	I2C_Master_Transmit(DAC_ADDR_W, I2C_buffer, 2);

	I2C_buffer[0] = POWER_CTRL_1;
	I2C_buffer[1] = 0x9F;					//Powering Down the DAC
	I2C_Master_Transmit(DAC_ADDR_W, I2C_buffer, 2);

	HAL_Delay(1);							//Wait 1ms

	GPIOD->BSRR = 0x00100000;				//Pin PD4 low (reset pin of the DAC)
}





/*_______________________________________________________________________________________
 |------------------------------------ Beep_Config --------------------------------------|
 |	 In this function, the sound signal (called "Beep") is configured. The frequency,	 |
 |	 				the ON time, the OFF time and the volume are configured.			 |
 |				   The value of the registers are transmitted via the I2C bus.			 |
 |     Several macros are defined in the header to allow you to customize the sound.     |
 |_______________________________________________________________________________________|
*/
void Beep_Config(uint8_t beepDuration, uint8_t silenceDuration, uint8_t frequency){

  //Configure the type of beep signal (multiple beep)
	  I2C_buffer[0] = BEEP_CONFIG_ADDR;						//DAC register of address 0x1E
	  I2C_buffer[1] = 0x80;									//Select the multiple beep mode
	  I2C_Master_Transmit(DAC_ADDR_W, I2C_buffer, 2);

/*
  //Configure the type of beep signal (continuous beep)
	  I2C_buffer[0] = BEEP_CONFIG_ADDR;						//DAC register of address 0x1E
	  I2C_buffer[1] = 0xC0;									//Select the continuous beep mode
	  I2C_Master_Transmit(DAC_ADDR_W, I2C_buffer, 2);
*/

  //Configure the beep frequency and the beep ON time
	  I2C_buffer[0] = BEEP_FREQ_ADDR;						//DAC register of address 0x1C
	  I2C_buffer[1] = (frequency << 4) | beepDuration;		//Set sound frequency and sound duration
	  I2C_Master_Transmit(DAC_ADDR_W, I2C_buffer, 2);

  //Configure the beep OFF time and the beep volume
	  I2C_buffer[0] = BEEP_VOLUME_ADDR;						//DAC register of address 0x1D
	  I2C_buffer[1] = (silenceDuration << 5) | 0x1A;		//Set silence duration
	  I2C_Master_Transmit(DAC_ADDR_W, I2C_buffer, 2);

}





/*_______________________________________________________________________________________
 |------------------------------------- Run_Alarm ---------------------------------------|
 |					Emitting the alarm by sending a frame on the I2S bus				 |
 |_______________________________________________________________________________________|
*/
void Run_Alarm(){
	I2S_buffer[0] = 0xAAAA;							//Frame to send on the I2S bus
	HAL_I2S_Transmit(&hi2s3, I2S_buffer, 1, 500);
}

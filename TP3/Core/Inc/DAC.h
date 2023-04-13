/*
 * i2c.h
 *
 *  Created on: 10 f�vr. 2020
 *      Author: aravey
 */

#ifndef DAC_H_
#define DAC_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"

#define DAC_ADDR_W			    0x94		//I2C address of the DAC, with LSB=0 for Master transmitting
#define DAC_ADDR_R			    0x95		//I2C address of the DAC, with LSB=1 for Master receiving
#define POWER_CTRL_1		    0x02		//Power Control 1 register address
#define POWER_CTRL_2		    0x04		//Power Control 2 register address
#define ITRF_CTRL_1			    0x06		//Interface Control 1 register address
#define	BEEP_CONFIG_ADDR	    0x1E		//DAC register address to enable the beep mode
#define	BEEP_VOLUME_ADDR	    0x1D		//DAC register address to adjust beep OFF time and volume
#define BEEP_FREQ_ADDR		    0x1C		//DAC register address to adjust the beep frequency and ON time


// ----- The macros below can be used in the 'Beep_config( )' function : -----

// beepDuration parameter :
#define VERY_SHORT_BEEP         0
#define SHORT_BEEP              0x02
#define MEDIUM_BEEP             0x05
#define LONG_BEEP               0x0A
#define VERY_LONG_BEEP          0x0F


// silenceDuration parameter :
#define SHORT_SILENCE           0
#define MEDIUM_SILENCE          0x02
#define LONG_SILENCE            0x03
#define VERY_LONG_SILENCE       0x04
#define VERY_VERY_LONG_SILENCE  0x07


// frequency parameter :
#define VERY_HIGH_PITCHED_SOUND 0x0F
#define HIGH_PITCHED_SOUND      0x0C
#define MEDIUM_SOUND            0x08
#define DEEP_SOUND              0x04
#define VERY_DEEP_SOUND         0

//----------------------------------------------------------------------------

extern I2S_HandleTypeDef hi2s3;
extern I2C_HandleTypeDef hi2c1;
extern uint8_t I2C_buffer[2];			//I2C data buffer array
extern uint16_t I2S_buffer[1];			//I2S data buffer array

void DAC_Power_Up(void);			//Initializing and powering up the DAC
void DAC_Power_Down(void);		//Disabling and powering down the DAC
void Beep_Config(uint8_t beepDuration, uint8_t silenceDuration, uint8_t frequency); //Configuring and enabling the beep
void Run_Alarm(void);			//Putting the alarm sound on

void DAC_GPIO_Init(void);
#endif

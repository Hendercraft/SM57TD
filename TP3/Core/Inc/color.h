/*
 * color.h
 *
 *  Created on: 12 avr. 2023
 *      Author: marie
 */

#ifndef COLORH
#define COLORH

#include "i2c.h"
#include "stm32f4xx_hal.h"
#include "usart2.h"

// TCS3472 registers definition
#define _ENABLE  0x00    // Enable status and interrupts
#define _ATIME   0x01    // RGBC ADC time
#define _WTIME   0x03    // Wait time
#define _AILTL   0x04    // RGBC Interrupt low treshold low byte
#define _AILTH   0x05    // RGBC Interrupt low treshold high byte
#define _AIHTL   0x06    // RGBC Interrupt high treshold low byte
#define _AIHTH   0x07    // RGBC Interrupt high treshold high byte
#define _PERS    0x0C    // Interrupt persistence filters
#define _CONFIG  0x0D    // Configuration
#define _CONTROL 0x0F    // Gain control register
#define _ID      0x12    // Device ID
#define _STATUS  0x13    // Device status

// TCS3472 color registers
#define _CDATA   0x14    // Clear ADC low data register
#define _CDATAH  0x15    // Clear ADC high data register
#define _RDATA   0x16    // RED ADC low data register
#define _RDATAH  0x17    // RED ADC high data register
#define _GDATA   0x18    // GREEN ADC low data register
#define _GDATAH  0x19    // GREEN ADC high data register
#define _BDATA   0x1A    // BLUE ADC low data register
#define _BDATAH  0x1B    // BLUE ADC high data register

#define _SENSOR_ADRESS  0x29

//I2C address
#define _SENSOR_ADRESS  0x29
#define _COLOR_W_ADDRESS (_SENSOR_ADRESS << 1) //Write I2C value
#define _COLOR_R_ADDRESS (_COLOR_W_ADDRESS | 0b1) //Read I2C value

//Constant to write a 1 to the 7th bit of the control register
#define _CMD 0x80

// Gain constants
#define _GAIN_x1    0x00
#define _GAIN_x4    0x01
#define _GAIN_x16   0x10
#define _GAIN_x60   0x11

// Struct used to store colors values
typedef struct
{
    uint16_t Red;
    uint16_t Green;
    uint16_t Blue;
    uint16_t Clear;
}ColorStruct16;

typedef struct
{
    uint8_t Red;
    uint8_t Green;
    uint8_t Blue;
    uint16_t Clear;
}RGBC;

//Defining a color enum cause it's fun to do so
enum Color{
	RED = 'R',
	GREEN = 'G',
	BLUE = 'B',
	CLEAR = 'C'
};

////////////////////////////////////////////////////////////


/* This function initializes the I2C protocol and the necessary registers for the color sensor to communicate and to be written
 *
 *
 */
void ColorSensor_Init();

/**
  * @brief Write data to the internal register of the Sensor, using I2C
  * @param address, the internal address of the sensor we want to write to
  * @param data, the value we want to write
  * @retval None
  */
void ColorSensor_Send(uint8_t address,uint8_t data);

/**
  * @brief Write data to the internal register of the Sensor, using I2C
  * @param address, the internal address of the sensor we want to read
  * @retval uint8_t, containing the data from the register
  */
uint8_t ColorSensor_Recive(uint8_t address);

/**
  * @brief Write data to the internal register of the Sensor, using I2C
  * @param address, the internal address of the sensor we want to read (lower byte)
  * @retval uint16_t, containing the data from the low and high byte
  */
uint16_t ColorSensor_Recive_u16(uint8_t address);

/**
  * @brief Return the Raw value of a given color
  * @param enum Color, RED,GREEN,BLUE,CLEAR
  * @retval uint16_t, the raw value of the color
  */
uint16_t Read_Color_value(enum Color reg);


/**
  * @brief Return the raw color value from the Sensor register in a struct
  * @retval ColorStruct16, A structure containing all the raw value from the sensor
  */
ColorStruct16 Read_Color_All();

/**
  * @brief Convert our raw data into a RGBC struct where all the value but the clear one are express on 8 bits
  * @retval RGBC, A RGB value with the Clear value on 16
  */
RGBC Convert_Color_To_RGBC(ColorStruct16 Colours);

#endif




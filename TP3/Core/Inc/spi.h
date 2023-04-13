/*
 * spi.h
 *
 *  Created on: 18 févr. 2020
 *      Author: aravey
 */

#ifndef SPI_H_
#define SPI_H_



#endif /* SPI_H_ */
#include "stm32f4xx_hal.h"

//defines

#define CS_HIGH HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET)
#define CS_LOW HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_RESET)

/* Dummy Byte Send by the SPI Master device in order to generate the Clock to the Slave device */
#define DUMMY_BYTE                        0x00

//prototypes
void init_SPI(void);
uint8_t SPIrw(uint8_t address, uint8_t data);
void SPIwrite(uint8_t address, uint8_t data);
uint8_t SPIread(uint8_t address);

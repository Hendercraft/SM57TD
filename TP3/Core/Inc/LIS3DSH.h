/**
  ******************************************************************************
  * @file           : LIS3DSH.h
  * @brief          : LIS3DSH header file Library
  ******************************************************************************
  * Created on: 24 mars 2023
  * Author: Léo PARIGAUX, Zetian WANG, Maxime MESSAOUI
  *
  ******************************************************************************
  */



//include
#include "main.h"
#include "spi.h"
#include "usart2.h"

#ifndef INC_LIS3DSH_H_
#define INC_LIS3DSH_H_

/*Functions Prototype */
void LIS302DL_init(void);
void LIS3DSH_affiche(void);
uint16_t detecter(int p);

#endif /* INC_LIS3DSH_H_ */

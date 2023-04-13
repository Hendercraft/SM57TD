/**
  ******************************************************************************
  * @file           : LIS3DSH.c
  * @brief          : LIS3DSH Library
  ******************************************************************************
  * Created on: 24 mars 2023
  * Author: Léo PARIGAUX, Zetian WANG, Maxime MESSAOUI
  *
  ******************************************************************************
  */

//include
#include "spi.h"
#include "usart2.h"

int MOVE;
int p;
float X;
float Y;
float Z;
int Z1;
int Z2;

char resultatX[20];
char resultatY[20];
char resultatZ[20];
char msg_RAM[]={'X','Y','Z',' ','\0'};
char msg_RAM2[]={'M','O','V','E','\0'};

int CTRL_REG5 	= 0x24;
int CTRL_REG1 	= 0x21;
int CTRL_REG3	= 0x23;
int CTRL_REG4	= 0x20;
int THRS2_1		= 0x56;
int THRS1_1		= 0x57;
int STx_1		= 0x40;
int MASK1_B		= 0x59;
int MASK1_A		= 0x5A;
int SETT1		= 0x5B;

void LIS302DL_init(void);
uint16_t detecter(int p);
void LIS3DSH_affiche(void);



/*initalisation of the LIS3DSH*/
void LIS302DL_init(void)
{
			//Ecrire valeurs dans CTRL_REG5
			//default BW, 2g scale, self stale disabled, SPI 4Wires)
			SPIwrite(CTRL_REG5 & 0x7f, 0x00);

			//Ecrire valeurs dans CTRL_REG4
			//400Hz data rate, continuous update, axis enabled
			SPIwrite(CTRL_REG4 & 0x7f, 0x77);
}



/*Plot with usart of the value of X, Y and Z*/
void LIS3DSH_affiche(void)
{
  	SPIread(CTRL_REG5 | 0x80); //read CTRL_REG5 2 time before to read the value.
  	SPIread(CTRL_REG5 | 0x80);

  	//Acquisition
  	X=SPIread(0x29 | 0x80);
  	Y=SPIread(0x2B | 0x80);
  	Z=SPIread(0x2D | 0x80);

	//AFFICHAGE X Y Z
  	newLine();newLine();
  	serial_puts("X: ");
  	serial_puts(float2string(X,resultatX));
  	serial_puts("  ");
  	serial_puts("Y: ");
  	serial_puts(float2string(Y,resultatY));
  	serial_puts("  ");
  	serial_puts("Z: ");
  	serial_puts(float2string(Z,resultatZ));
}



/*Detection of the movement
 *
 * Parameter in :
 * P [int] : sensitivity of the motion detection
 * */
uint16_t  detecter(int p)
{
	uint16_t MOVE;
	/*Initialisation de la variable MOVE*/
	MOVE=0;

	/*Acquisition à t1*/
	Z1=SPIread(0x2D | 0x80);

	/*DELAY*/
  	HAL_Delay(250);

  	/*Acquisition à t2*/
  	Z2=SPIread(0x2D | 0x80);

  	/*Comparaison des positions*/
  	if(abs(Z1-Z2)>=p)				//p est la sensibilité
  	{
  		MOVE=1;						//Incrémentation de la variable
  		newLine();					//Nouvelle line dans le terminal
  		//serial_puts(msg_RAM2);		//Affichage du message dans le terminal
  	}
  	return MOVE;
}

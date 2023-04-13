/*
 * color.c
 *
 *  Created on: Apr 7, 2023
 *      Author: marie
 */

#include "color.h"

void ColorSensor_Init(){
	init_I2C1();

	ColorSensor_Send(_ENABLE,0b1);

	//Setting the delay for RGBC at 2.4ms
	ColorSensor_Send(_ATIME, 0xFF);
	//Setting the gain at 1x
	ColorSensor_Send(_CONTROL,_GAIN_x1);

	//necessary delay for the sensor
	HAL_Delay(3);
}


void ColorSensor_Send(uint8_t address,uint8_t data){
	//Setting the 7th bit of the register to 0, and writing the address we want to write to in the first 5 bit
	uint8_t command_register = _CMD | (address & 0x1F);
	uint8_t data_buffer[2] = {command_register,data}; //Precede the data we want to write by the command
    I2C_Master_Transmit(_COLOR_W_ADDRESS, data_buffer, 2);

}

uint8_t ColorSensor_Recive(uint8_t address){
	//Setting the 7th bit of the register to 0, and writing the address we want to read in the first 5 bit
	uint8_t command_register = _CMD | (address & 0x1F);
	I2C_Master_Transmit(_COLOR_W_ADDRESS, &command_register, 1);
	uint8_t data;
	I2C_Master_Receive(_COLOR_R_ADDRESS, &data, 1);
	return data;
}

uint16_t ColorSensor_Recive_U16(uint8_t address){
	return ColorSensor_Recive(address) + (ColorSensor_Recive(address+1) << 8);
}

uint16_t Read_Color(enum Color reg){

	uint16_t Out_color;
	 switch(reg){
		case 'C':
					Out_color = ColorSensor_Recive_U16(_CDATA);
					return Out_color;
		case 'R':
					Out_color = ColorSensor_Recive_U16(_RDATA);
					return Out_color;
		case 'G':
					Out_color = ColorSensor_Recive_U16(_GDATA);
					return Out_color;
		case 'B':
					Out_color = ColorSensor_Recive_U16(_BDATA);
					return Out_color;
		default:
					return 0;
	  }
}

ColorStruct16 Read_Color_All(){

	ColorStruct16 value;
	value.Blue = Read_Color(BLUE);
	value.Green = Read_Color(GREEN);
	value.Red = Read_Color(RED);
	value.Clear = Read_Color(CLEAR);
	return value;

}

RGBC Convert_Color_To_RGBC(ColorStruct16 Colours){
	RGBC value;
	value.Blue = ((float)Colours.Blue/(float)Colours.Clear * 255 < 255) ? (float)Colours.Blue/(float)Colours.Clear * 255 : 255;
	value.Red = ((float)Colours.Red/(float)Colours.Clear * 255 < 255) ? (float)Colours.Red/(float)Colours.Clear * 255 : 255;
	value.Green = ((float)Colours.Green/(float)Colours.Clear * 255 < 255) ? (float)Colours.Green/(float)Colours.Clear * 255 : 255;
	value.Clear = Colours.Clear;
	return value;
}

void Print_Color_To_Usart(){
	//First we activate the RGBC
	ColorSensor_Send(_ENABLE, 0b00000011);
	HAL_Delay(5);
	ColorStruct16 raw_color = Read_Color_All();
	RGBC rgbc_value = Convert_Color_To_RGBC(raw_color);
	serial_puts("The value seen by the color sensors is : \r\n");
	char* buffer = Rgbc_To_String(&rgbc_value);
	serial_puts(buffer);
	free(buffer);
	//Once done we desactivate it to "save" power
	ColorSensor_Send(_ENABLE, 0b00000001);

}

char* Rgbc_To_String(RGBC *rgbc_value){
	char* rgbc_buffer = (char*) malloc(sizeof(char)*50);
	sprintf(rgbc_buffer, "R:%02d G:%02d B:%02d C:%02d\r\n", rgbc_value->Red, rgbc_value->Green, rgbc_value->Blue,rgbc_value->Clear);  // format time string
	return rgbc_buffer;
}

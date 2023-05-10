/*
 * usart2.h
 *
 *  Created on: 13 sept. 2019
 *      Author: aravey
 */

#ifndef USART2_H_
#define USART2_H_


/*Functions Prototype */
void init_usart(void);
void init_usart_int(void);
void serial_putc(char c);
void serial_puts( char *msg);
void newLine();
signed int string2int(char *s);
char * int2string(signed int num, char * s);
char * float2string(float nombre,char *chaine);

#endif /* USART2_H_ */

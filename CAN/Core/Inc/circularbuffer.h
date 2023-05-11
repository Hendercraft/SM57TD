
#ifndef INC_CIRCULARBUFFER_H_
#define INC_CIRCULARBUFFER_H_
#include "stdlib.h"
/* typedef struct{
	int ID;
	int IDE;
	int RTR;
	int DLC;
	int data[8];
}CAN_frame; */
typedef struct CircularBuffer CircularBuffer; //Forwad decleration


CircularBuffer* getNewBuffer(void);
int pushToBuffer(CircularBuffer *buffer, void* frame);
int pullFromBuffer(CircularBuffer *buffer, void** frame);

#endif

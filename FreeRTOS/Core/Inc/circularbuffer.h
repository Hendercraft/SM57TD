
#ifndef INC_CIRCULARBUFFER_H_
#define INC_CIRCULARBUFFER_H_
#include "stdlib.h"
#include "can.h"
#include "UART_LIN.h"

typedef struct CircularBufferCan CircularBufferCan; //Forwad decleration
typedef struct CircularBufferLin CircularBufferLin;


CircularBufferCan* getNewBufferCan(void);
CircularBufferLin* getNewBufferLin(void);

int pushToBufferCan(CircularBufferCan *buffer, CAN_frame frame);
int pullFromBufferCan(CircularBufferCan *buffer, CAN_frame* frame);

int pushToBufferLin(CircularBufferLin *buffer, LINMSG frame);
int pullFromBufferLin(CircularBufferLin *buffer, LINMSG* frame);

#endif

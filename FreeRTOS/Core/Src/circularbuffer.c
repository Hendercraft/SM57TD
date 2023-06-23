#include "circularbuffer.h"

typedef struct CircularBufferCan{
	CAN_frame frameBuffer[10];
	int readIndex;
	int writeIndex;
	int bufferLenght;
}CircularBufferCan;

typedef struct CircularBufferLin{
	LINMSG frameBuffer[10];
	int readIndex;
	int writeIndex;
	int bufferLenght;
}CircularBufferLin;

CircularBufferCan* getNewBufferCan(void){

  CircularBufferCan* newbuffer = (CircularBufferCan*) malloc(sizeof(CircularBufferCan));
  newbuffer->readIndex = 0;
  newbuffer->writeIndex = 0;
  newbuffer->bufferLenght = 0;
  return newbuffer;
}

CircularBufferLin* getNewBufferLin(void){

  CircularBufferLin* newbuffer = (CircularBufferLin*) malloc(sizeof(CircularBufferLin));
  newbuffer->readIndex = 0;
  newbuffer->writeIndex = 0;
  newbuffer->bufferLenght = 0;
  return newbuffer;
}

int pushToBufferCan(CircularBufferCan *buffer, CAN_frame frame){
  if(buffer->bufferLenght == 10){
    //Buffer plein
    return 1;
  }

  buffer->frameBuffer[buffer->writeIndex] = frame;

  buffer->bufferLenght++;

  if(buffer->writeIndex == 10){
    buffer->writeIndex = 0;
  }else{
    buffer->writeIndex++;
  }
  return 0;
}

int pullFromBufferCan(CircularBufferCan *buffer, CAN_frame* frame){
  if (buffer->bufferLenght == 0){
    //Buffer vide
    return 1;
  }

  frame = &buffer->frameBuffer[buffer->readIndex];

  buffer->bufferLenght--;

  if(buffer->readIndex == 10){
    buffer->readIndex = 0;
  }else{
    buffer->readIndex++;
  }
  return 0;
}


int pushToBufferLin(CircularBufferLin *buffer, LINMSG frame){
  if(buffer->bufferLenght == 10){
    //Buffer plein
    return 1;
  }

  buffer->frameBuffer[buffer->writeIndex] = frame;

  buffer->bufferLenght++;

  if(buffer->writeIndex == 10){
    buffer->writeIndex = 0;
  }else{
    buffer->writeIndex++;
  }
  return 0;
}

int pullFromBufferLin(CircularBufferLin *buffer, LINMSG* frame){
  if (buffer->bufferLenght == 0){
    //Buffer vide
    return 1;
  }

  frame = &buffer->frameBuffer[buffer->readIndex];

  buffer->bufferLenght--;

  if(buffer->readIndex == 10){
    buffer->readIndex = 0;
  }else{
    buffer->readIndex++;
  }
  return 0;
}


#include "circularbuffer.h"
#include "can.h"


typedef struct CircularBuffer{
	CAN_frame frameBuffer[10];
	int readIndex;
	int writeIndex;
	int bufferLenght;
}CircularBuffer;

CircularBuffer* getNewBuffer(void){

  CircularBuffer* newbuffer = (CircularBuffer*) malloc(sizeof(CircularBuffer));
  newbuffer->readIndex = 0;
  newbuffer->writeIndex = 0;
  newbuffer->bufferLenght = 0;
  return newbuffer;
}

int pushToBuffer(CircularBuffer *buffer, void* frame){
  if(buffer->bufferLenght == 10){
    //Buffer plein
    return 1;
  }

  buffer->frameBuffer[buffer->writeIndex] = *(CAN_frame*)frame;

  buffer->bufferLenght++;

  if(buffer->writeIndex == 10){
    buffer->writeIndex = 0;
  }else{
    buffer->writeIndex++;
  }
  return 0;
}

int pullFromBuffer(CircularBuffer *buffer, void **frame){
  if (buffer->bufferLenght == 0){
    //Buffer vide
    return 1;
  }

  *frame = &buffer->frameBuffer[buffer->readIndex];

  buffer->bufferLenght--;

  if(buffer->readIndex == 10){
    buffer->readIndex = 0;
  }else{
    buffer->readIndex++;
  }
  return 0;

}

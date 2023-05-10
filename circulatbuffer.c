#include "circularbuffer.h"

CircularBuffer getNewBuffer(void){
  CircularBuffer newbuffer;
  newbuffer.readIndex = 0;
  newbuffer.writeIndex = 0;
  newbuffer.bufferLenght = 0;
  return newbuffer;
}

int pushToBuffer(CircularBuffer *buffer, CAN_frame frame){
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

int pullFromBuffer(CircularBuffer *buffer, CAN_frame *frame){
  if (buffer->bufferLenght == 0){
    //Buffer vide
    return 1;
  }

  *frame = buffer->frameBuffer[buffer->readIndex];

  buffer->bufferLenght--;

  if(buffer->readIndex == 10){
    buffer->readIndex = 0;
  }else{
    buffer->readIndex++;
  }
  return 0;
  
}
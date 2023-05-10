/* typedef struct{
	int ID;
	int IDE;
	int RTR;
	int DLC;
	int data[8];
}CAN_frame; */

typedef struct CircularBuffer{
  CAN_frame frameBuffer[10];
  int readIndex;
  int writeIndex;
  int bufferLenght;
}CircularBuffer;


CircularBuffer getNewBuffer(void);
int pushToBuffer(CircularBuffer *buffer, CAN_frame frame);
int pullFromBuffer(CircularBuffer *buffer, CAN_frame *frame);
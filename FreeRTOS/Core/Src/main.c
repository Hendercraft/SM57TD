/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define LIN_MODE 1
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
osThreadId defaultTaskHandle;
osThreadId Task_can_receptHandle;
osThreadId Task_can_sendHandle;
osThreadId Task_can_procesHandle;
osThreadId Task_lin_procesHandle;
osThreadId Task_lin_sendHandle;
osTimerId Timer_Ask_comodoHandle;
osSemaphoreId CanRxSemaphoreHandle;
osSemaphoreId CanTxSemaphoreHandle;
osSemaphoreId LinRxSemaphoreHandle;
osSemaphoreId LinTxSemaphoreHandle;
/* USER CODE BEGIN PV */
CircularBufferCan* CanTxBuffer;
CircularBufferCan* CanRxBuffer;
CircularBufferLin* LinTxBuffer;
CircularBufferLin* LinRxBuffer;

CAN_frame Ask_comodo_frame; //RTC frame sent to the comodo


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void StartDefaultTask(void const * argument);
void can_reception(void const * argument);
void can_send(void const * argument);
void can_process(void const * argument);
void lin_process(void const * argument);
void lin_send(void const * argument);
void ask_comodo(void const * argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// Interrupt handler for CAN1 RX0
void CAN1_RX0_IRQHandler(void)
{
    if (CAN1->RF0R & CAN_RF0R_FMP0) // check if there is a message in the FIFO
    {
    	 // clear the message from the FIFO
    	CAN1->RF0R |= CAN_RF0R_RFOM0;

        // send a message to task
        osSignalSet(Task_can_receptHandle, 0x01);

    }
}

void USART3_IRQHandler(void)
{
		if(USART3->SR & USART_SR_RXNE || USART3->SR & USART_SR_LBD_Msk){
			slave_response();
		}

}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	CanTxBuffer = getNewBufferCan();
	CanRxBuffer = getNewBufferCan();
	LinTxBuffer = getNewBufferLin();
	LinRxBuffer = getNewBufferLin();

	Ask_comodo_frame.ID = 0x10510111;
	Ask_comodo_frame.RTR = 1;
	Ask_comodo_frame.IDE = 1;
	Ask_comodo_frame.DLC = 0;

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  CAN_GPIO_Init();
  CAN_Counter_Init();
  //2.3
  //32bit mode, list mode
  //ID = 0x010,standard ID, accept both data and request frame
  CAN_config(1,1,(0x10 << 5),0x2,(0x10 << 5),0x00); //TODO init with right ID


  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of CanRxSemaphore */
  osSemaphoreDef(CanRxSemaphore);
  CanRxSemaphoreHandle = osSemaphoreCreate(osSemaphore(CanRxSemaphore), 10);

  /* definition and creation of CanTxSemaphore */
  osSemaphoreDef(CanTxSemaphore);
  CanTxSemaphoreHandle = osSemaphoreCreate(osSemaphore(CanTxSemaphore), 10);

  /* definition and creation of LinRxSemaphore */
  osSemaphoreDef(LinRxSemaphore);
  LinRxSemaphoreHandle = osSemaphoreCreate(osSemaphore(LinRxSemaphore), 10);

  /* definition and creation of LinTxSemaphore */
  osSemaphoreDef(LinTxSemaphore);
  LinTxSemaphoreHandle = osSemaphoreCreate(osSemaphore(LinTxSemaphore), 10);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* definition and creation of Timer_Ask_comodo */
  osTimerDef(Timer_Ask_comodo, ask_comodo);
  Timer_Ask_comodoHandle = osTimerCreate(osTimer(Timer_Ask_comodo), osTimerPeriodic, NULL);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of Task_can_recept */
  osThreadDef(Task_can_recept, can_reception, osPriorityHigh, 0, 128);
  Task_can_receptHandle = osThreadCreate(osThread(Task_can_recept), NULL);

  /* definition and creation of Task_can_send */
  osThreadDef(Task_can_send, can_send, osPriorityHigh, 0, 128);
  Task_can_sendHandle = osThreadCreate(osThread(Task_can_send), NULL);

  /* definition and creation of Task_can_proces */
  osThreadDef(Task_can_proces, can_process, osPriorityNormal, 0, 128);
  Task_can_procesHandle = osThreadCreate(osThread(Task_can_proces), NULL);

  /* definition and creation of Task_lin_proces */
  osThreadDef(Task_lin_proces, lin_process, osPriorityNormal, 0, 128);
  Task_lin_procesHandle = osThreadCreate(osThread(Task_lin_proces), NULL);

  /* definition and creation of Task_lin_send */
  osThreadDef(Task_lin_send, lin_send, osPriorityHigh, 0, 128);
  Task_lin_sendHandle = osThreadCreate(osThread(Task_lin_send), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();
  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_can_reception */
/**
* @brief Function implementing the Task_can_recept thread.
* This Task fetch can frame when it's given a signal by a CAN_RX interrupt
* Once it has fetch the frame, it adds it to a cricular buffer, and release a semaphore so it can be processed
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_can_reception */
void can_reception(void const * argument)
{
  /* USER CODE BEGIN can_reception */
  /* Infinite loop */
  for(;;)
  {
	osSignalWait(0,0); //wait for the interruption to generate a signal
    CAN_frame recived_frame = CAN_frameFetch(); //Fetch the frame from the DR
    pushToBufferCan(CanRxBuffer,recived_frame);
    osSemaphoreRelease(CanRxSemaphoreHandle); //Release the semaphore so the frame can be processed
  }
  /* USER CODE END can_reception */
}

/* USER CODE BEGIN Header_can_send */
/**
* @brief Function implementing the Task_can_send thread.
* This Task wait for a semaphore that is released each time a frame is added to the TxBuffer
* It then fetch this frame and send it to the bus
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_can_send */
void can_send(void const * argument)
{
  /* USER CODE BEGIN can_send */
  /* Infinite loop */
  for(;;)
  {
	  osSemaphoreWait(CanTxSemaphoreHandle,0); //Waiting for a semaphore
	  CAN_frame *frame;
	  pullFromBufferCan(CanTxBuffer,frame); //Fetching the frame from the buffer
	  CAN_sendFrame(*frame); //Sending the frame
  }
  /* USER CODE END can_send */
}

/* USER CODE BEGIN Header_can_process */
/**
* @brief Function implementing the can_process_RX thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_can_process */
void can_process(void const * argument)
{
	osTimerStart (Timer_Ask_comodoHandle,500);// Activate the timer to ask the comodo every 500ms
  /* USER CODE BEGIN can_process */
  /* Infinite loop */
  for(;;)
  {
	  // Fetch a CAN frame from the queue
	  osSemaphoreWait(CanRxSemaphoreHandle,0); //Waiting for a semaphore
	  CAN_frame *frame;
	  pullFromBufferCan(CanRxBuffer,frame);
	  if(frame->ID == 0x10015111){ //If it's the response from the comodo
		  serial_puts("Recived a CAN frame with comodo info\r\n");
		  serial_puts("Sending a LIN frame containing the info\r\n");

		  //*Sending the info via LIN*//
		  LINMSG lin_frame;
		  lin_frame.ID = 0x44;
		  lin_frame.length = 1;
		  lin_frame.data[0] = frame->data[0];
		  lin_frame.checksum = checksum(1,lin_frame.data);

		  //Puting the frame in the buffer
		  pushToBufferLin(LinTxBuffer,lin_frame);
		  //Unblocking the sendLin
		  osSemaphoreRelease(LinTxSemaphoreHandle);
	  }else if(frame->ID == 0x10015112){

		  //*Sending the info via LIN*//
		  LINMSG* lin_frame;
		  lin_frame->ID = SLAVE_ADDR_READ;
		  lin_frame->length = 1;
		  lin_frame->data[0] = frame->data[0];
		  lin_frame->checksum = checksum(1,lin_frame->data);
		  SendMessage(lin_frame,0); //Sending the message manually as it's in "slave mode"

	  }
  }
  /* USER CODE END can_process */
}

/* USER CODE BEGIN Header_lin_process */
/**
* @brief Function implementing the Task_lin_proces thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_lin_process */
void lin_process(void const * argument)
{
  /* USER CODE BEGIN lin_process */
  /* Infinite loop */
  for(;;){
	  osSemaphoreWait(LinRxSemaphoreHandle,0);
	  LINMSG* frame;
	  pullFromBufferLin(LinRxBuffer,frame);
	  if (frame->ID == SLAVE_ADDR_READ){
		  serial_puts("Received a request via LIN");
		  serial_puts("Asking the comodo via can and sending the result back");

		  CAN_frame can_frame;
		  can_frame.ID = 0x10510112;
		  can_frame.RTR = 1;
		  can_frame.IDE = 1;
		  can_frame.DLC = 0;
		  pushToBufferCan(CanTxBuffer,can_frame);
		  osSemaphoreRelease(CanTxSemaphoreHandle);
	  }
  }
  /* USER CODE END lin_process */
}

/* USER CODE BEGIN Header_lin_send */
/**
* @brief Function implementing the Task_lin_send thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_lin_send */
void lin_send(void const * argument)
{
  /* USER CODE BEGIN lin_send */
	/* Infinite loop */
	for(;;){
		osSemaphoreWait(LinTxSemaphoreHandle,0); //Waiting for a semaphore
		LINMSG* frame;
		pullFromBufferLin(LinTxBuffer,frame);
		SendMessage(frame,LIN_MODE);
	}
  /* USER CODE END lin_send */
}

/* ask_comodo function */
void ask_comodo(void const * argument)
{
  /* USER CODE BEGIN ask_comodo */
	CAN_sendFrame(Ask_comodo_frame);
  /* USER CODE END ask_comodo */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

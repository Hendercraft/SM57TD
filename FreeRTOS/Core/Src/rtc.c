// -------------------------- HOW TO USE -----------------------------
//
// It's really easy. Follow these 4 steps:
// 1) Place rtc.c in Core/Src, and rtc.h in Core/Inc
// 2) Add #include "rtc.h" to your main.c
// 3) Call RTC_Init() in the "USER CODE BEGIN SysInit" tag
// 4) Congrats! You can now call the RTC_Print_Timestamp() function anywhere you want
//
// Don't forget to connect a 19200 bauds serial connection to the board.
// For example, you can use PuTTY and connect using /dev/ACM0
//
// -------------------------------------------------------------------

// No HAL include!! :)
#include "stm32f407xx.h"
#include "rtc.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// These globals are used for USART transmission with interrupts
// Pointer to data to transmit over USART
uint8_t *USART_tr_buffer;
// Needed to keep USART_tr_buffer original address to free it later
uint32_t USART_tr_buffer_offset = 0;
// Number of chars left to transmit over USART
uint32_t USART_tr_buffer_len = 0;

// ----------------------------- RTC ---------------------------------

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
void RTC_Init(void)
{
	// Init USART2
	USART2_Init();

	// Enable Power Clock
	__IO uint32_t tmpreg = 0x00U;
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	// Delay after an RCC peripheral clock enabling
	tmpreg = (RCC->APB1ENR & RCC_APB1ENR_PWREN);
	(void)tmpreg;

	// Enable LSI
	RCC->CSR |= RCC_CSR_LSION;

	// Wait until LSI is stable
	while ((RCC->CSR & RCC_CSR_LSIRDY) == 0) {}

	// Enable write access to Backup domain
	PWR->CR |= PWR_CR_DBP;

	// Confirm write access has been enabled
	while((PWR->CR & PWR_CR_DBP) == 0) {}

	// Reset the Backup domain only if the RTC Clock source is different from "No clock" and "LSI oscillator clock"
	uint32_t rtcsel = ((RCC->BDCR & RCC_BDCR_RTCSEL) >> RCC_BDCR_RTCSEL_Pos);
	if((rtcsel != 0b00) && (rtcsel != 0b10))
	{
	  // Store the content of BDCR register before the reset of Backup Domain
	  uint32_t old_bdcr = (RCC->BDCR & ~(RCC_BDCR_RTCSEL));

	  // RTC Clock selection can be changed only if the Backup Domain is reset
	  RCC->BDCR |= RCC_BDCR_BDRST;
	  RCC->BDCR &= ~RCC_BDCR_BDRST;

	  // Restore the Content of BDCR register
	  RCC->BDCR = old_bdcr;
	}

	// Select LSI oscillator as RTC clock source
	RCC->BDCR |= RCC_BDCR_RTCSEL_1;

	// Enable RTC clock
	RCC->BDCR |= RCC_BDCR_RTCEN;

	// Unlock write protection on RTC registers
	RTC->WPR = 0xCA;
	RTC->WPR = 0x53;

	// Enable RTC initialization mode
	RTC->ISR |= RTC_ISR_INIT;
	// Wait until the INITF bit is set (this should only take a few clock cycles)
	while((RTC->ISR & RTC_ISR_INITF) == 0) {}

	// SynchPrediv = 255, AsynchPrediv = 127
	RTC->PRER = 255;
	RTC->PRER |= (127 << RTC_PRER_PREDIV_A_Pos);

	// Check if the RTC calendar has already been initialized in a previous reset. If not, start the init sequence
	if ((RTC->ISR & RTC_ISR_INITS) == 0) {
		RTC_Calendar_Init();
	}

	// Exit initialization mode
	RTC->ISR &= ~RTC_ISR_INIT;

	// Re-enable RTC registers write protection
	RTC->WPR = 0xFF;
}

/**
  * @brief Sets the RTC calendar using USART prompts
  * @param None
  * @retval None
  */
void RTC_Calendar_Init(void)
{
	uint32_t date_tmp_reg = 0;
	uint32_t time_tmp_reg = 0;

	// Wait until the user confirms they are in front of a TTY terminal
	USART2_WaitUser();

	// Prompt time and date
	date_tmp_reg |= (RTC_Calendar_Init_Prompt("Enter day DD: ") << RTC_DR_DU_Pos);
	date_tmp_reg |= (RTC_Calendar_Init_Prompt("Enter month MM: ") << RTC_DR_MU_Pos);
	date_tmp_reg |= (RTC_Calendar_Init_Prompt("Enter year AA: ") << RTC_DR_YU_Pos);
	time_tmp_reg |= (RTC_Calendar_Init_Prompt("Enter hour HH: ") << RTC_TR_HU_Pos);
	time_tmp_reg |= (RTC_Calendar_Init_Prompt("Enter minutes MM: ") << RTC_TR_MNU_Pos);

	// Save values to registers
    RTC->TR = (uint32_t)(time_tmp_reg & (RTC_TR_HT | RTC_TR_HU | RTC_TR_MNT | RTC_TR_MNU | RTC_TR_ST  | RTC_TR_SU | RTC_TR_PM));
    RTC->DR = (uint32_t)(date_tmp_reg & (RTC_DR_YT | RTC_DR_YU | RTC_DR_MT | RTC_DR_MU | RTC_DR_DT | RTC_DR_DU | RTC_DR_WDU));
}

/**
  * @brief Prompts the user for a two-digits number and returns it as BCD
  * @param prompt a string literal that will be displayed to the user
  * @retval bcd the number typed as BCD
  */
uint8_t RTC_Calendar_Init_Prompt(char *prompt)
{
	// Print prompt
	serial_puts(prompt);

	// Wait for user input
	uint8_t input[10];
	USART2_Receive_line(input, 10, 1);
	USART2_NewLine();

	// Turn input to BCD and return it
	return c_to_bcd(input[0], input[1]);
}

/**
  * @brief Prints the current timestamp over USART
  * @retval None
  */
void RTC_Print_Timestamp(void)
{
	// Read date and time registers
	uint32_t time_tmp_reg = RTC->TR;
	uint32_t date_tmp_reg = RTC->DR;

	// Extract values from saved register values
	uint8_t day = bcd_to_int((date_tmp_reg & (RTC_DR_DU | RTC_DR_DT)) >> RTC_DR_DU_Pos);
	uint8_t month = bcd_to_int((date_tmp_reg & (RTC_DR_MU | RTC_DR_MT)) >> RTC_DR_MU_Pos);
	uint8_t year = bcd_to_int((date_tmp_reg & (RTC_DR_YU | RTC_DR_YT)) >> RTC_DR_YU_Pos);
	uint8_t hour = bcd_to_int((time_tmp_reg & (RTC_TR_HU | RTC_TR_HT)) >> RTC_TR_HU_Pos);
	uint8_t min = bcd_to_int((time_tmp_reg & (RTC_TR_MNU | RTC_TR_MNT)) >> RTC_TR_MNU_Pos);
	uint8_t sec = bcd_to_int((time_tmp_reg & (RTC_TR_SU | RTC_TR_ST)) >> RTC_TR_SU_Pos);

	// Format date to a nice string
	char *buffer = malloc(sizeof(char) * 100);
	sprintf(buffer, "Theft attempt!!! Date: %02d/%02d/%02d Time: %02d:%02d:%02d\r\n", day, month, year, hour, min, sec);

	// Send string over USART using interrupts
	while (USART2_transmit_IRQ((uint8_t *)buffer, strlen(buffer)) == -1) {}
}

/**
  * @brief Turn two chars into a 2-digit BCD (8 bits)
  * @param c_t tens digit
  * @param c_u units digit
  * @retval bcd the two chars coded as bcd. Any invalid char will be coded as a 0
  */
uint8_t c_to_bcd(char c_t, char c_u)
{
	uint8_t bcd_t = 0;
	uint8_t bcd_u = 0;
	uint8_t bcd = 0;

	// Tens
	if (c_t >= '0' && c_t <= '9') {
		bcd_t = (c_t - '0');
    }

	// Units
	if (c_u >= '0' && c_u <= '9') {
		bcd_u = (c_u - '0');
    }

	// Merge the two
	bcd = (bcd_t << 4) | bcd_u;

    return bcd;
}

/**
  * @brief Turn an 8-bit bcd number into an 8-bit binary number
  * @param bcd the binary coded decimal number
  * @retval n the binary number
  */
uint8_t bcd_to_int(uint8_t bcd)
{
	uint8_t n_t = (bcd & 0xF0) >> 4;
	uint8_t n_u = (bcd & 0x0F);
	uint8_t n = (n_t*10 + n_u);
	return n;
}

// ------------------------------- USART2 ---------------------------------

/**
  * @brief  Init USART2 at 19200 bauds
  * @retval None
  * */
void USART2_Init(void)
{
	//Initialize PA2 as USART2_TX and PA3 as USART2_RX
	//Activate A port
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    // Setup PA2 and PA3 as Alternate Function
	GPIOA->MODER |= 0x000000A0;
	GPIOA->MODER &= 0xFFFFFFAF;
	// Setup Alternate function as USART2
	GPIOA->AFR[0] &= 0xFFFF77FF;
	GPIOA->AFR[0] |= 0x00007700;
	// Push pull output
	GPIOA->OTYPER &= 0xFFFFFFF3;
	// Pull up resistor on
	GPIOA->PUPDR &= 0xFFFFFF5F;
	GPIOA->PUPDR |= 0x00000050;
	// Output speed set to VeryHigh
	GPIOA->OSPEEDR |= 0x000000F0;

	// Reset/de-reset USART2
	RCC->APB1RSTR |= RCC_APB1RSTR_USART2RST;
	RCC->APB1RSTR &= ~RCC_APB1RSTR_USART2RST;
	// Activate USART2 Clock
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	// Enable USART, no TE no RE yet, Oversampling = 8, 8bit mode, no parity
	// Enable Tx and Rx
	USART2->CR1 = 0x0000802C;
	// No LIN mode, No clock output (synchronous mode)
	USART2->CR2 = 0x00000000;
	// No control mode, 3 sample points
	USART2->CR3 = 0x00000000;
	// 19200bauds -> USARTDIV = 273.4375 -> Mantissa = 273d=0x111 , Fraction = 0.4375*16 = 7d = 0x7
	USART2->BRR = 0x00001117;
	// Enable UART. Tx interrupts will only be enabled when there is a transmission to do
	USART2->CR1 = 0x0000A00C;

	// Wait around 0.1ms. No HAL so we're doing it the old-fashioned way.
	// Note that we're using volatile to prevent the compiler from optimising the loop away
	volatile uint32_t n = 0;
	while (n < 1680) {
		++n;
	}

	// Enable USART2 interrupt
	NVIC_SetPriority(USART2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
	NVIC_EnableIRQ(USART2_IRQn);
}

/**
  * @brief  Send char to host through USART2 using the USART_tr_buffer global variable
  * @retval the character sent or \0 if transmission is over
  * */
int __io_putchar(void)
{
	// If transmission is over
	if (USART_tr_buffer_len == 0) {
		// Disable TXE interrupts
		USART2->CR1 &= ~USART_CR1_TXEIE;

		free(USART_tr_buffer);

		return '\0';
	}

	// Decrement number of characters left
	USART_tr_buffer_len--;

	// Fetch the char to send
	uint8_t tx_ch = *(USART_tr_buffer + USART_tr_buffer_offset);

	// Increment buffer pointer offset
	USART_tr_buffer_offset++;

	// Send char with data register (note that we only keep the low byte of ch,
	// because DR[31:9] must be kept at reset value according to the datasheet
	USART2->DR = USART_DR_DR_Msk & tx_ch;

	return tx_ch;
}

/**
  * @brief  Send string to host through USART2
  * @param  data pointer to a data buffer to send over USART. Will be freed once transmission is over
  * @param  len number of chars to send
  * @retval -1 if the transmission failed (there is already one in progress), 0 otherwise
  * */
int32_t USART2_transmit_IRQ(uint8_t *data, uint32_t len)
{
	// If a transmission is already in progress, return with an error
	if (((USART2->CR1 & USART_CR1_TXEIE) != 0) || (USART_tr_buffer_len > 0)) {
		return -1;
	}

	// Setup globals
	USART_tr_buffer = data;
	USART_tr_buffer_offset = 0;
	USART_tr_buffer_len = len;

	// Send first char. Note that this write to USART2->DR clears the TXE flag
	__io_putchar();

	// Enable TXE interrupts
	USART2->CR1 |= USART_CR1_TXEIE;

	return 0;
}

/**
  * @brief  Send string to host through USART2 using interrupts. If a transmission is already in progress, the function waits
  * @param 	msg the string to send
  * @retval None
  * */

/**
  * @brief  Receive a single char from host through USART2. If none is available, the function waits
  * @retval int the received char
  * */
int __io_getchar(void)
{
	// Wait until the data register receives data
	while ((USART2->SR & USART_SR_RXNE) == 0) {}

	// Read and return the lowest byte of DR
	return (USART2->DR & 0xFF);
}

/**
  * @brief  Receive str from host through USART2. Reads up to a newline char or until max_len-1 characters have been read
  * @param  data buffer address. The newline char is not returned
  * @param 	max_len the size of the buffer
  * @param 	feedback_enable whether or not to send typed chars back over USART. 1 to enable, 0 to disable
  * @retval nb_rx number of chars received
  * */
int32_t USART2_Receive_line(uint8_t *data, uint32_t max_len, uint8_t feedback_enable)
{
	// Number of chars received
	uint32_t nb_rx = 0;

	// Read until a newline char is received
	while (1) {
		// Wait and read the next char
		char rx_ch = __io_getchar();

		// If it is a newline, exit loop
		if ((rx_ch == '\n') || (rx_ch == '\r')) {
			break;
		};

		if (rx_ch == 0x7F) {
			// Backspace support
			if (nb_rx > 0) {
				nb_rx--;
			}
			else {
				// Skip feedback
				continue;
			}
		}
		else {
			// Save received char in buffer
			data[nb_rx] = rx_ch;
			nb_rx++;
		}

		// If enabled, send the character back over USART
		if (feedback_enable) {
			// Wait until there are no transmissions in progress
			while (((USART2->CR1 & USART_CR1_TXEIE) != 0) || ((USART2->SR & USART_SR_TXE) == 0)) {}

			// Push char to data register
			USART2->DR = USART_DR_DR_Msk & rx_ch;

			// Wait until char has been sent
			while ((USART2->SR & USART_SR_TXE) == 0) {}
		}

		// If the max number of chars has been reached, exit loop
		if (nb_rx >= (max_len-1)) {
			break;
		}
	}

	// Insert \0
	data[nb_rx] = '\0';

	return nb_rx;
}

/**
  * @brief  Print prompt until the user confirms the TTY terminal is open by pressing enter
  * @retval None
  * */
void USART2_WaitUser(void)
{
	while (1) {
		// Print prompt
		serial_puts("Press enter to begin...\r\n");

		// Poll for a received char
		if ((USART2->SR & USART_SR_RXNE) != 0) {
			// Retreive char. If it's a newline, exit loop
			char rx_ch = __io_getchar();
			if ((rx_ch == '\n') || (rx_ch == '\r')) {
				break;
			};
		}

		// Wait around 1s between each prompt. No HAL so we're doing it the old-fashioned way.
		// Note that we're using volatile to prevent the compiler from optimising the loop away
		volatile uint32_t n = 0;
		while (n < 16800000) {
			++n;
		}
	}
}

/**
  * @brief  Print newline over USART
  * @retval None
  * */
void USART2_NewLine(void)
{
	serial_puts("\n\r");
}


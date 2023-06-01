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

#ifndef RTCH
#define RTCH

// Function Prototypes

// RTC
void RTC_Init(void);
void RTC_Calendar_Init(void);
uint8_t RTC_Calendar_Init_Prompt(char* prompt);
void RTC_Print_Timestamp(void);
uint8_t c_to_bcd(char c_t, char c_u);
uint8_t bcd_to_int(uint8_t bcd);

// USART2
void USART2_Init(void);
int __io_putchar(void);
int32_t USART2_transmit_IRQ(uint8_t *data, uint32_t len);
void serial_puts(char * msg);
int __io_getchar(void);
int32_t USART2_Receive_line(uint8_t *data, uint32_t max_len, uint8_t feedback_enable);
void USART2_WaitUser(void);
void USART2_NewLine(void);


#endif /* RTCH */
